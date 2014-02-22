#pragma once
/******************************************************************
*                                                                 *
* GraphicObj.h - Declare and define all of the core graphics      *
*                                                                 *
*******************************************************************
*                                                                 *
* Includes and defines                                            *
*                                                                 *
******************************************************************/

#include "All.h"

#define D2D1_COLOR(x) D2D1::ColorF(D2D1::ColorF::x)

/******************************************************************
*                                                                 *
*  End Includes and defines                                       *
*                                                                 *
*******************************************************************
*                                                                 *
*  Main Graphic Object                                            *
*                                                                 *
******************************************************************/

class GraphicObj
{
public:
	struct comp : binary_function <GraphicObj*,GraphicObj*,bool> {
	bool operator() (const GraphicObj *x, const GraphicObj* y) const
		{return x->layerId < y->layerId;}
	};
	typedef enum GO_Types
	{
		Image,
		Text,
		Rect
	} GO_Types;
	GraphicObj(
		D2D1_SIZE_U size = D2D1::SizeU(10, 10),
		D2D1_POINT_2F pos = D2D1::Point2F(),
		BYTE layer = DEFAULT_LAYER
		);
	~GraphicObj();
	virtual const void DrawObject() = 0;
	virtual GO_Types GetType() = 0;
	virtual void SetSize(D2D1_SIZE_U size);
	const D2D1_SIZE_U GetSize();
	virtual void SetPos(D2D1_POINT_2F pos);
	const D2D1_POINT_2F GetPos();
	const BYTE GetLayerID();
	virtual HRESULT Initialize() = 0;
	virtual void Uninitialize() = 0;

	static multiset<GraphicObj*, comp> listOfGO;
	static void ResetListOfGO();
	bool toRender;
protected:
	D2D1_POINT_2F pos;
	D2D1_SIZE_U size;
private:
	BYTE layerId;
};

/******************************************************************
*                                                                 *
*  End Main Graphic Object                                        *
*                                                                 *
*******************************************************************
*                                                                 *
*  Bitmap Graphic Object                                          *
*  Inherited from GraphicObj                                      *
*                                                                 *
******************************************************************/

class BitmapGraphicObj : public GraphicObj
{
public:
	BitmapGraphicObj(
		wstring resName = wstring(),
		D2D1_SIZE_U size = D2D1::SizeU(10, 10),
		D2D1_POINT_2F pos = D2D1::Point2F(),
		BYTE layer = DEFAULT_LAYER
		);
	~BitmapGraphicObj();
	virtual HRESULT Initialize();
	virtual void Uninitialize();
	virtual const void DrawObject();
	virtual GO_Types GetType();
protected:
	wstring resName;
	ID2D1Bitmap *D2D_Bitmap;
};

/******************************************************************
*                                                                 *
*  End Bitmap Graphic Object                                      *
*                                                                 *
*******************************************************************
*                                                                 *
*  Rectangle Graphic Object                                       *
*  Inherited from GraphicObj                                      *
*                                                                 *
******************************************************************/

class RectGraphicObj : public GraphicObj
{
public:
	RectGraphicObj(
		D2D1_COLOR_F color = D2D1_COLOR(White),
		float strokeSize = 1.0f,
		D2D1_COLOR_F strokeColor = D2D1_COLOR(Black),
		D2D1_SIZE_U size = D2D1::SizeU(10, 10),
		D2D1_POINT_2F pos = D2D1::Point2F(),
		BYTE layer = DEFAULT_LAYER
		);
	~RectGraphicObj();
	virtual HRESULT Initialize();
	virtual void Uninitialize();
	void SetSize(D2D1_SIZE_U size);
	void SetPos(D2D1_POINT_2F pos);
	virtual const void DrawObject();
	virtual GO_Types GetType();
protected:
	ID2D1RectangleGeometry *D2D_Rect;
	ID2D1SolidColorBrush *D2D_ColorBrush;
	ID2D1SolidColorBrush *D2D_StrokeBrush;
private:
	D2D1_COLOR_F color;
	D2D1_COLOR_F strokeColor;
	float strokeSize;
};

/******************************************************************
*                                                                 *
*  End Rectangle Graphic Object                                   *
*                                                                 *
*******************************************************************
*                                                                 *
*  Textual Graphic Object                                         *
*  Inherited from GraphicObj                                      *
*                                                                 *
******************************************************************/

class TextGraphicObj : public GraphicObj
{
public:
	TextGraphicObj(
					wstring text = wstring(),
					D2D1_SIZE_U size = D2D1::SizeU(10, 10),
					D2D1_POINT_2F pos = D2D1::Point2F(),
					BYTE layer = DEFAULT_LAYER,
					D2D1_COLOR_F color = D2D1_COLOR(Black),
					float fontSize = 22.0f,
					DWRITE_TEXT_ALIGNMENT textAlign = DWRITE_TEXT_ALIGNMENT_LEADING,
					DWRITE_PARAGRAPH_ALIGNMENT paraAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
					DWRITE_READING_DIRECTION readDir = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT,
					wstring fontFamilyName = L"Arial",
					DWRITE_FONT_WEIGHT  fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
					DWRITE_FONT_STYLE  fontStyle = DWRITE_FONT_STYLE_NORMAL,
					DWRITE_FONT_STRETCH  fontStretch = DWRITE_FONT_STRETCH_NORMAL
					);
	~TextGraphicObj();
	virtual HRESULT Initialize();
	virtual void Uninitialize();
	virtual const void DrawObject();
	virtual GO_Types GetType();
	void SetText(wstring text);
	const wstring GetText();
protected:
	wstring text;
	IDWriteTextFormat *DW_TextFormat;
	ID2D1SolidColorBrush *D2D_ColorBrush;
private:
	D2D1_COLOR_F color;
};

/******************************************************************
*                                                                 *
*  End Textual Graphic Object                                     *
*                                                                 *
*******************************************************************
*                                                                 *
*  End GrphicObj.h                                                *
*                                                                 *
******************************************************************/