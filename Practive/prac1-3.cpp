#include <windows.h>
#include <tchar.h>

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 2";

LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow )
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	WndClass.cbSize = sizeof( WndClass );
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;				// 加己 函版
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	WndClass.hCursor = LoadCursor( NULL, IDC_CROSS );					// 加己 函版
	WndClass.hbrBackground = (HBRUSH)GetStockObject( GRAY_BRUSH );		// 加己 函版
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = L"MyWinProject";							// 加己 函版
	WndClass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx( &WndClass );
	hWnd = CreateWindow( L"MyWinProject", lpszWindowName, WS_OVERLAPPEDWINDOW,
		100, 50, 800, 600, NULL, (HMENU)NULL, hInstance, NULL );

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

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
	case WM_PAINT:
		hDC = BeginPaint( hWnd, &ps );
		// TODO
		EndPaint( hWnd, &ps );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}