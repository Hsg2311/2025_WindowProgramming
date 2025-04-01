#include <windows.h>
#include <tchar.h>
#include <vector>
#include <numeric>
#include <random>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 2-5";
RECT resolution = { 0, 0, 800, 600 };

std::random_device rd;
std::mt19937_64 gen( rd( ) );
std::uniform_int_distribution ranColor( 0, 255 );

int x, y;

class String {
public:
	String( ) = default;
	String( const TCHAR* str, std::int32_t coordX, std::int32_t coordY, COLORREF RGB, bool reflect ) 
		: str_( str ), coordX_( coordX ), coordY_( coordY ), RGB_( RGB ), reflect_( reflect ) { }

	const std::wstring& getStr( ) const { return str_; }
	std::int32_t getCoordX( ) const { return coordX_; }
	std::int32_t getCoordY( ) const { return coordY_; }
	COLORREF getRGB( ) const { return RGB_; }
	bool getReflect( ) const { return reflect_; }

private:
	std::wstring str_;
	std::int32_t coordX_;
	std::int32_t coordY_;
	COLORREF RGB_;
	bool reflect_;
};

LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow )
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof( WndClass );
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx( &WndClass );
	hWnd = CreateWindow( lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
		100, 100, resolution.right, resolution.bottom, NULL, (HMENU)NULL, hInstance, NULL );

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	AdjustWindowRect( &resolution, WS_OVERLAPPEDWINDOW, FALSE );
	SetWindowPos( hWnd, nullptr, 100, 100, resolution.right - resolution.left, resolution.bottom - resolution.top, 0 );

	x = 400;
	y = 300;

	while ( GetMessage( &Message, 0, 0, 0 ) ) {
		TranslateMessage( &Message );
		DispatchMessage( &Message );
	}

	return static_cast<int>( Message.wParam );
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam ) {
	PAINTSTRUCT ps;
	HDC hDC;

	static TCHAR str[ 100 ]{ };
	static int cnt{ };
	static std::vector<String> strings;
	static COLORREF RGB = RGB( ranColor( gen ), ranColor( gen ), ranColor( gen ) );
	static bool reflect = false;

	switch ( iMessage ) {
	case WM_PAINT: {
		hDC = BeginPaint( hWnd, &ps );

		SetTextColor( hDC, RGB );
		TextOut( hDC, x, y, str, lstrlen( str ) );
		if ( reflect ) {
			auto tempY = 300 - y;
			TextOut( hDC, x, 300 + tempY, str, lstrlen( str ) );
		}
		
		for ( const auto& s : strings ) {
			SetTextColor( hDC, s.getRGB( ) );
			TextOut( hDC, s.getCoordX( ), s.getCoordY( ), s.getStr( ).c_str( ), lstrlen( s.getStr( ).c_str( ) ) );
			if ( s.getReflect( ) ) {
				auto tempY = 300 - s.getCoordY( );
				TextOut( hDC, s.getCoordX( ), 300 + tempY, s.getStr( ).c_str( ), lstrlen( s.getStr( ).c_str( ) ) );
			}
		}

		EndPaint( hWnd, &ps );
		break;
	}

	case WM_CHAR:
		hDC = GetDC( hWnd );

		if ( wParam == VK_ESCAPE ) {
			PostQuitMessage( 0 );
			break;
		}

		if ( wParam == VK_BACK ) {
			if ( cnt > 0 ) {
				--cnt;
			}
		}
		else if ( wParam == VK_RETURN ) {
			strings.push_back( String{ str, x, y, RGB, reflect } );
			RGB = RGB( ranColor( gen ), ranColor( gen ), ranColor( gen ) );
			reflect = true;

			x -= 20;
			y -= 15;

			memset( str, 0, sizeof( str ) );
			cnt = 0;
		}
		else {
			if ( cnt > 98 ) {
				break;
			}
			str[ cnt++ ] = static_cast<TCHAR>( wParam );
		}
		str[ cnt ] = '\0';

		InvalidateRect( hWnd, nullptr, true );

		ReleaseDC( hWnd, hDC );
		break;

	case WM_KEYDOWN:
		if ( wParam == 112 ) {
			strings.clear( );
			x = 400;
			y = 300;
			cnt = 0;
			InvalidateRect( hWnd, nullptr, true );
		}
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}