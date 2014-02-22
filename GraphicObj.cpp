#include "GraphicObj.h"
#include "Main.h"

multiset<GraphicObj*, GraphicObj::comp> GraphicObj::listOfGO;

void GraphicObj::ResetListOfGO()
{
	multiset<GraphicObj*, GraphicObj::comp> temp = GraphicObj::listOfGO;
	for (auto it = temp.begin(); it != temp.end(); ++it)
	{
		switch ((*it)->GetType())
		{
		case GraphicObj::Image:
			delete (BitmapGraphicObj*)(*it);
			break;
		case GraphicObj::Rect:
			delete (RectGraphicObj*)(*it);
			break;
		case GraphicObj::Text:
			delete (TextGraphicObj*)(*it);
			break;
		default:
			delete *it;
			break;
		}
	}
}

GraphicObj::GraphicObj(D2D1_SIZE_U size, D2D1_POINT_2F pos, BYTE layer)
{
	layerId = layer;
	listOfGO.insert(this);
	this->pos = pos;
	this->size = size;
	toRender = true;
}

GraphicObj::~GraphicObj()
{
	for (multiset<GraphicObj*>::iterator it = listOfGO.begin(); it != listOfGO.end(); ++it)
		if (this == (*it))
		{
			listOfGO.erase(it);
			break;
		}
}

void GraphicObj::SetSize(D2D1_SIZE_U size)
{
	this->size = size;
}

const D2D1_SIZE_U GraphicObj::GetSize()
{
	return size;
}

void GraphicObj::SetPos(D2D1_POINT_2F pos)
{
	this->pos = pos;
}

const D2D1_POINT_2F GraphicObj::GetPos()
{
	return pos;
}

const BYTE GraphicObj::GetLayerID()
{
	return layerId;
}


BitmapGraphicObj::BitmapGraphicObj(wstring resName, D2D1_SIZE_U size, D2D1_POINT_2F pos, BYTE layer)
	:GraphicObj(size, pos, layer),
	 D2D_Bitmap(NULL)
{
	this->resName = resName;

	Initialize();
}

BitmapGraphicObj::~BitmapGraphicObj()
{
	Uninitialize();
}

HRESULT BitmapGraphicObj::Initialize()
{
	HRESULT hr = App::ins->LoadResourceBitmap(
		D2D_RENDERTARGET,
		WIC_FACTORY,
		resName.c_str(),
		L"Image",
		size.width,
		size.height,
		&D2D_Bitmap
		);
	
	return hr;
}

void BitmapGraphicObj::Uninitialize()
{
	SafeRelease(&D2D_Bitmap);
}

const void BitmapGraphicObj::DrawObject()
{
	if( D2D_Bitmap )
	{
		D2D_RENDERTARGET->DrawBitmap(D2D_Bitmap, D2D1::RectF(
			pos.x,
			pos.y,
			pos.x + size.width,
			pos.y + size.height
			));
	}
}

GraphicObj::GO_Types BitmapGraphicObj::GetType()
{
	return Image;
}


RectGraphicObj::RectGraphicObj(D2D1_COLOR_F color, float strokeSize, D2D1_COLOR_F strokeColor, D2D1_SIZE_U size, D2D1_POINT_2F pos, BYTE layer)
	:GraphicObj(size, pos, layer),
	D2D_Rect(NULL),
	D2D_ColorBrush(NULL),
	D2D_StrokeBrush(NULL)
{
	this->color = color;
	this->strokeColor = strokeColor;
	this->strokeSize = strokeSize;

	D2D_FACTORY->CreateRectangleGeometry(
		D2D1::RectF(pos.x, pos.y, pos.x + size.width, pos.y + size.height),
		&D2D_Rect
		);

	Initialize();
}

RectGraphicObj::~RectGraphicObj()
{
	Uninitialize();
	SafeRelease(&D2D_Rect);
}

HRESULT RectGraphicObj::Initialize()
{
	HRESULT hr = D2D_RENDERTARGET->CreateSolidColorBrush(color, &D2D_ColorBrush);
	if (SUCCEEDED(hr))
		hr = D2D_RENDERTARGET->CreateSolidColorBrush(strokeColor, &D2D_StrokeBrush);

	return hr;
}

void RectGraphicObj::Uninitialize()
{
	SafeRelease(&D2D_ColorBrush);
	SafeRelease(&D2D_StrokeBrush);
}

void RectGraphicObj::SetSize(D2D1_SIZE_U size)
{
	SafeRelease(&D2D_Rect);
	D2D_FACTORY->CreateRectangleGeometry(
		D2D1::RectF(pos.x, pos.y, pos.x + size.width, pos.y + size.height),
		&D2D_Rect
		);
	this->size = size;
}

void RectGraphicObj::SetPos(D2D1_POINT_2F pos)
{
	SafeRelease(&D2D_Rect);
	D2D_FACTORY->CreateRectangleGeometry(
		D2D1::RectF(pos.x, pos.y, pos.x + size.width, pos.y + size.height),
		&D2D_Rect
		);
	this->pos = pos;
}

const void RectGraphicObj::DrawObject()
{
	D2D_RENDERTARGET->FillGeometry(D2D_Rect, D2D_ColorBrush);
	if (strokeSize > 0)
		D2D_RENDERTARGET->DrawGeometry(D2D_Rect, D2D_StrokeBrush);
}

GraphicObj::GO_Types RectGraphicObj::GetType()
{
	return Rect;
}


TextGraphicObj::TextGraphicObj(wstring text, D2D1_SIZE_U size, D2D1_POINT_2F pos, BYTE layer, D2D1_COLOR_F color, float fontSize, DWRITE_TEXT_ALIGNMENT textAlign, DWRITE_PARAGRAPH_ALIGNMENT paraAlign, DWRITE_READING_DIRECTION readDir, wstring fontFamilyName, DWRITE_FONT_WEIGHT  fontWeight, DWRITE_FONT_STYLE  fontStyle, DWRITE_FONT_STRETCH  fontStretch)
	:GraphicObj(size, pos, layer)
{
	this->color = color;
	this->text = text;

	// Create a DirectWrite text format object.
	HRESULT hr = DW_FACTORY->CreateTextFormat(
		fontFamilyName.c_str(),
		NULL,
		fontWeight,
		fontStyle,
		fontStretch,
		fontSize,
		L"", //locale
		&DW_TextFormat
		);

	if (FAILED(hr))
		throw Error(Error::DW_ERROR_CREATE_TEXT_FORMAT);

	DW_TextFormat->SetTextAlignment(textAlign);
	DW_TextFormat->SetParagraphAlignment(paraAlign);
	DW_TextFormat->SetReadingDirection(readDir);

	Initialize();
}

TextGraphicObj::~TextGraphicObj()
{
	Uninitialize();
}

HRESULT TextGraphicObj::Initialize()
{
	HRESULT hr = D2D_RENDERTARGET->CreateSolidColorBrush(color, &D2D_ColorBrush);

	return hr;
}

void TextGraphicObj::Uninitialize()
{
	SafeRelease(&D2D_ColorBrush);
}

const void TextGraphicObj::DrawObject()
{
	D2D_RENDERTARGET->DrawText(
		text.c_str(),
		text.size(),
		DW_TextFormat,
		D2D1::RectF(pos.x, pos.y, pos.x + size.width, pos.y + size.height),
		D2D_ColorBrush,
		D2D1_DRAW_TEXT_OPTIONS_CLIP
		);
}

GraphicObj::GO_Types TextGraphicObj::GetType()
{
	return Text;
}

void TextGraphicObj::SetText(wstring text)
{
	this->text = text;
}

const wstring TextGraphicObj::GetText()
{
	return text;
}