#pragma once
/******************************************************************
*                                                                 *
* All.h - Include and declare all the cores.                      *
*                                                                 *
*******************************************************************
*                                                                 *
* Includes and defines                                            *
*                                                                 *
******************************************************************/

#ifndef WINVER                          // Allow use of features specific to Windows Vista or later.
#define WINVER _WIN32_WINNT_VISTA
#endif

#ifndef _WIN32_WINNT                    // Allow use of features specific to Windows Vista or later.
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#endif

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEFAULT_LAYER 127

#define WINDOW_TITLE L"PectoStudio login..."

// Windows Header Files:
#include <windows.h>
#include <windowsx.h> // Exstntion to Win.h
#include <shellapi.h> // Shell API
#include <d2d1.h> // Direct2D
#include <d2d1helper.h> // D2D Exstntion
#include <dwrite.h> // DirectWrite
#include <wincodec.h> // Media Codecs
#include <commctrl.h>
#include <assert.h>
#include <Winhttp.h>
#include <Winscard.h>
#include <cpprest\json.h>


#pragma comment(lib, "Winhttp.lib")
#pragma comment(lib, "Winscard.lib")


// C RunTime Header Files
#include <cstdlib>
#include <cwchar>
#include <cmath>
#include <cstdarg>
#include <ctime>
#include <malloc.h>
#include <memory.h>

// C++ STL
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <queue>
#include <exception>
#include <iostream>



#pragma warning( disable: 4702) // Ingnore useless warnings

using namespace std;

/******************************************************************
*                                                                 *
*  End Includes and defines                                       *
*                                                                 *
*******************************************************************
*                                                                 *
*  Macros                                                         *
*                                                                 *
******************************************************************/

template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#define STDMSGBOX (MB_RTLREADING | MB_RIGHT | MB_TOPMOST)

/******************************************************************
*                                                                 *
*  End Macros                                                     *
*                                                                 *
*******************************************************************
*                                                                 *
*  String convertion                                              *
*                                                                 *
*******************************************************************/

wstring s2ws(const string& s);
string ws2s(const wstring& s);

/******************************************************************
*                                                                 *
*  End String convertion                                          *
*                                                                 *
*******************************************************************
*                                                                 *
*  Exceptions                                                     *
*                                                                 *
*******************************************************************/

class Error
	:public std::exception
{
public:
	typedef enum ErrorType {
		GENERIC_ERROR,
		HTTP_REQ_ERROR,
		DW_ERROR_CREATE_TEXT_FORMAT,
		NFC_NO_CARD
	} ErrorType;
	Error(ErrorType et = GENERIC_ERROR, int code = -1);
	const char* what();
	string toString();
	const ErrorType GetErrorType();
	const int GetNumCode();
private:
	int code;
	ErrorType et;
};

/******************************************************************
*                                                                 *
*  End Exception                                                  *
*                                                                 *
*******************************************************************
*                                                                 *
*  End All.h                                                      *
*                                                                 *
******************************************************************/