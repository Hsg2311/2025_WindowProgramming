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

auto rc = RECT{
	.left = 0,
	.top = 0,
	.right = 800,
	.bottom = 600
};

auto arr = std::array<std::array<TCHAR, 30>, 10>( );
auto arrCopy = std::array<std::array<TCHAR, 30>, 10>( );
auto tempStrs = std::vector<std::wstring>( 10 );
std::uint32_t caretPosX = 0;
std::uint32_t caretPosY = 0;
std::uint8_t caretHeight = 20;
std::uint8_t line = 0;
std::uint8_t count = 0;

SIZE size;

void checkCaret( );

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
		100, 100, rc.right - rc.left, rc.bottom - rc.left, NULL, (HMENU)NULL, hInstance, NULL );

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, false );
	SetWindowPos( hWnd, nullptr, 100, 100, rc.right - rc.left, rc.bottom - rc.top, 0 );

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
		CreateCaret( hWnd, nullptr, 2, caretHeight );
		ShowCaret( hWnd );

		for ( auto& a : arr ) {
			a.fill( L' ' );
		}

		for ( auto& a : arrCopy ) {
			a.fill( L' ' );
		}

		break;

	case WM_PAINT:
		hDC = BeginPaint( hWnd, &ps );

		SetCaretPos( caretPosX, caretPosY );

		for ( auto i = 0; i < arr.size( ); ++i ) {
			TextOut( hDC, 0, i * caretHeight, tempStrs[ i ].c_str( ), static_cast<int>( tempStrs[ i ].size( ) ) );
		}

		EndPaint( hWnd, &ps );
		break;

	case WM_CHAR: {
		auto hDC = GetDC( hWnd );

		if ( wParam == VK_RETURN ) {
			checkCaret( );
		}
		else if ( wParam == VK_BACK ) {
			if ( count > 0 ) {
				arr[ line ][ --count ] = L' ';
			}
			else {
				if ( line > 0 ) {
					--line;
					caretPosY -= caretHeight;

					if ( arr[ line ].front( ) == L' ' ) {
						ReleaseDC( hWnd, hDC );
						InvalidateRect( hWnd, nullptr, true );
						break;
					}
					else {
						for ( auto i = 0; i < arr[ line ].size( ); ++i ) {
							if ( i == arr[ line ].size( ) - 1 ) {
								count = i;
								break;
							}
							else if ( arr[ line ][ i ] == L' ' ) {
								count = i - 1;
								break;
							}
						}
						arr[ line ][ count ] = L' ';
					}
				}
			}
		}
		else {
			if ( count < arr[ line ].size( ) ) {
				arr[ line ][ count++ ] = static_cast<TCHAR>( wParam );
			}
			else {
				checkCaret( );
				arr[ line ][ count++ ] = static_cast<TCHAR>( wParam );
			}
		}

		for ( auto& a : arrCopy ) {
			a.fill( L' ' );
		}

		int tempLine = 0;
		for ( const auto& a : arr ) {
			int idx = 0;
			for ( auto c : a ) {
				if ( !iswspace( c ) ) {
					arrCopy[ tempLine ][ idx++ ] = c;
				}
			}
			arr[ tempLine ] = arrCopy[ tempLine ];
			++tempLine;
		}

		auto str = std::wstring( arrCopy[ line ].begin( ), arrCopy[ line ].begin( ) + count );		// 한 줄에 지금까지 입력된 문자들을 문자열로 변환
		GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );			// 문자열의 길이를 계산
		caretPosX = size.cx;																		// 지금까지 입력된 문자들이기 때문에 size.cx(문자열 끝 x좌표)가 caretPosX가 됨

		for ( auto i = 0; i < arrCopy.size( ); ++i ) {
			auto s = std::wstring( arrCopy[ i ].begin( ), arrCopy[ i ].end( ) );
			s.erase( std::remove_if( s.begin( ), s.end( ), iswspace ), s.end( ) );

			tempStrs[ i ] = s;
		}

		/*std::cout << "arr" << std::endl;
		for ( const auto& a : arr ) {
			for ( const auto& b : a ) {
				std::wcout << b;
			}
			std::wcout << std::endl;
		}

		std::cout << "arrCopy" << std::endl;
		for ( const auto& a : arrCopy ) {
			for ( const auto& b : a ) {
				std::wcout << b;
			}
			std::wcout << std::endl;
		}

		std::cout << "tempStrs" << std::endl;
		for ( const auto& a : tempStrs ) {
			std::wcout << a << std::endl;
		}*/

		ReleaseDC( hWnd, hDC );
		InvalidateRect( hWnd, nullptr, true );
	}
				break;

	case WM_DESTROY:
		HideCaret( hWnd );
		DestroyCaret( );
		PostQuitMessage( 0 );
		return 0;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}


void checkCaret( ) {
	caretPosX = 0;
	caretPosY += caretHeight;
	++line;
	if ( caretPosY > ( arr.size( ) - 1 ) * caretHeight ) {
		caretPosY = 0;
		line = 0;
	}
	count = 0;
}

