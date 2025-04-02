#include <Windows.h>
#include <tchar.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";
LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

auto winRect = RECT{
	.left = 0,
	.top = 0,
	.right = 800,
	.bottom = 600
};

auto centerRectPos = POINT{
	.x = 400,
	.y = 300
};

auto centerRectScale = POINT{
	.x = 300,
	.y = 300
};

auto color = RGB( 0, 0, 0 );
char cmd = ' ';

auto rowLine = std::array<POINT, 2>{
	POINT{ centerRectPos.x - 10, centerRectPos.y },
	POINT{ centerRectPos.x + 10, centerRectPos.y }
};

auto colLine = std::array<POINT, 2>{
	POINT{ centerRectPos.x, centerRectPos.y - 10 },
	POINT{ centerRectPos.x, centerRectPos.y + 10 }
};

auto triangle = std::array<POINT, 3>{
	POINT{ centerRectPos.x, centerRectPos.y - 10 },
	POINT{ centerRectPos.x - 10, centerRectPos.y + 10 },
	POINT{ centerRectPos.x + 10, centerRectPos.y + 10 }
};

auto rectangle = POINT{
	.x = centerRectPos.x,
	.y = centerRectPos.y
};

auto triRowPosOffset = centerRectScale.y / 2;
auto triColPosOffset = centerRectScale.x / 2;

auto diagramCount = 10;

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
		100, 100, winRect.right - winRect.left, winRect.bottom - winRect.left, NULL, (HMENU)NULL, hInstance, NULL );

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	AdjustWindowRect( &winRect, WS_OVERLAPPEDWINDOW, false );
	SetWindowPos( hWnd, nullptr, 100, 100, winRect.right - winRect.left, winRect.bottom - winRect.top, 0 );

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
	case WM_CREATE:
		break;

	case WM_PAINT: {
		hDC = BeginPaint( hWnd, &ps );

		Rectangle( hDC,
			centerRectPos.x - centerRectScale.x / 2,
			centerRectPos.y - centerRectScale.y / 2,
			centerRectPos.x + centerRectScale.x / 2,
			centerRectPos.y + centerRectScale.y / 2
		);

		auto newPen = CreatePen( PS_SOLID, 5, color );
		auto oldPen = (HPEN)SelectObject( hDC, newPen );
		auto newBrush = CreateSolidBrush( color );
		auto oldBrush = (HBRUSH)SelectObject( hDC, newBrush );
		if ( cmd == 'l' ) {
			auto step = centerRectScale.x / diagramCount;
			for ( auto i = 0; i < diagramCount; ++i ) {
				// top -------------------------------------------------
				MoveToEx( hDC,
					rowLine[ 0 ].x - triColPosOffset + i * step + 15,
					rowLine[ 0 ].y - triRowPosOffset,
					nullptr
				);
				LineTo( hDC,
					rowLine[ 1 ].x - triColPosOffset + i * step + 15,
					rowLine[ 1 ].y - triRowPosOffset
				);
				// -----------------------------------------------------

				// left -------------------------------------------------
				MoveToEx( hDC,
					colLine[ 0 ].x - triColPosOffset,
					colLine[ 0 ].y - triRowPosOffset + i * step + 15,
					nullptr
				);
				LineTo( hDC,
					colLine[ 1 ].x - triColPosOffset,
					colLine[ 1 ].y - triRowPosOffset + i * step + 15
				);
				// -----------------------------------------------------

				// bottom -------------------------------------------------
				MoveToEx( hDC,
					rowLine[ 0 ].x - triColPosOffset + i * step + 15,
					rowLine[ 0 ].y + triRowPosOffset,
					nullptr
				);
				LineTo( hDC,
					rowLine[ 1 ].x - triColPosOffset + i * step + 15,
					rowLine[ 1 ].y + triRowPosOffset
				);
				// -----------------------------------------------------

				// right -------------------------------------------------
				MoveToEx( hDC,
					colLine[ 0 ].x + triColPosOffset,
					colLine[ 0 ].y - triRowPosOffset + i * step + 15,
					nullptr
				);
				LineTo( hDC,
					colLine[ 1 ].x + triColPosOffset,
					colLine[ 1 ].y - triRowPosOffset + i * step + 15
				);
				// -----------------------------------------------------
			}
		}
		else if ( cmd == 't' ) {
			auto step = centerRectScale.x / diagramCount;
			for ( auto i = 0; i < diagramCount; ++i ) {
				// top -------------------------------------------------
				POINT tempTriangle1[ ] = {
					POINT{ triangle[ 0 ].x - triColPosOffset + i * step + 15, triangle[ 0 ].y - triRowPosOffset },
					POINT{ triangle[ 1 ].x - triColPosOffset + i * step + 15, triangle[ 1 ].y - triRowPosOffset },
					POINT{ triangle[ 2 ].x - triColPosOffset + i * step + 15, triangle[ 2 ].y - triRowPosOffset }
				};

				Polygon( hDC, tempTriangle1, 3 );
				// -----------------------------------------------------

				// left -------------------------------------------------
				POINT tempTriangle2[ ] = {
					POINT{ triangle[ 0 ].x - triColPosOffset , triangle[ 0 ].y - triRowPosOffset + i * step + 15 },
					POINT{ triangle[ 1 ].x - triColPosOffset , triangle[ 1 ].y - triRowPosOffset + i * step + 15 },
					POINT{ triangle[ 2 ].x - triColPosOffset , triangle[ 2 ].y - triRowPosOffset + i * step + 15 }
				};

				Polygon( hDC, tempTriangle2, 3 );
				// -----------------------------------------------------

				// bottom -------------------------------------------------
				POINT tempTriangle3[ ] = {
					POINT{ triangle[ 0 ].x - triColPosOffset + i * step + 15, triangle[ 0 ].y + triRowPosOffset },
					POINT{ triangle[ 1 ].x - triColPosOffset + i * step + 15, triangle[ 1 ].y + triRowPosOffset },
					POINT{ triangle[ 2 ].x - triColPosOffset + i * step + 15, triangle[ 2 ].y + triRowPosOffset }
				};
				Polygon( hDC, tempTriangle3, 3 );
				// -----------------------------------------------------

				// right -------------------------------------------------
				POINT tempTriangle4[ ] = {
					POINT{ triangle[ 0 ].x + triColPosOffset, triangle[ 0 ].y - triRowPosOffset + i * step + 15 },
					POINT{ triangle[ 1 ].x + triColPosOffset, triangle[ 1 ].y - triRowPosOffset + i * step + 15 },
					POINT{ triangle[ 2 ].x + triColPosOffset, triangle[ 2 ].y - triRowPosOffset + i * step + 15 }
				};
				Polygon( hDC, tempTriangle4, 3 );
				// -----------------------------------------------------
			}
		}
		else if ( cmd == 'r' ) {
			auto step = centerRectScale.x / diagramCount;
			for ( auto i = 0; i < diagramCount; ++i ) {
				// top -------------------------------------------------
				Rectangle( hDC,
					rectangle.x - triColPosOffset - 10 + i * step + 15,
					rectangle.y - triRowPosOffset - 10,
					rectangle.x - triColPosOffset + 10 + i * step + 15,
					rectangle.y - triRowPosOffset + 10
				);
				// -----------------------------------------------------

				// left -------------------------------------------------
				Rectangle( hDC,
					rectangle.x - triColPosOffset - 10,
					rectangle.y - triRowPosOffset - 10 + i * step + 15,
					rectangle.x - triColPosOffset + 10,
					rectangle.y - triRowPosOffset + 10 + i * step + 15
				);
				// -----------------------------------------------------

				// bottom -------------------------------------------------
				Rectangle( hDC,
					rectangle.x - triColPosOffset - 10 + i * step + 15,
					rectangle.y + triRowPosOffset - 10,
					rectangle.x - triColPosOffset + 10 + i * step + 15,
					rectangle.y + triRowPosOffset + 10
				);
				// -----------------------------------------------------

				// right -------------------------------------------------
				Rectangle( hDC,
					rectangle.x + triColPosOffset - 10,
					rectangle.y - triRowPosOffset - 10 + i * step + 15,
					rectangle.x + triColPosOffset + 10,
					rectangle.y - triRowPosOffset + 10 + i * step + 15
				);
				// -----------------------------------------------------
			}
		}
		SelectObject( hDC, oldPen );
		SelectObject( hDC, oldBrush );
		DeleteObject( newPen );
		DeleteObject( newBrush );

		EndPaint( hWnd, &ps );
		break;
	}

	case WM_CHAR:
		if ( wParam == 'q' ) {
			PostQuitMessage( 0 );
		}
		else if ( wParam == 'l' ) {
			cmd = 'l';
			color = RGB( 0, 0, 255 );
		}
		else if ( wParam == 't' ) {
			cmd = 't';
			color = RGB( 255, 0, 0 );
		}
		else if ( wParam == 'r' ) {
			cmd = 'r';
			color = RGB( 0, 255, 0 );
		}
		InvalidateRect( hWnd, nullptr, true );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}

