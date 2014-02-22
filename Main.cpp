#include "Main.h"
#include "UIEntity.h"
#include "Json.h"
#include "NFC.h"
#include "Network.h"

int WINAPI wWinMain(HINSTANCE /* hInstance */, HINSTANCE /* hPrevInstance */, LPWSTR lpCmdLine, int /* nCmdShow */)
{
	LPWSTR* arg_list;
	int arg_num = 0;

    // Ignore the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
		App *app = new App();
		arg_list = CommandLineToArgvW(lpCmdLine, &arg_num);
		if (arg_num == 2)
		{
			CardData data;
			strcpy_s(data.username, ws2s(arg_list[0]).c_str());
			strcpy_s(data.password, ws2s(arg_list[1]).c_str());

			try
			{
				NFC_READER->Initialize();
				NFC_READER->Write(data);
				NFC_READER->Uninitialize();
			}
			catch (...)
			{
				MessageBox(NULL, L"שגיאה התרחשה בכתיבה לכרטיס או שלא נמצא כרטיס...", L"שים/י לב!", STDMSGBOX | MB_ICONWARNING);
				return 1;
			}
			return 0;
		}
        if (SUCCEEDED(app->Initialize()))
        {
			app->ChangeScreenTo(App::Screens::WaitingToCardScreen);
			MSG msg;
			memset(&msg, 0, sizeof(msg));
			while (app->AppMode != -1)
			{
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				app->Update();
				app->Render();
			}
        }
		delete app;
        CoUninitialize();
    }
	
    return 0;
}

void App::Update()
{
	wstring recv;
	CardData data;
	wstringstream oss;

	try
	{
		reader = new NFC;
		reader->Initialize();
		data = reader->Read();
		reader->Uninitialize();
		delete reader;
	}
	catch (...)
	{
		delete reader;
		return;
	}

	oss << L"http://46.120.171.183/auth/?username=" << data.username << L"&password=" << data.password;

	ShellExecute(NULL, L"open", oss.str().c_str(), NULL, NULL, SW_SHOWNORMAL);

	exit(0);
}

void App::ChangeScreenTo(Screens screen)
{
	TextGraphicObj::ResetListOfGO();
	switch (screen)
	{
	case App::WaitingToCardScreen:
		new UI_Image(
			L"Logo",
			D2D1::SizeU(static_cast<int>((438.0/383.0)*280), 280),
			D2D1::Point2F(65, 35)
			);
		new UI_Rect(
			D2D1_COLOR(White),
			1.0f,
			D2D1::ColorF(0x333333),
			D2D1::SizeU(1, 1000),
			D2D1::Point2F(450, 0)
			);
		new UI_Text(
			L"התחבר...",
			D2D1::SizeU(500, 75),
			D2D1::Point2F(350, 0),
			DEFAULT_LAYER,
			D2D1::ColorF(0x333333),
			68.0f
			);
		new UI_Text(
			L"ברוך הבא משתמש/ת יקר!\n\nלצורך התחברות אנא שים כרטיס התחברות על הקורא והמתן בסבלנות עד אשר האתר יטען.",
			D2D1::SizeU(390, 150),
			D2D1::Point2F(460, 100),
			DEFAULT_LAYER,
			D2D1::ColorF(0x333333),
			28.0f
			);
		break;

	default:
		break;
	}
}

App *App::ins = 0;

App::App() :
    m_hwnd(NULL),
    D2D_RenderTarget(NULL),
	D2D_Factory(NULL),
	WIC_Factory(NULL),
	DW_Factory(NULL)
{
	ins = this;
	AppMode = 1;
	changeTo = Null;
	reader = new NFC;
}

App::~App()
{ 
	DiscardDeviceResources();
	GraphicObj::ResetListOfGO();
	SafeRelease(&D2D_Factory);
	SafeRelease(&WIC_Factory);
	SafeRelease(&DW_Factory);
}

HRESULT App::Initialize()
{
	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	HRESULT hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(wcex) };
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = WndProc;
		wcex.cbWndExtra    = sizeof(LONG_PTR);
		wcex.hInstance     = HINST_THISCOMPONENT;
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"App";

		RegisterClassEx(&wcex);

		// Create the application window.
		//
		// Because the CreateWindow function takes its size in pixels, we
		// obtain the system DPI and use it to scale the window size.
		FLOAT dpiX, dpiY;
		D2D_Factory->GetDesktopDpi(&dpiX, &dpiY);

		// Create the application window.
		m_hwnd = CreateWindow(
			L"App",
			WINDOW_TITLE,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			50,
			50,
			static_cast<UINT>(ceil(900 * dpiX / 96.f)),
			static_cast<UINT>(ceil(400 * dpiY / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
			);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			this->CreateDeviceResources();
			UpdateWindow(m_hwnd);
		}
	}
	return hr;
}

HRESULT App::CreateDeviceIndependentResources()
{
	HRESULT hr;

	// Create a Direct2D factory.
#if defined(DEBUG) || defined(_DEBUG)
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options,
		&D2D_Factory
		);
#else
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_MULTI_THREADED,
		&D2D_Factory
		);
#endif
	if (SUCCEEDED(hr))
	{
		// Create WIC factory.
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&WIC_Factory)
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(DW_Factory),
			reinterpret_cast<IUnknown **>(&DW_Factory)
			);
	}

    return hr;
}

HRESULT App::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!D2D_RenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		// Create a Direct2D render target.
		hr = D2D_Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size, D2D1_PRESENT_OPTIONS_NONE),
			&D2D_RenderTarget
			);

		for (auto it = GraphicObj::listOfGO.begin();
			it != GraphicObj::listOfGO.end();
			++it)
			if (SUCCEEDED(hr))
				hr = (*it)->Initialize();
    }

    return hr;
}

void App::DiscardDeviceResources()
{
	SafeRelease(&D2D_RenderTarget);
	for (auto it = GraphicObj::listOfGO.begin(); it != GraphicObj::listOfGO.end(); ++it)
		(*it)->Uninitialize();
}

HRESULT App::Render()
{
	HRESULT hr = S_OK;
	D2D_RenderTarget->BeginDraw();

	// Safe fail BG
	D2D_RenderTarget->Clear(D2D1::ColorF(0xC6C6C6));

	for (auto it = GraphicObj::listOfGO.begin(); it != GraphicObj::listOfGO.end(); ++it)
		if ((*it)->toRender)
			(*it)->DrawObject();

	hr = D2D_RenderTarget->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}
	return hr;
}

D2D1_SIZE_F App::GetSize()
{
	return D2D1::SizeF(
		static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
		static_cast<float>(GetSystemMetrics(SM_CYSCREEN))
		);
}

void App::OnResize(UINT width, UINT height)
{
    if (D2D_RenderTarget)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        // Note: This method can fail, but it's okay to ignore the
        // error here -- it will be repeated on the next call to
        // EndDraw.
        D2D_RenderTarget->Resize(size);
    }
}

LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        App *pApp = (App *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pApp)
            );

        result = 1;
    }
    else
    {
        App *pApp = reinterpret_cast<App *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                {
                    pApp->Render();

                    ValidateRect(hwnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
					ins->AppMode = -1;
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

HRESULT App::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    )
{
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);
    HRESULT hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
        
    }
    if (SUCCEEDED(hr))
    {
          // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                            pSource,
                            destinationWidth,
                            destinationHeight,
                            WICBitmapInterpolationModeCubic
                            );
                    if (SUCCEEDED(hr))
                    {
                        hr = pConverter->Initialize(
                            pScaler,
                            GUID_WICPixelFormat32bppPBGRA,
                            WICBitmapDitherTypeNone,
                            NULL,
                            0.f,
                            WICBitmapPaletteTypeMedianCut
                            );
                    }
                }
            }
        }
        else
        {
                    
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
        }
    }
    if (SUCCEEDED(hr))
    {
        //create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

wstring s2ws(const string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
	wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}


string ws2s(const wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0); 
	string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0); 
	return r;
}


