#define _USE_MATH_DEFINES
#include <Windows.h>
#include <tchar.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>

std::random_device rd;
std::mt19937_64 gen( rd( ) );
std::uniform_int_distribution randColor( 0, 255 );

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
	.x = 200,
	.y = 200
};

class Diagram {
public:
	Diagram( COLORREF color, int x, int y ) : color_( color ), prevColor_( ), xOffset_( x ), yOffset_( y ) { }

	POINT rotatePoint( const POINT& point, const POINT& center, float degree ) {
		float radian = static_cast<float>( degree * ( M_PI / 180.f ) );
		float s = sin( radian );
		float c = cos( radian );

		auto result = POINT( );
		result.x = static_cast<LONG>( c * ( point.x - center.x ) - s * ( point.y - center.y ) + center.x );
		result.y = static_cast<LONG>( s * ( point.x - center.x ) + c * ( point.y - center.y ) + center.y );

		return result;
	}

	void setColor( COLORREF color ) {
		color_ = color;
	}

	void setOriginalColor( ) {
		color_ = prevColor_;
	}

	void setPrevColor( ) {
		prevColor_ = color_;
	}

	void moveLeft( ) {
		if ( xOffset_ == 0 && yOffset_ < 0 ) {
			xOffset_ = -170;
			yOffset_ = 0;
		}
		else if ( xOffset_ == 0 && yOffset_ > 0 ) {
			xOffset_ = 170;
			yOffset_ = 0;
		}
		else if ( xOffset_ < 0 && yOffset_ == 0 ) {
			xOffset_ = 0;
			yOffset_ = 170;
		}
		else if ( xOffset_ > 0 && yOffset_ == 0 ) {
			xOffset_ = 0;
			yOffset_ = -170;
		}
	}

	void moveRight( ) {
		if ( xOffset_ == 0 && yOffset_ < 0 ) {
			xOffset_ = 170;
			yOffset_ = 0;
		}
		else if ( xOffset_ == 0 && yOffset_ > 0 ) {
			xOffset_ = -170;
			yOffset_ = 0;
		}
		else if ( xOffset_ < 0 && yOffset_ == 0 ) {
			xOffset_ = 0;
			yOffset_ = -170;
		}
		else if ( xOffset_ > 0 && yOffset_ == 0 ) {
			xOffset_ = 0;
			yOffset_ = 170;
		}
	}

protected:
	COLORREF color_;
	COLORREF prevColor_;

	int xOffset_;
	int yOffset_;
};

class Hourglass : public Diagram {
public:
	Hourglass( const std::array<POINT, 3>& top, 
		const std::array<POINT, 3>& bottom, COLORREF color ) 
		: Diagram( color, 0, -170 ), topTriangle_( top ), bottomTriangle_( bottom ) {
		for ( auto i = 0; i < topTriangle_.size( ); ++i ) {
			leftTriangle_[ i ] = rotatePoint( topTriangle_[ i ], centerRectPos, 90.f );
			rightTriangle_[ i ] = rotatePoint( bottomTriangle_[ i ], centerRectPos, 90.f );
		}
	}

	void draw( HDC hdc ) {
		auto copyTopTriangle = topTriangle_;
		auto copyBottomTriangle = bottomTriangle_;
		for ( auto i = 0; i < topTriangle_.size( ); ++i ) {
			copyTopTriangle[ i ].x += xOffset_;
			copyTopTriangle[ i ].y += yOffset_;
			copyBottomTriangle[ i ].x += xOffset_;
			copyBottomTriangle[ i ].y += yOffset_;
		}

		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );

		Polygon( hdc, copyTopTriangle.data( ), 3 );
		Polygon( hdc, copyBottomTriangle.data( ), 3 );

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

	void drawRotated( HDC hdc ) {
		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );

		Polygon( hdc, leftTriangle_.data( ), 3 );
		Polygon( hdc, rightTriangle_.data( ), 3 );

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

private:
	std::array<POINT, 3> topTriangle_;
	std::array<POINT, 3> bottomTriangle_;

	std::array<POINT, 3> leftTriangle_;
	std::array<POINT, 3> rightTriangle_;
};

class Circle : public Diagram {
public:
	Circle( const POINT& pos, const POINT& scale, COLORREF color )
		: Diagram( color, -170, 0 ), pos_( pos ), scale_( scale ) {
	}

	void draw( HDC hdc ) {
		auto copyPos = pos_;
		copyPos.x += xOffset_;
		copyPos.y += yOffset_;

		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );

		Ellipse( hdc,
			copyPos.x - scale_.x / 2,
			copyPos.y - scale_.y / 2,
			copyPos.x + scale_.x / 2,
			copyPos.y + scale_.y / 2
		);

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

	void drawEllipse( HDC hdc ) {
		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );

		Ellipse( hdc,
			pos_.x - scale_.x / 2 - 20,
			pos_.y - scale_.y / 2,
			pos_.x + scale_.x / 2 + 20,
			pos_.y + scale_.y / 2
		);

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

private:
	POINT pos_;
	POINT scale_;
};

class Pentagon : public Diagram {
public:
	Pentagon( const std::array<POINT, 5>& pentagon, COLORREF color )
		: Diagram( color, 170, 0 ), pentagon_( pentagon ) {
		for ( auto i = 0; i < pentagon_.size( ); ++i ) {
			pentagonRotated_[ i ] = rotatePoint( pentagon_[ i ], centerRectPos, 180.f );
		}
	}

	void draw( HDC hdc ) {
		auto copyPentagon = pentagon_;
		for ( auto i = 0; i < pentagon_.size( ); ++i ) {
			copyPentagon[ i ].x += xOffset_;
			copyPentagon[ i ].y += yOffset_;
		}

		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );

		Polygon( hdc, copyPentagon.data( ), 5 );

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

	void drawRotated( HDC hdc ) {
		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );
		
		Polygon( hdc, pentagonRotated_.data( ), 5 );

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

private:
	std::array<POINT, 5> pentagon_;
	std::array<POINT, 5> pentagonRotated_;
};

class MyPie : public Diagram {
public:
	MyPie( const POINT& leftTop, const POINT& rightBottom,
		const POINT& startPoint, const POINT& endPoint, COLORREF color )
		: Diagram( color, 0, 170 ), leftTop_( leftTop ), rightBottom_( rightBottom ),
		start_( startPoint ), end_( endPoint ) {
		rStart_ = end_;
		rEnd_ = start_;
	}

	void draw( HDC hdc ) {
		auto copyLeftTop = leftTop_;
		auto copyRightBottom = rightBottom_;
		auto copyStart = start_;
		auto copyEnd = end_;

		copyLeftTop.x += xOffset_;
		copyLeftTop.y += yOffset_;
		copyRightBottom.x += xOffset_;
		copyRightBottom.y += yOffset_;
		copyStart.x += xOffset_;
		copyStart.y += yOffset_;
		copyEnd.x += xOffset_;
		copyEnd.y += yOffset_;

		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );

		Pie( hdc,
			copyLeftTop.x, copyLeftTop.y,
			copyRightBottom.x, copyRightBottom.y,
			copyStart.x, copyStart.y,
			copyEnd.x, copyEnd.y
		);

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

	void drawRemained( HDC hdc ) {
		auto newBrush = CreateSolidBrush( color_ );
		auto oldBrush = (HBRUSH)SelectObject( hdc, newBrush );
		
		Pie( hdc,
			leftTop_.x, leftTop_.y,
			rightBottom_.x, rightBottom_.y,
			rStart_.x, rStart_.y,
			rEnd_.x, rEnd_.y
		);

		SelectObject( hdc, oldBrush );
		DeleteObject( newBrush );
	}

private:
	POINT leftTop_;
	POINT rightBottom_;
	POINT start_;
	POINT end_;

	POINT rStart_;
	POINT rEnd_;
};

auto topTriangle = std::array<POINT, 3>{
	POINT( 400, 300 ),
	POINT( 350, 250 ),
	POINT( 450, 250 )
};
auto bottomTriangle = std::array<POINT, 3>{
	POINT( 400, 300 ),
	POINT( 350, 350 ),
	POINT( 450, 350 )
};
auto hourglass = Hourglass( topTriangle, bottomTriangle, RGB( 255, 0, 0 ) );

auto circleScale = POINT{
	.x = 100,
	.y = 100
};
auto circle = Circle( centerRectPos, circleScale, RGB( 0, 0, 255 ) );

auto pentagonPos = std::array<POINT, 5>{
	POINT( 400, 250 ),
	POINT( 350, 290 ),
	POINT( 370, 350 ),
	POINT( 430, 350 ),
	POINT( 450, 290 )
};
auto pentagon = Pentagon( pentagonPos, RGB( 255, 165, 0 ) );

auto pieLeftTop = POINT{
	.x = 350,
	.y = 250
};
auto pieRightBottom = POINT{
	.x = 450,
	.y = 350
};
auto pieStart = POINT{
	.x = 400,
	.y = 250
};
auto pieEnd = POINT{
	.x = 450,
	.y = 300
};
auto pie = MyPie( pieLeftTop, pieRightBottom, pieStart, pieEnd, RGB( 0, 255, 0 ) );

bool isCPressed = false;
bool isSPressed = false;
bool isPPressed = false;
bool isEPressed = false;

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

		hourglass.draw( hDC );
		if ( isSPressed ) {
			hourglass.drawRotated( hDC );
		}

		circle.draw( hDC );
		if ( isCPressed ) {
			circle.drawEllipse( hDC );
		}
		
		pentagon.draw( hDC );
		if ( isPPressed ) {
			pentagon.drawRotated( hDC );
		}

		pie.draw( hDC );
		if ( isEPressed ) {
			pie.drawRemained( hDC );
		}
		
		EndPaint( hWnd, &ps );
		break;
	}

	case WM_KEYDOWN:
		if ( wParam == 'Q' ) {
			PostQuitMessage( 0 );
			break;
		}
		else if ( wParam == 'C' ) {
			if ( !isCPressed ) {
				circle.setPrevColor( );
				circle.setColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
				isCPressed = true;
			}
		}
		else if ( wParam == 'S' ) {
			if ( !isSPressed ) {
				hourglass.setPrevColor( );
				hourglass.setColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
				isSPressed = true;
			}
		}
		else if ( wParam == 'P' ) {
			if ( !isPPressed ) {
				pentagon.setPrevColor( );
				pentagon.setColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
				isPPressed = true;
			}
		}
		else if ( wParam == 'E' ) {
			if ( !isEPressed ) {
				pie.setPrevColor( );
				pie.setColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
				isEPressed = true;
			}
		}
		else if ( wParam == VK_LEFT ) {
			circle.moveLeft( );
			hourglass.moveLeft( );
			pentagon.moveLeft( );
			pie.moveLeft( );
		}
		else if ( wParam == VK_RIGHT ) {
			circle.moveRight( );
			hourglass.moveRight( );
			pentagon.moveRight( );
			pie.moveRight( );
		}
		InvalidateRect( hWnd, nullptr, true );
		break;

	case WM_KEYUP:
		if ( wParam == 'C' ) {
			isCPressed = false;
			circle.setOriginalColor( );
		}
		else if ( wParam == 'S' ) {
			isSPressed = false;
			hourglass.setOriginalColor( );
		}
		else if ( wParam == 'P' ) {
			isPPressed = false;
			pentagon.setOriginalColor( );
		}
		else if ( wParam == 'E' ) {
			isEPressed = false;
			pie.setOriginalColor( );
		}
		InvalidateRect( hWnd, nullptr, true );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}

