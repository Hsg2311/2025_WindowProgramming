#include <windows.h>
#include <tchar.h>
#include <vector>
#include <numeric>
#include <random>
#include <iostream>

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 2-2";
RECT resolution = { 0, 0, 800, 600 };

auto posX = std::vector<int>( );
auto posX2 = std::vector<int>( );
auto posY = std::vector<int>( );
auto posY2 = std::vector<int>( );
auto copyX = std::vector<int>( );
auto copyY = std::vector<int>( );

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

	posX.resize( 300 );		// 50 ~ 350
	posX2.resize( 300 );	// 450 ~ 750
	posY.resize( 200 );		// 50 ~ 250
	posY2.resize( 200 );	// 350 ~ 550

	copyX.resize( 10 );
	copyY.resize( 10 );

	std::iota( posX.begin( ), posX.end( ), 100 );
	std::iota( posX2.begin( ), posX2.end( ), 450 );
	std::iota( posY.begin( ), posY.end( ), 50 );
	std::iota( posY2.begin( ), posY2.end( ), 350 );

	std::shuffle( posX.begin( ), posX.end( ), std::mt19937{ std::random_device{}( ) } );
	std::shuffle( posX2.begin( ), posX2.end( ), std::mt19937{ std::random_device{}( ) } );
	std::shuffle( posY.begin( ), posY.end( ), std::mt19937{ std::random_device{}( ) } );
	std::shuffle( posY2.begin( ), posY2.end( ), std::mt19937{ std::random_device{}( ) } );

	posX.insert( posX.end( ), posX2.begin( ), posX2.end( ) );
	posY.insert( posY.end( ), posY2.begin( ), posY2.end( ) );

	std::shuffle( posX.begin( ), posX.end( ), std::mt19937{ std::random_device{}( ) } );
	std::shuffle( posY.begin( ), posY.end( ), std::mt19937{ std::random_device{}( ) } );

	std::copy( posX.begin( ), posX.begin( ) + 10, copyX.begin( ) );
	std::copy( posY.begin( ), posY.begin( ) + 10, copyY.begin( ) );

	//std::cout << copyX.size( ) << std::endl;
	//std::cout << copyY.size( ) << std::endl;

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

		RECT rc{ 360, 260, 440, 340 };
		DrawText( hDC, L"0: (400, 300)", -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

		if ( copyX.size( ) != 0 ) {
			for ( auto i = 0; i < 10; ++i ) {
				TCHAR str[ 30 ];
				RECT rcTmp{ copyX[ i ] - 40, copyY[ i ] - 40, copyX[ i ] + 40, copyY[ i ] + 40 };
				wsprintf( str, L"%d: (%d, %d)", i + 1, copyX[ i ], copyY[ i ] );
				DrawText( hDC, str, -1, &rcTmp, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
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