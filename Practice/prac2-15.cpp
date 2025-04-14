#define NOMINMAX

#pragma warning(disable : 4146)

#include <Windows.h>
#include <system_error>
#include <ranges>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <random>

std::mt19937_64 gen{ std::random_device{ }( ) };
std::uniform_int_distribution randColor{ 0, 255 };

using i32t = std::int32_t;
using u32t = std::uint32_t;

inline constexpr u32t boardSize = 40u;
inline constexpr u32t maxShapeCnt = 10u;

inline constexpr i32t clWidth = 1200u;
inline constexpr i32t clHeight = 600u;

inline constexpr u32t slotWidth = clWidth / boardSize;
inline constexpr u32t slotHeight = clHeight / boardSize;

template <class Enum>
constexpr auto etoi( Enum e ) noexcept {
	return static_cast<std::underlying_type_t<Enum>>( e );
}

enum class ShapeType {
	None,
	Line,
	Triangle,
	Rectangle,
	Pentagon,
	Hexagon,
};

COLORREF gColors[ ] = { RGB( 0, 162, 232 ), RGB( 126, 132, 247 ), RGB( 161, 250, 79 ), RGB( 240, 135, 132 ) };
HBRUSH gBrushes[ maxShapeCnt ];
HPEN gPens[ maxShapeCnt ];
COLORREF gSetColor;
HBRUSH gSetBrush;
HPEN gSetPen;
ShapeType gSetType = ShapeType::None;
HBRUSH gWhiteBrush;
HPEN gBlackPen;
HPEN gYellowPen;

struct Pos {
	u32t x, y;
};

struct Shape {
	COLORREF color;
	ShapeType type;
	u32t scale;
	Pos pos;
};

Shape gShapes[ maxShapeCnt ];

u32t gShapeCnt = 0u;
u32t gChosenShapeIdx = -1u;

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void processCmd( HWND hWnd, WPARAM cmd );
void Triangle( HDC memDC, int left, int top, int right, int bottom );
void Pentagon( HDC memDC, int left, int top, int right, int bottom );
void drawShape( HDC memDC, u32t i );

//--------------------------------------------------------------------------------------
auto inputRect = RECT{
	.left = 410,
	.top = 0,
	.right = 690,
	.bottom = 30
};
auto inputBuffer = std::array<TCHAR, 256>( );
i32t count;

class Diagram;
auto diagrams = std::vector<Diagram>( );
i32t selectDiag;
char c;

class Diagram {
public:
	Diagram( ) = default;
	Diagram( ShapeType shape, RECT drawScope, i32t lineWidth )
		: shape_( shape ), drawScope_( drawScope ), lineWidth_( lineWidth ), info_( ) {}

	ShapeType getShape( ) const { return shape_; }
	RECT getDrawScope( ) const { return drawScope_; }
	i32t getLineWidth( ) const { return lineWidth_; }
	const std::wstring& getInfo( ) const { return info_; }
	void makeInfo( ) {
		info_ += L"Shape: " + std::to_wstring( etoi( shape_ ) ) + L", Left: " + std::to_wstring( drawScope_.left ) +
			L", Top: " + std::to_wstring( drawScope_.top ) + L", Right: " + std::to_wstring( drawScope_.right ) +
			L", Bottom: " + std::to_wstring( drawScope_.bottom ) + L", LineWidth: " + std::to_wstring( lineWidth_ );
	}

	void setShape( ShapeType shape ) { shape_ = shape; }
	void setDrawScope( RECT drawScope ) { drawScope_ = drawScope; }
	void setLineWidth( i32t lineWidth ) { lineWidth_ = lineWidth; }
	void setLineColor( COLORREF color ) { randLineColor_ = color; }
	void setFillColor( COLORREF color ) { randFillColor_ = color; }

	void draw( HDC hdc ) const {
		auto newPen = CreatePen( PS_SOLID, lineWidth_, randLineColor_ );
		auto newBrush = CreateSolidBrush( randFillColor_ );
		auto oldPen = (HPEN)SelectObject( hdc, newPen );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );

		switch ( shape_ ) {
		case ShapeType::Line:
			MoveToEx( hdc, drawScope_.left, drawScope_.top, nullptr );
			LineTo( hdc, drawScope_.right, drawScope_.bottom );
			break;

		case ShapeType::Triangle:
			Triangle( hdc, drawScope_.left, drawScope_.top, drawScope_.right, drawScope_.bottom );
			break;

		case ShapeType::Rectangle:
			Rectangle( hdc, drawScope_.left, drawScope_.top, drawScope_.right, drawScope_.bottom );
			break;

		case ShapeType::Pentagon:
			Pentagon( hdc, drawScope_.left, drawScope_.top, drawScope_.right, drawScope_.bottom );
			break;

		case ShapeType::Hexagon:
			
			break;
		}

		SelectObject( hdc, oldPen );
		SelectObject( hdc, oldBrush );
		DeleteObject( newPen );
		DeleteObject( newBrush );
	}

private:
	ShapeType shape_;
	ShapeType originShape_;
	RECT drawScope_;
	i32t lineWidth_;
	std::wstring info_;

	COLORREF randLineColor_;
	COLORREF randFillColor_;
};

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) {
	auto wndCls = WNDCLASSEXW{
		.cbSize = sizeof( WNDCLASSEXW ),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIconW( nullptr, IDI_APPLICATION ),
		.hCursor = LoadCursorW( nullptr, IDC_ARROW ),
		.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH ),
		.lpszMenuName = nullptr,
		.lpszClassName = L"WindowClass",
		.hIconSm = LoadIconW( nullptr, IDI_APPLICATION )
	};

	if ( !RegisterClassExW( &wndCls ) ) {
		OutputDebugStringA( std::system_category( ).message( GetLastError( ) ).c_str( ) );
		return -1;
	}

	auto wndRect = RECT{ 0, 0, clWidth, clHeight };
	AdjustWindowRect( &wndRect, WS_OVERLAPPEDWINDOW, false );

	auto hWnd = CreateWindowW(
		L"WindowClass",
		L"prac 2-15",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wndRect.right - wndRect.left,
		wndRect.bottom - wndRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if ( !hWnd ) {
		OutputDebugStringA( std::system_category( ).message( GetLastError( ) ).c_str( ) );
		return -1;
	}

	ShowWindow( hWnd, nShowCmd );

	gWhiteBrush = (HBRUSH)GetStockObject( WHITE_BRUSH );
	gBlackPen = (HPEN)GetStockObject( BLACK_PEN );
	gYellowPen = CreatePen( PS_SOLID, 5, RGB( 200, 200, 12 ) );

	MSG msg;

	while ( GetMessageW( &msg, nullptr, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	}

	DeleteObject( gYellowPen );

	return static_cast<int>( msg.wParam );
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	PAINTSTRUCT ps;
	HDC hDC, memDC;
	HBITMAP memBitmap;

	switch ( msg ) {
	case WM_CLOSE:
		DestroyWindow( hWnd );
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;

	case WM_CHAR:
		if ( wParam == 'q' || wParam == 'Q' ) {
			PostQuitMessage( 0 );
			return 0;
		}
		else if ( wParam == '1' ) {
			if ( !diagrams.empty( ) ) {
				diagrams[ selectDiag ].setLineColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
			}
			c = '1';
		}
		else if ( wParam == '2' ) {
			if ( !diagrams.empty( ) ) {
				diagrams[ selectDiag ].setFillColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
			}
			c = '2';
		}
		else if ( wParam == '+' ) {
			if ( diagrams.empty( ) ) {
				break;
			}

			auto lineWidth = diagrams[ selectDiag ].getLineWidth( ) + 1;
			if ( lineWidth > 10 ) {
				auto tmpRc = diagrams[ selectDiag ].getDrawScope( );
				tmpRc.left = tmpRc.left - 5;
				tmpRc.top = tmpRc.top - 5;
				tmpRc.right = tmpRc.right + 5;
				tmpRc.bottom = tmpRc.bottom + 5;

				diagrams[ selectDiag ].setDrawScope( tmpRc );
			}
			else {
				diagrams[ selectDiag ].setLineWidth( lineWidth );
			}
		}
		else if ( wParam == '-' ) {
			if ( diagrams.empty( ) ) {
				break;
			}

			auto lineWidth = diagrams[ selectDiag ].getLineWidth( ) - 1;
			if ( lineWidth < 1 ) {
				auto tmpRc = diagrams[ selectDiag ].getDrawScope( );
				tmpRc.left = tmpRc.left + 5;
				tmpRc.top = tmpRc.top + 5;
				tmpRc.right = tmpRc.right - 5;
				tmpRc.bottom = tmpRc.bottom - 5;

				diagrams[ selectDiag ].setDrawScope( tmpRc );
			}
			else {
				diagrams[ selectDiag ].setLineWidth( lineWidth );
			}
		}
		
		if ( wParam == VK_BACK ) {
			if ( count > 0 ) {
				inputBuffer[ --count ] = '\0';
			}
		}
		else {
			if ( count < 256 ) {
				inputBuffer[ count++ ] = static_cast<TCHAR>( wParam );
			}
		}

		InvalidateRect( hWnd, nullptr, true );
		break;

	case WM_KEYDOWN: 
		if ( wParam == 'P' ) {
			if ( selectDiag != diagrams.size( ) - 1 ) {
				++selectDiag;
			}
			c = 'p';
		}
		else if ( wParam == 'N' ) {
			if ( selectDiag != 0 ) {
				--selectDiag;
			}
			c = 'n';
		}
		else if ( wParam == 'A' ) {
			c = 'a';
		}
		else if ( wParam == 'D' ) {
			c = 'd';

			diagrams.clear( );
		}
		else if ( wParam == '-' ) {

		}
		else if ( wParam == VK_RETURN ) {
			int type{ };
			RECT rc{ };
			int lineWidth{ };

			auto nums = std::vector<int>( );
			auto str = std::wstring( );

			auto diag = Diagram( );

			for ( auto ch : inputBuffer ) {
				if ( ch != L' ' && ch != 0 ) {
					str += ch;
				}
				else if ( ch == L' ' || ch == 0 ) {
					nums.push_back( std::stoi( str ) );
					str.clear( );

					if ( ch == 0 ) {
						break;
					}
				}
			}
			diag.setShape( static_cast<ShapeType>( nums[ 0 ] ) );
			auto diagRect = RECT{
				.left = nums[ 1 ],
				.top = nums[ 2 ],
				.right = nums[ 3 ],
				.bottom = nums[ 4 ]
			};
			diag.setDrawScope( diagRect );
			diag.setLineWidth( nums[ 5 ] );
			diag.makeInfo( );
			diag.setLineColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
			diag.setFillColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );

			diagrams.push_back( diag );
		}
		else if ( wParam == VK_LEFT ) {
			if ( diagrams.empty( ) ) {
				break;
			}

			auto left = diagrams[ selectDiag ].getDrawScope( ).left - 10;
			auto right = diagrams[ selectDiag ].getDrawScope( ).right - 10;
			diagrams[ selectDiag ].setDrawScope( RECT{
				.left = left,
				.top = diagrams[ selectDiag ].getDrawScope( ).top,
				.right = right,
				.bottom = diagrams[ selectDiag ].getDrawScope( ).bottom
			} );
		}
		else if ( wParam == VK_RIGHT ) {
			if ( diagrams.empty( ) ) {
				break;
			}

			auto left = diagrams[ selectDiag ].getDrawScope( ).left + 10;
			auto right = diagrams[ selectDiag ].getDrawScope( ).right + 10;
			diagrams[ selectDiag ].setDrawScope( RECT{
				.left = left,
				.top = diagrams[ selectDiag ].getDrawScope( ).top,
				.right = right,
				.bottom = diagrams[ selectDiag ].getDrawScope( ).bottom
			} );
		}
		else if ( wParam == VK_UP ) {
			if ( diagrams.empty( ) ) {
				break;
			}

			auto top = diagrams[ selectDiag ].getDrawScope( ).top - 10;
			auto bottom = diagrams[ selectDiag ].getDrawScope( ).bottom - 10;
			diagrams[ selectDiag ].setDrawScope( RECT{
				.left = diagrams[ selectDiag ].getDrawScope( ).left,
				.top = top,
				.right = diagrams[ selectDiag ].getDrawScope( ).right,
				.bottom = bottom
			} );
		}
		else if ( wParam == VK_DOWN ) {
			if ( diagrams.empty( ) ) {
				break;
			}

			auto top = diagrams[ selectDiag ].getDrawScope( ).top + 10;
			auto bottom = diagrams[ selectDiag ].getDrawScope( ).bottom + 10;
			diagrams[ selectDiag ].setDrawScope( RECT{
				.left = diagrams[ selectDiag ].getDrawScope( ).left,
				.top = top,
				.right = diagrams[ selectDiag ].getDrawScope( ).right,
				.bottom = bottom
				} );
		}

		InvalidateRect( hWnd, nullptr, true );
		break;

	case WM_PAINT:
		hDC = BeginPaint( hWnd, &ps );

		memDC = CreateCompatibleDC( hDC );
		memBitmap = CreateCompatibleBitmap( hDC, clWidth, clHeight );
		SelectObject( memDC, memBitmap );
		SelectObject( memDC, gWhiteBrush );
		SelectObject( memDC, gBlackPen );
		PatBlt( memDC, 0, 0, clWidth, clHeight, PATCOPY );

		// rendering =====

		Rectangle( memDC, 400, 0, 700, 30 );
		DrawText( memDC, inputBuffer.data( ), count, &inputRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER );

		for ( auto i = 0; i < diagrams.size( ); ++i ) {
			TextOut( memDC, 710, 0 + i * 20, diagrams[ i ].getInfo( ).c_str( ), diagrams[ i ].getInfo( ).size( ) );
		}

		if ( !diagrams.empty( ) ) {
			if ( c == 'a' ) {
				for ( const auto& diag : diagrams ) {
					diag.draw( memDC );
				}
			}
			else {
				diagrams[ selectDiag ].draw( memDC );
			}
		}

		// ===============

		BitBlt( hDC, 0, 0, clWidth, clHeight, memDC, 0, 0, SRCCOPY );

		DeleteObject( memBitmap );
		DeleteDC( memDC );

		EndPaint( hWnd, &ps );
		return 0;

	default:
		break;
	}

	return DefWindowProcW( hWnd, msg, wParam, lParam );
}

void processCmd( HWND hWnd, WPARAM cmd ) {
	switch ( cmd ) {
	case VK_BACK:
		inputBuffer[ --count ] = '\0';
		break;

	case VK_RETURN:

		break;

	case VK_LEFT:

		break;

	case VK_RIGHT:

		break;

	case VK_UP:

		break;

	case VK_DOWN:

		break;

	/*case '+':

		break;

	case '-':
		if ( gChosenShapeIdx == -1u ) {
			break;
		}
		gShapes[ gChosenShapeIdx ].scale = std::max( 1u, gShapes[ gChosenShapeIdx ].scale - 1u );
		break;*/

	/*case 'D':
		if ( gChosenShapeIdx == -1u ) {
			break;
		}
		DeleteObject( gBrushes[ gChosenShapeIdx ] );
		DeleteObject( gPens[ gChosenShapeIdx ] );
		std::move( &gShapes[ gChosenShapeIdx + 1 ], std::end( gShapes ), &gShapes[ gChosenShapeIdx ] );
		std::move( &gBrushes[ gChosenShapeIdx + 1 ], std::end( gBrushes ), &gBrushes[ gChosenShapeIdx ] );
		std::move( &gPens[ gChosenShapeIdx + 1 ], std::end( gPens ), &gPens[ gChosenShapeIdx ] );
		gChosenShapeIdx = std::min( 0u, gChosenShapeIdx - 1u );
		--gShapeCnt;
		if ( gShapeCnt == 0u ) {
			gChosenShapeIdx = -1u;
		}
		break;*/

	/*case 'C':

		break;

	case 'P':
		std::ranges::for_each( gBrushes, DeleteObject );
		std::ranges::for_each( gPens, DeleteObject );
		gChosenShapeIdx = -1u;
		gShapeCnt = 0u;
		if ( gSetType != ShapeType::None ) {
			DeleteObject( gSetBrush );
			DeleteObject( gSetPen );
			gSetType = ShapeType::None;
		}
		break;*/

	default:
		break;
	}

	InvalidateRect( hWnd, nullptr, true );
}

void Triangle( HDC memDC, int left, int top, int right, int bottom ) {
	POINT points[ 3 ] = {
		{ ( left + right ) / 2, top },
		{ left, bottom },
		{ right, bottom }
	};

	Polygon( memDC, points, 3 );
}

void Pentagon( HDC memDC, int left, int top, int right, int bottom ) {
	POINT points[ 5 ] = {
		{ ( left + right ) / 2, top },
		{ left, ( top + bottom ) / 2 },
		{ ( left * 2 + right ) / 3, bottom },
		{ ( left + right * 2 ) / 3, bottom },
		{ right, ( top + bottom ) / 2 }
	};

	Polygon( memDC, points, 5 );
}

void drawShape( HDC memDC, u32t i ) {
	/*auto& shape = gShapes[ i ];

	auto color = shape.color;
	auto type = shape.type;
	auto scale = shape.scale;
	auto pos = shape.pos;

	int left = pos.x * slotWidth - slotWidth * ( scale - 1u );
	int top = pos.y * slotHeight - slotHeight * ( scale - 1u );
	int right = ( pos.x + 1 ) * slotWidth + slotWidth * ( scale - 1u );
	int bottom = ( pos.y + 1 ) * slotHeight + slotHeight * ( scale - 1u );

	if ( type == gSetType ) {
		Pentagon( memDC, left, top, right, bottom );
		Pentagon( memDC, left, top - clHeight, right, bottom - clHeight );
		Pentagon( memDC, left, top + clHeight, right, bottom + clHeight );
		Pentagon( memDC, left - clWidth, top, right - clWidth, bottom );
		Pentagon( memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight );
		Pentagon( memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight );
		Pentagon( memDC, left + clWidth, top, right + clWidth, bottom );
		Pentagon( memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight );
		Pentagon( memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight );
		return;
	}

	switch ( type ) {
	case ShapeType::Circle:
		Ellipse( memDC, left, top, right, bottom );
		Ellipse( memDC, left, top - clHeight, right, bottom - clHeight );
		Ellipse( memDC, left, top + clHeight, right, bottom + clHeight );
		Ellipse( memDC, left - clWidth, top, right - clWidth, bottom );
		Ellipse( memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight );
		Ellipse( memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight );
		Ellipse( memDC, left + clWidth, top, right + clWidth, bottom );
		Ellipse( memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight );
		Ellipse( memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight );
		break;

	case ShapeType::Square:
		Rectangle( memDC, left, top, right, bottom );
		Rectangle( memDC, left, top - clHeight, right, bottom - clHeight );
		Rectangle( memDC, left, top + clHeight, right, bottom + clHeight );
		Rectangle( memDC, left - clWidth, top, right - clWidth, bottom );
		Rectangle( memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight );
		Rectangle( memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight );
		Rectangle( memDC, left + clWidth, top, right + clWidth, bottom );
		Rectangle( memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight );
		Rectangle( memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight );
		break;

	case ShapeType::Triangle:
		Triangle( memDC, left, top, right, bottom );
		Triangle( memDC, left, top - clHeight, right, bottom - clHeight );
		Triangle( memDC, left, top + clHeight, right, bottom + clHeight );
		Triangle( memDC, left - clWidth, top, right - clWidth, bottom );
		Triangle( memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight );
		Triangle( memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight );
		Triangle( memDC, left + clWidth, top, right + clWidth, bottom );
		Triangle( memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight );
		Triangle( memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight );
		break;
	}*/
}
