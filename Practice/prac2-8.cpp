#include <Windows.h>
#include <tchar.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

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
auto tempStrs = std::vector<std::wstring>( 10 );
auto strLineCount = std::array<int, 10>( );
auto totalLineCount = std::array<int, 10>( );
std::int32_t caretPosX = 0;
std::int32_t caretPosY = 0;
std::uint16_t caretHeight = 20;
std::uint16_t line = 0;
std::uint16_t count = 0;
std::uint16_t caretLineCount = 0;

SIZE size;
bool isInsertPressed = false;

void checkCaret( );
void resetCaret( );
void totalLineCountCheck( );

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
	
	switch ( iMessage ) {
	case WM_CREATE:
		CreateCaret( hWnd, nullptr, 2, caretHeight );
		ShowCaret( hWnd );

		for ( auto& a : arr ) {
			a.fill( L' ' );
		}

		break;

	case WM_PAINT: {
		auto hDC = BeginPaint( hWnd, &ps );

		SetCaretPos( caretPosX, caretPosY );

		for ( auto i = 0; i < arr.size( ); ++i ) {
			TextOut( hDC, 0, i * caretHeight, arr[ i ].data( ), static_cast<int>( arr[ i ].size( ) ) );
		}

		EndPaint( hWnd, &ps );
		break;
	}

	case WM_CHAR: {
		auto hDC = GetDC( hWnd );

		if ( wParam == VK_RETURN ) {
			checkCaret( );
		}
		else if ( wParam == VK_ESCAPE ) {
			resetCaret( );
		}
		else if ( wParam == VK_TAB ) {
			if ( isInsertPressed ) {
				auto tmpStr = std::wstring( L"     " );
				auto copyCnt = count;

				for ( auto riter = arr[ line ].rbegin( ); ; ++riter ) {
					if ( riter == arr[ line ].rend( ) ) {
						break;
					}

					if ( *riter != L' ' ) {
						auto tmpCnt = std::distance( arr[ line ].begin( ), riter.base( ) );
						if ( tmpCnt <= copyCnt ) {
							break;
						}

						auto str = std::wstring( arr[ line ].begin( ) + copyCnt, arr[ line ].begin( ) + tmpCnt );
						tmpStr += str;
						break;
					}
				}

				if ( tmpStr.size( ) < arr[ line ].size( ) - copyCnt ) {
					for ( auto i = 0; i < tmpStr.size( ); ++i ) {
						arr[ line ][ copyCnt++ ] = tmpStr[ i ];
					}
					count += 5;
					totalLineCount[ line ] += 5;
				}
			}
			else {
				for ( auto i = 0; i < 5; ++i ) {
					if ( count < arr[ line ].size( ) ) {
							arr[ line ][ count++ ] = L' ';
						++totalLineCount[ line ];
					}
					else {
						checkCaret( );
						break;
					}
				}
			}
		}
		else if ( wParam == VK_BACK ) {
			if ( count > 0 ) {
				arr[ line ][ --count ] = L' ';
				auto tempMem = std::vector<TCHAR>( arr[ line ].begin( ) + count + 1, arr[ line ].end( ) );

				for ( auto iter = arr[ line ].begin( ) + count + 1; iter != arr[ line ].end( ); ++iter ) {
					*iter = L' ';
				}

				for ( auto i = count; i < arr[ line ].size( ) - 1; ++i ) {
					arr[ line ][ i ] = tempMem[ i - count ];
				}
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
						count = totalLineCount[ line ];
					}
				}
			}
		}
		else {
			if ( isInsertPressed ) {
				if ( totalLineCount[ line ] < arr[ line ].size( ) ) {
					std::uint16_t tmpCnt;
					for ( auto riter = arr[ line ].rbegin( ); ; ++riter ) {
						if ( riter == arr[ line ].rend( ) ) {
							break;
						}
						if ( *riter != L' ' ) {
							tmpCnt = std::distance( arr[ line ].begin( ), riter.base( ) );
							break;
						}
					}

					auto tmpStr = std::wstring( arr[ line ].begin( ) + count, arr[ line ].begin( ) + tmpCnt );
					arr[ line ][ count++ ] = static_cast<TCHAR>( wParam );
					auto tmpCnt2 = count;
					for ( auto i = 0; i < tmpStr.size( ); ++i ) {
						arr[ line ][ tmpCnt2++ ] = tmpStr[ i ];
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
		}

		strLineCount[ line ] = count;
		caretLineCount = count;

		totalLineCountCheck( );

		auto str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + count );		// 한 줄에 지금까지 입력된 문자들을 문자열로 변환
		GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );	// 문자열의 길이를 계산
		caretPosX = size.cx;																// 지금까지 입력된 문자들이기 때문에 size.cx(문자열 끝 x좌표)가 caretPosX가 됨

		for ( auto i = 0; i < arr.size( ); ++i ) {
			auto s = std::wstring( arr[ i ].begin( ), arr[ i ].begin( ) + strLineCount[ i ] );
			tempStrs[ i ] = s;
		}

		std::cout << "arr" << std::endl;
		for ( const auto& a : arr ) {
			for ( const auto& b : a ) {
				std::wcout << b;
			}
			std::wcout << std::endl;
		}

		std::cout << "tempStrs" << std::endl;
		for ( const auto& a : tempStrs ) {
			std::wcout << a << std::endl;
		}

		/*std::cout << "lineCount" << std::endl;
		for ( const auto& a : strLineCount ) {
			std::cout << a << std::endl;
		}*/

		std::cout << "totalLineCount" << std::endl;
		for ( const auto& a : totalLineCount ) {
			std::cout << a << std::endl;
		}

		ReleaseDC( hWnd, hDC );
		InvalidateRect( hWnd, nullptr, true );
		break;
	}

	case WM_KEYDOWN: {
		auto hDC = GetDC( hWnd );

		if ( wParam == VK_HOME ) {
			caretPosX = 0;
			count = 0;
			caretLineCount = 0;
		}
		else if ( wParam == VK_END ) {
			for ( auto riter = arr[ line ].rbegin( ); ; ++riter ) {
				if ( riter == arr[ line ].rend( ) ) {
					break;
				}

				if ( *riter != L' ' ) {
					count = std::distance( arr[ line ].begin( ), riter.base( ) );
					caretLineCount = count;
					auto str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + count );
					GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
					caretPosX = size.cx;
					break;
				}
			}
		}
		else if ( wParam == VK_INSERT ) {
			isInsertPressed = !isInsertPressed;
		}
		else if ( wParam == VK_DELETE ) {
			if ( arr[ line ].front( ) == L' ' ) {
				if ( line > 0 ) {
					--line;
					caretPosY -= caretHeight;
					count = totalLineCount[ line ];
					caretLineCount = count;

					auto str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + totalLineCount[ line ] );
					GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
					caretPosX = size.cx;
				}
				ReleaseDC( hWnd, hDC );
				InvalidateRect( hWnd, nullptr, true );
				break;
			}

			if ( count != arr[ line ].size( ) ) {
				arr[ line ][ count ] = L' ';
				auto tempStr = std::wstring( arr[ line ].begin( ) + count + 1, arr[ line ].end( ) );

				for ( auto iter = arr[ line ].begin( ) + count + 1; iter != arr[ line ].end( ); ++iter ) {
					*iter = L' ';
				}
				
				auto tmpCnt = count;
				for ( auto i = 0; i < tempStr.size( ); ++i ) {
					arr[ line ][ tmpCnt++ ] = tempStr[ i ];
				}

				for ( auto riter = arr[ line ].rbegin( ); ; ++riter ) {
					if ( riter == arr[ line ].rend( ) ) {
						strLineCount[ line ] = 0;
						break;
					}
					if ( *riter != L' ' ) {
						std::uint16_t tmpCnt = std::distance( arr[ line ].begin( ), riter.base( ) );
						strLineCount[ line ] = tmpCnt;
						break;
					}
				}
				for ( auto i = 0; i < arr.size( ); ++i ) {
					auto s = std::wstring( arr[ i ].begin( ), arr[ i ].begin( ) + strLineCount[ i ] );
					tempStrs[ i ] = s;
				}

				totalLineCountCheck( );

				std::cout << "arr" << std::endl;
				for ( const auto& a : arr ) {
					for ( const auto& b : a ) {
						std::wcout << b;
					}
					std::wcout << std::endl;
				}

				std::cout << "tempStrs" << std::endl;
				for ( const auto& a : tempStrs ) {
					std::wcout << a << std::endl;
				}

				std::cout << "totalCount" << std::endl;
				for ( const auto& a : totalLineCount ) {
					std::cout << a << std::endl;
				}
			}
		}
		else if ( wParam == VK_UP ) {
			caretPosY -= caretHeight;
			if ( line > 0 ) {
				--line;
			}
			if ( caretPosY < 0 ) {
				caretPosY = ( arr.size( ) - 1 ) * caretHeight;
				line = arr.size( ) - 1;
			}

			auto str = std::wstring( );
			if ( totalLineCount[ line ] >= caretLineCount ) {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + caretLineCount );
			}
			else {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + totalLineCount[ line ] );
				caretLineCount = totalLineCount[ line ];
			}
			count = caretLineCount;
			GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
			caretPosX = size.cx;
		}
		else if ( wParam == VK_DOWN ) {
			caretPosY += caretHeight;
			++line;
			if ( caretPosY > ( arr.size( ) - 1 ) * caretHeight ) {
				caretPosY = 0;
				line = 0;
			}

			auto str = std::wstring( );
			if ( totalLineCount[ line ] >= caretLineCount ) {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + caretLineCount );
			}
			else {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + totalLineCount[ line ] );
				caretLineCount = totalLineCount[ line ];
			}
			count = caretLineCount;
			GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
			caretPosX = size.cx;
		}
		else if ( wParam == VK_LEFT ) {
			if ( ( caretLineCount - 1 ) < 0 ) {
				ReleaseDC( hWnd, hDC );
				InvalidateRect( hWnd, nullptr, true );
				break;
			}
			--caretLineCount;

			auto str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + caretLineCount );
			GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
			caretPosX = size.cx;
			
			count = caretLineCount;
		}
		else if ( wParam == VK_RIGHT ) {
			if ( ( caretLineCount + 1 ) > totalLineCount[ line ] ) {
				ReleaseDC( hWnd, hDC );
				InvalidateRect( hWnd, nullptr, true );
				break;
			}

			++caretLineCount;

			auto str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + caretLineCount );
			GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
			caretPosX = size.cx;

			count = caretLineCount;
		}
		else if ( wParam == VK_PRIOR ) {
			caretPosY -= ( caretHeight * 2 );
			if ( caretPosY < 0 ) {
				auto remainLineCnt = abs( caretPosY ) / caretHeight;
				caretPosY = ( arr.size( ) - 1 ) * caretHeight - ( remainLineCnt * caretHeight ) + caretHeight;
				line = ( arr.size( ) - 1 ) - remainLineCnt + 1;
			}
			else {
				line -= 2;
			}

			auto str = std::wstring( );
			if ( totalLineCount[ line ] >= caretLineCount ) {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + caretLineCount );
			}
			else {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + totalLineCount[ line ] );
				caretLineCount = totalLineCount[ line ];
			}
			count = caretLineCount;
			GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
			caretPosX = size.cx;
		}
		else if ( wParam == VK_NEXT ) {
			caretPosY += ( caretHeight * 2 );
			if ( caretPosY > ( arr.size( ) - 1 ) * caretHeight ) {
				auto remainedLineCnt = ( caretPosY - ( ( arr.size( ) - 1 ) * caretHeight ) ) / caretHeight;
				caretPosY = ( remainedLineCnt * caretHeight ) - caretHeight;
				line = remainedLineCnt - 1;
				std::cout << "line : " << line << std::endl;
			}
			else {
				line += 2;
				std::cout << "line : " << line << std::endl;
			}

			auto str = std::wstring( );
			if ( totalLineCount[ line ] >= caretLineCount ) {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + caretLineCount );
			}
			else {
				str = std::wstring( arr[ line ].begin( ), arr[ line ].begin( ) + totalLineCount[ line ] );
				caretLineCount = totalLineCount[ line ];
			}
			count = caretLineCount;
			GetTextExtentPoint( hDC, str.c_str( ), static_cast<int>( str.size( ) ), &size );
			caretPosX = size.cx;
		}

		ReleaseDC( hWnd, hDC );
		InvalidateRect( hWnd, nullptr, true );
		break;
	}

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

void resetCaret( ) {
	caretPosX = 0;
	caretPosY = 0;
	line = 0;
	count = 0;

	for ( auto& a : arr ) {
		a.fill( L' ' );
	}

	for ( auto& str : tempStrs ) {
		str.clear( );
	}

	strLineCount.fill( 0 );
	totalLineCount.fill( 0 );
}

void totalLineCountCheck( ) {
	auto tmpLine = 0;
	for ( const auto& a : arr ) {
		for ( auto riter = a.rbegin( ); ; ++riter ) {
			if ( riter == a.rend( ) ) {
				break;
			}
			if ( *riter != L' ' ) {
				auto tmpCnt = std::distance( a.begin( ), riter.base( ) );
				totalLineCount[ tmpLine++ ] = tmpCnt;
				break;
			}
		}
	}
}
