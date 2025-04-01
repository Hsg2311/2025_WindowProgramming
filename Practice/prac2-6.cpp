#include <windows.h>
#include <tchar.h>
#include <vector>
#include <vector>
#include <random>
#include <iostream>
#include <string>

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 2-6";
RECT resolution = { 0, 0, 800, 600 };

std::random_device rd;
std::mt19937_64 gen( rd( ) );
std::uniform_int_distribution coordX( 0, 600 );
std::uniform_int_distribution coordY( 0, 400 );
std::uniform_int_distribution randNum( 0, 200 );
std::uniform_int_distribution randCount( 5, 20 );
std::uniform_int_distribution randColor( 0, 255 );

int x = coordX( gen );
int y = coordY( gen );
int num = randNum( gen );
int count = randCount( gen );
COLORREF bgColor = RGB( randColor( gen ), randColor( gen ), randColor( gen ) );
COLORREF fColor = RGB( randColor( gen ), randColor( gen ), randColor( gen ) );

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

	switch ( iMessage ) {
	case WM_PAINT: {
		hDC = BeginPaint( hWnd, &ps );

		auto tmp = std::wstring( L"Num: " + std::to_wstring( num ) + L", Count: " + std::to_wstring( count ) );
		TextOut( hDC, 10, 10, tmp.c_str( ), static_cast<int>( tmp.size( ) ) );

		SetBkColor( hDC, bgColor );
		SetTextColor( hDC, fColor );
		
		auto vecs = std::vector< std::vector<int> >( count, std::vector<int>( count, num ) );
		
		auto str = std::wstring( );
		for ( const auto& vec : vecs ) {
			for ( const auto& i : vec ) {
				str += std::to_wstring( i );
			}
			str += L"\n";
		}

		auto xPos = x;
		auto yPos = y;
		auto x2Pos = x + count * 25;
		auto y2Pos = y + count * 25;

		RECT rc = { xPos, yPos, x2Pos, y2Pos };
		//Rectangle( hDC, xPos - 5, yPos - 5, xPos + 5, yPos + 5 );
		//Rectangle( hDC, x2Pos - 5, y2Pos - 5, x2Pos + 5, y2Pos + 5 );
		DrawText( hDC, str.c_str( ), -1, &rc, DT_CENTER | DT_VCENTER );

		EndPaint( hWnd, &ps );
		break;
	}

	case WM_CHAR:
		if ( wParam == VK_RETURN ) {
			x = coordX( gen );
			y = coordY( gen );
			num = randNum( gen );
			count = randCount( gen );
			bgColor = RGB( randColor( gen ), randColor( gen ), randColor( gen ) );
			fColor = RGB( randColor( gen ), randColor( gen ), randColor( gen ) );
			InvalidateRect( hWnd, nullptr, true );
		}
		else if ( wParam == 'q' ) {
			PostQuitMessage( 0 );
		}
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}