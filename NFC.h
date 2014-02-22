#pragma once
/******************************************************************
*                                                                 *
* NFC.h - Declare and define the NFC interface                    *
*                                                                 *
*******************************************************************
*                                                                 *
* Includes and defines                                            *
*                                                                 *
******************************************************************/

#include "All.h"

/******************************************************************
*                                                                 *
*  End Includes and defines                                       *
*                                                                 *
*******************************************************************
*                                                                 *
*  CardData Struct                                                *
*                                                                 *
******************************************************************/

typedef struct CardData
{
	char username[16];
	char password[16];
} CardData;

/******************************************************************
*                                                                 *
*  End CardData                                                   *
*                                                                 *
*******************************************************************
*                                                                 *
*  NFC Class                                                      *
*                                                                 *
******************************************************************/

class NFC
{
public:
	NFC();
	~NFC();

	void Initialize();
	void Uninitialize();
	CardData Read();
	void Write(CardData data);

private:
	typedef union GUID2BYTE
	{
		char data[16];
		BYTE bData[16];
	} GUID2BYTE;

	void ResetBuffer(BYTE *buf, int n);
	void SetBuffer(BYTE *buf, int n, ...);
	SCARDCONTEXT hContext;
	SCARDHANDLE hScard;
	DWORD activeProtocol;
	wstring readerName;
};

/******************************************************************
*                                                                 *
*  End NFC Class                                                  *
*                                                                 *
*******************************************************************
*                                                                 *
*  End NFC.h                                                      *
*                                                                 *
******************************************************************/