#include <windows.h>
#include <tchar.h>
#include <vector>
#include <numeric>
#include <random>
#include <iostream>
#include <string>

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 2-3";
RECT resolution = { 0, 0, 800, 600 };

std::random_device rd;
std::mt19937_64 gen( rd( ) );
std::uniform_int_distribution<int> ranNum( 2, 10 );
std::uniform_int_distribution<int> letter( 0, 25 );
std::uniform_int_distribution<int> ranColor( 0, 255 );

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
	case WM_PAINT: {	// 얘는 무슨 윈도우 만들어지기 전에도 불리냐
		hDC = BeginPaint( hWnd, &ps );

		auto row = ranNum( gen );
		auto col = ranNum( gen );

		auto width = ( resolution.right - resolution.left ) / col;
		auto height = ( resolution.bottom - resolution.top ) / row;
		auto offsetX = width / 2;
		auto offsetY = height / 2;
		auto lastLetter = letter( gen );

		for ( auto i = 0; i < row; ++i ) {
			for ( auto j = 0; j < col; ++j ) {
		//		auto lastLetter = letter( gen );
				auto str = std::wstring( );
				auto center = lastLetter / 2;
				
				if ( lastLetter < 6 ) {
					for ( auto c = 0; c <= lastLetter; ++c ) {
						str.push_back( L'a' + c );
					}
				}
				else {
					for ( auto c = 0; c <= lastLetter; ++c ) {
						if ( c == center ) {
							str.push_back( L'\n' );
						}
						str.push_back( L'a' + c );
					}
				}
				
				RECT rc{ 
					j * width,
					i * height, 
					( j + 1 ) * width, 
					( i + 1 ) * height
				};

				SetBkColor( hDC, RGB( ranColor( gen ), ranColor( gen ), ranColor( gen ) ) );
				SetTextColor( hDC, RGB( ranColor( gen ), ranColor( gen ), ranColor( gen ) ) );

				DrawText( hDC, str.c_str( ), -1, &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK );
			}
		}

		EndPaint( hWnd, &ps );
		break;
	}
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}