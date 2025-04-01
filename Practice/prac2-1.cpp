#include <windows.h>
#include <tchar.h>

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 2-1";
RECT resolution = { 0, 0, 800, 600 };

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

		auto colDiv = resolution.right / 4;
		auto rowDiv = resolution.bottom / 2;

		for ( auto i = 0; i < 4; ++i ) {
			MoveToEx( hDC, colDiv * i, 0, NULL );
			LineTo( hDC, colDiv * i, resolution.bottom );
		}

		for ( auto i = 0; i < 2; ++i ) {
			MoveToEx( hDC, 0, rowDiv * i, NULL );
			LineTo( hDC, resolution.right, rowDiv * i );
		}

		POINT pt{ 10, 10 };
		for ( auto i = 2; i <= 9; ++i ) {
			TCHAR str[ 100 ];
			for ( auto j = 1; j <= 9; ++j ) {
				wsprintf( str, L"%d * %d = %d", i, j, i * j );
				TextOut( hDC, pt.x, pt.y + ( ( j - 1 ) * rowDiv / 9 ), str, lstrlen( str ) );
			}
			pt.x += colDiv;

			if ( i == 5 ) {
				pt.x = 10;
				pt.y += rowDiv;
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