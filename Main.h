#pragma once
/******************************************************************
*                                                                 *
* Main.h - Declare and define all of the core App features        *
*                                                                 *
*******************************************************************
*                                                                 *
* Includes and defines                                            *
*                                                                 *
******************************************************************/

#include "All.h"
#include "Input.h"
#include "Network.h"
#include "NFC.h"

#define D2D_FACTORY App::ins->D2D_Factory
#define WIC_FACTORY App::ins->WIC_Factory
#define DW_FACTORY App::ins->DW_Factory
#define D2D_RENDERTARGET App::ins->D2D_RenderTarget
#define SCREEN_SIZE App::ins->GetSize()
#define NFC_READER App::ins->reader

/******************************************************************
*                                                                 *
*  End Includes and defines                                       *
*                                                                 *
*******************************************************************
*                                                                 *
*  Main App Class                                                 *
*                                                                 *
******************************************************************/

class App
{
public:
	typedef enum Screens
	{
		Null,
		WaitingToCardScreen,
	} Screens;

	App();
	~App();

	// Creates the application window and initializes device-independent resources.
	HRESULT Initialize();

	// Update the App in a tick.
	void Update();

	// Rander a frame.
	HRESULT Render();

	// Get the rander target size.
	D2D1_SIZE_F GetSize();

	// Gets the latest error message
	string GetLastErrorMsg();

	// Changes the screen
	void ChangeScreenTo(Screens screen);

	int AppMode;
	static App *ins; // Static instance of the main App class for usage from across the progarm
	HWND m_hwnd; // Handle to the main window
	ID2D1HwndRenderTarget *D2D_RenderTarget; // Direct2D Render Target
	Screens changeTo;
	NFC *reader;

private:
	// Create resources which are not bound to any device. Their lifetime effectively extends for the duration of the app.
	// These resources include the Direct2D, DirectWrite, and WIC factories; and a DirectWrite Text Format object
	// (used for identifying particular font characteristics) and a Direct2D geometry.
	HRESULT CreateDeviceIndependentResources();

	// This method creates resources which are bound to a particular Direct3D device.
	// It's all centralized here, in case the resources need to be recreated in case of Direct3D device loss
	// (eg. display change, remoting, removal of video card, alt-tab, etc).
	HRESULT CreateDeviceResources();

	// Discard device-specific resources which need to be recreated when a Direct3D device is lost
	void DiscardDeviceResources();

	//  If the application receives a WM_SIZE message, this method resize the render target appropriately.
	void OnResize(
		UINT width,
		UINT height
		);

	// Windows Messages Handler
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);

	// Creates a Direct2D bitmap from a resource in the application resource file.
	HRESULT LoadResourceBitmap(
		ID2D1RenderTarget *pRenderTarget,
		IWICImagingFactory *pIWICFactory,
		PCWSTR resourceName,
		PCWSTR resourceType,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap **ppBitmap
		);

	string lastError;
	// Factories
	ID2D1Factory *D2D_Factory; // Direct2D Factory
	IWICImagingFactory *WIC_Factory; // Windows Imageing Componets Factory
	IDWriteFactory *DW_Factory; // DirectWrite Factory

	// Friends
	friend class Error;
	friend class BitmapGraphicObj;
	friend class RectGraphicObj;
	friend class TextGraphicObj;
};

/******************************************************************
*                                                                 *
*  End Main App Class                                             *
*                                                                 *
*******************************************************************
*                                                                 *
*  End Main.h                                                     *
*                                                                 *
******************************************************************/