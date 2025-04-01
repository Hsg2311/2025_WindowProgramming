#include <windows.h>
#include <tchar.h>
#include <vector>
#include <numeric>
#include <random>
#include <iostream>
#include <string>

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 2-4";
RECT resolution = { 0, 0, 800, 600 };

std::random_device rd;
std::mt19937_64 gen( rd( ) );
std::uniform_int_distribution coordX( 0, 600 );
std::uniform_int_distribution coordY( 0, 500 );
std::uniform_int_distribution ranColor( 0, 255 );

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

	static int posX = coordX( gen );
	static int posY = coordY( gen );
	static COLORREF color = RGB( ranColor( gen ), ranColor( gen ), ranColor( gen ) );

	switch ( iMessage ) {
	case WM_PAINT: {	// 얘는 무슨 윈도우 만들어지기 전에도 불리냐
		hDC = BeginPaint( hWnd, &ps );
		SetTextColor( hDC, color );

		TextOut( hDC, posX, posY, str, lstrlen( str ) );

		EndPaint( hWnd, &ps );
		break;
	}

	case WM_CHAR:
		hDC = GetDC( hWnd );

		if ( wParam == VK_ESCAPE ) {
			PostQuitMessage( 0 );
		}

		if ( wParam == VK_BACK ) {
			if ( cnt > 0 ) {
				--cnt;
			}
		}
		else if ( wParam == VK_RETURN ) {
			posY += 20;
		}
		else {
			if ( cnt >= 80 ) {
				break;
			}
			str[ cnt++ ] = wParam;
		}
		str[ cnt ] = '\0';

		InvalidateRect( hWnd, nullptr, true );

		ReleaseDC( hWnd, hDC );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}