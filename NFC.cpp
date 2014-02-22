#include "NFC.h"

NFC::NFC()
{
	wchar_t buf[1024];
	DWORD a = 1024;
	SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
	SCardListReaders(hContext, NULL, buf, &a);
	readerName = buf;
}

NFC::~NFC()
{
	this->Uninitialize();
	SCardReleaseContext(hContext);
}

void NFC::ResetBuffer(BYTE *buf, int n)
{
	for (int i = 0; i < n; i++)
		buf[i] = 0;
}

void NFC::SetBuffer(BYTE *buf, int n, ...)
{
	va_list ap;
	va_start(ap, n);
	for (int i = 0; i < n; i++)
		buf[i] = va_arg(ap, BYTE);
	va_end(ap);
}

void NFC::Initialize()
{
	HRESULT iRes = 0;
	BYTE inBuf[16];
	BYTE outBuf[16];
	DWORD len = 2;
	iRes = SCardConnect(
		hContext,
		readerName.c_str(),
		SCARD_SHARE_SHARED,
		SCARD_PROTOCOL_T1,
		&hScard,
		&activeProtocol
		);
	if (iRes != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		throw Error(Error::NFC_NO_CARD);
	}

	SCardBeginTransaction(hScard);

	SetBuffer(inBuf, 11, 0xFF, 0x82, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
	iRes = SCardTransmit(
		hScard,
		SCARD_PCI_T1,
		inBuf,
		11,
		NULL,
		outBuf,
		&len
	);
}

void NFC::Uninitialize()
{
	SCardEndTransaction(hScard, SCARD_LEAVE_CARD);
	SCardDisconnect(hScard, SCARD_LEAVE_CARD);
}

CardData NFC::Read()
{
	CardData out;
	GUID2BYTE temp;
	HRESULT iRes = 0;
	BYTE inBuf[6], outBuf[0x12];
	DWORD len = 0;

	len = 2;
	SetBuffer(inBuf, 6, 0xFF, 0x88, 0x00, 0x04, 0x61, 0x00);
	iRes = SCardTransmit(
		hScard,
		SCARD_PCI_T1,
		inBuf,
		6,
		NULL,
		outBuf,
		&len
	);

	Sleep(1000);
	
	// read the card id
	len = 0x12;
	ResetBuffer(inBuf, 6);
	SetBuffer(inBuf, 5, 0xFF, 0xB0, 0x00, 0x04, 0x10);
	iRes = SCardTransmit(
		hScard,
		SCARD_PCI_T1,
		inBuf,
		5,
		NULL,
		outBuf,
		&len
	);
	for (int i = 0; i < 16; i++) temp.bData[i] = outBuf[i];
	strcpy_s(out.username, temp.data);


	Sleep(1000);

	// read the card vri code
	len = 0x12;
	ResetBuffer(inBuf, 6);
	SetBuffer(inBuf, 5, 0xFF, 0xB0, 0x00, 0x05, 0x10);
	iRes = SCardTransmit(
		hScard,
		SCARD_PCI_T1,
		inBuf,
		5,
		NULL,
		outBuf,
		&len
	);
	for (int i = 0; i < 16; i++) temp.bData[i] = outBuf[i];
	strcpy_s(out.password, temp.data);

	return out;
}

void NFC::Write(CardData data)
{
	GUID2BYTE temp;
	HRESULT iRes = 0;
	BYTE inBuf[21], outBuf[2];
	DWORD len = 0;

	len = 2;
	SetBuffer(inBuf, 6, 0xFF, 0x88, 0x00, 0x04, 0x61, 0x00);
	iRes = SCardTransmit(
		hScard,
		SCARD_PCI_T1,
		inBuf,
		6,
		NULL,
		outBuf,
		&len
	);
	
	// write the card id
	len = 2;
	ResetBuffer(inBuf, 6);
	SetBuffer(inBuf, 5, 0xFF, 0xD6, 0x00, 0x04, 0x10);
	strcpy_s(temp.data, data.username);
	for (int i = 0; i < 16; i++) inBuf[5+i] = temp.bData[i];
	iRes = SCardTransmit(
		hScard,
		SCARD_PCI_T1,
		inBuf,
		21,
		NULL,
		outBuf,
		&len
	);
	if (outBuf[0] == 0x63) // it's Ultralight
	{
		// TODO handle it
		ResetBuffer(inBuf, 21);
		for (int i = 1; i <= 4; i++)
		{
			len = 2;
			SetBuffer(inBuf, 5, 0xFF, 0xD6, 0x00, i*0x04, 0x04);
			for (int j = 0; j < 4; j++)
				inBuf[5+j] = temp.bData[(4*(i-1))+j];
			iRes = SCardTransmit(
				hScard,
				SCARD_PCI_T1,
				inBuf,
				9,
				NULL,
				outBuf,
				&len
			);
		}
	}

	// write the card vri code
	len = 2;
	ResetBuffer(inBuf, 6);
	SetBuffer(inBuf, 5, 0xFF, 0xD6, 0x00, 0x05, 0x10);
	strcpy_s(temp.data, data.password);
	for (int i = 0; i < 16; i++) inBuf[5+i] = temp.bData[i];
	iRes = SCardTransmit(
		hScard,
		SCARD_PCI_T1,
		inBuf,
		21,
		NULL,
		outBuf,
		&len
	);
}