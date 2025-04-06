#include <Windows.h>
#include <tchar.h>
#include <array>
#include <cstdint>
#include <vector>
#include <iostream>
#include <random>

std::random_device rd;
std::mt19937_64 gen( rd( ) );
std::uniform_int_distribution randColor( 0, 255 );
std::uniform_int_distribution randChangeObjType( 3, 4 );
std::uniform_int_distribution randShape( 0, 3 );

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";
LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

auto xPos = std::vector<int>( );
auto yPos = std::vector<int>( );

auto winRect = RECT{
	.left = 0,
	.top = 0,
	.right = 800,
	.bottom = 800
};

void DrawBoard( HDC hdc );
int offset = winRect.right / 40;

class Object;
void CheckCollision( Object& obj, int xOffset, int yoffset );
void resetGame( );

COLORREF obstacleColor = RGB( 255, 0, 0 );
COLORREF scaleUpObjColor = RGB( 150, 150, 150 );
COLORREF scaleDownObjColor = RGB( 100, 100, 100 );

enum class ObjectType {
	Obstacle,
	ScaleUpObject,
	ScaleDownObject,
	ChangeColorObj,
	ChangeShapeObj,
	Player,
	Target,
};

enum class Shape {
	Rectangle,
	Triangle,
	Circle,
	Ellipse,
	None,
};

class Object {
public:
	Object( ObjectType objType, POINT pos, COLORREF color, Shape shape = Shape::None )
		: objType_( objType ), pos_( pos ), color_( color ), shape_( shape ) { }

	ObjectType getObjType( ) const { return objType_; }
	POINT getPos( ) const { return pos_; }
	COLORREF getColor( ) const { return color_; }
	Shape getShape( ) const { return shape_; }
	int getPlayerOffset( ) const { return playerOffset_; }

	void setPos( POINT pos ) { pos_ = pos; }
	void setColor( COLORREF color ) { color_ = color; }
	void setShape( Shape shape ) { shape_ = shape; }
	void setPlayerOffset( int offset ) { playerOffset_ = offset; }
	void setOriginShape( ) { shape_ = originShape_; }

	bool compShape( ) const {
		return shape_ == originShape_;
	}

	bool operator==( const Object& other ) const {
		return pos_.x == other.pos_.x && pos_.y == other.pos_.y && shape_ == other.shape_;
	}

private:
	ObjectType objType_;
	POINT pos_;
	COLORREF color_;
	Shape shape_;
	Shape originShape_ = Shape::Rectangle;

	int playerOffset_ = winRect.right / 40;

public:
	int movement_ = 20;
};
auto objects = std::vector<Object>( );

auto player1 = Object( ObjectType::Player, POINT{ 0, 0 },
	RGB( randColor( gen ), randColor( gen ), randColor( gen ) ), Shape::Rectangle );
auto player2 = Object( ObjectType::Player, POINT{ 780, 0 },
	RGB( randColor( gen ), randColor( gen ), randColor( gen ) ), Shape::Rectangle );
auto target = Object( ObjectType::Target, POINT{ 400, 780 },
	RGB( randColor( gen ), randColor( gen ), randColor( gen ) ), static_cast<Shape>( randShape( gen ) ) );

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
		for ( auto i = 20; i <= 760; i += 20 ) {
			xPos.push_back( i );
			yPos.push_back( i );
		}
		std::shuffle( xPos.begin( ), xPos.end( ), gen );
		std::shuffle( yPos.begin( ), yPos.end( ), gen );
		
		for ( auto i = 0; i < 40; ++i ) {
			if ( 0 <= i && i < 15 ) {
				objects.push_back( Object( ObjectType::Obstacle, POINT{ xPos[ i ], yPos[ i ] }, obstacleColor ) );
			}
			else if ( 15 <= i && i < 20 ) {
				objects.push_back( Object( ObjectType::ScaleUpObject, POINT{ xPos[ i ], yPos[ i ] }, scaleUpObjColor ) );
			}
			else if ( 20 <= i && i < 25 ) {
				objects.push_back( Object( ObjectType::ScaleDownObject, POINT{ xPos[ i ], yPos[ i ] }, scaleDownObjColor ) );
			}
			else if ( 25 <= i && i < 38 ) {
				objects.push_back( Object( static_cast<ObjectType>( randChangeObjType( gen ) ), POINT{ xPos[ i ], yPos[ i ] },
					RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) ) );
			}
		}
		break;

	case WM_PAINT: {
		hDC = BeginPaint( hWnd, &ps );

		DrawBoard( hDC );
		
		for ( auto i = 0; i < objects.size( ); ++i ) {
			auto obj = objects[ i ];
			auto objType = obj.getObjType( );
			auto objPos = obj.getPos( );
			auto objColor = obj.getColor( );

			auto newBrush = CreateSolidBrush( objColor );
			auto oldBrush = (HBRUSH)SelectObject( hDC, newBrush );

			switch ( objType ) {
			case ObjectType::Obstacle:
				Rectangle( hDC, objPos.x, objPos.y, objPos.x + offset, objPos.y + offset );
				break;
			case ObjectType::ScaleUpObject: {
				auto arr = std::array<POINT, 3>{
					POINT{ objPos.x + offset / 2, objPos.y },
					POINT{ objPos.x, objPos.y + offset },
					POINT{ objPos.x + offset, objPos.y + offset }
				};
				Polygon( hDC, arr.data( ), arr.size( ) );
				break;
			}
			case ObjectType::ScaleDownObject: {
				auto arr = std::array<POINT, 3>{
					POINT{ objPos.x, objPos.y },
					POINT{ objPos.x + offset / 2, objPos.y + offset },
					POINT{ objPos.x + offset, objPos.y }
				};
				Polygon( hDC, arr.data( ), arr.size( ) );
				break;
			}
			case ObjectType::ChangeColorObj:
				Rectangle( hDC, objPos.x, objPos.y, objPos.x + offset, objPos.y + offset );
				break;
			case ObjectType::ChangeShapeObj: {
				auto arr = std::array<POINT, 4>{
					POINT{ objPos.x + offset / 2, objPos.y },
					POINT{ objPos.x, objPos.y + offset / 2 },
					POINT{ objPos.x + offset / 2, objPos.y + offset },
					POINT{ objPos.x + offset, objPos.y + offset / 2 }
				};
				Polygon( hDC, arr.data( ), arr.size( ) );
				break;
			}
			default:
				break;
			}

			SelectObject( hDC, oldBrush );
			DeleteObject( newBrush );
		}

		auto newBrush = CreateSolidBrush( player1.getColor( ) );
		auto oldBrush = (HBRUSH)SelectObject( hDC, newBrush );

		auto obj = player1.getPos( );
		auto playerObjCenter = POINT{ obj.x + offset / 2, obj.y + offset / 2 };
		auto playerShape = player1.getShape( );
		auto playerOffset = player1.getPlayerOffset( );

		switch ( playerShape ) {
		case Shape::Rectangle:
			Rectangle( hDC, 
				playerObjCenter.x - playerOffset / 2,
				playerObjCenter.y - playerOffset / 2,
				playerObjCenter.x + playerOffset / 2,
				playerObjCenter.y + playerOffset / 2 
			);
			break;

		case Shape::Triangle: {
			auto arr = std::array<POINT, 3>{
				POINT{ playerObjCenter.x, playerObjCenter.y - playerOffset / 2 },
				POINT{ playerObjCenter.x - playerOffset / 2, playerObjCenter.y + playerOffset / 2 },
				POINT{ playerObjCenter.x + playerOffset / 2, playerObjCenter.y + playerOffset / 2 }
			};
			Polygon( hDC, arr.data( ), arr.size( ) );
			break;
		}

		case Shape::Circle:
			Ellipse( hDC,
				playerObjCenter.x - playerOffset / 2,
				playerObjCenter.y - playerOffset / 2,
				playerObjCenter.x + playerOffset / 2,
				playerObjCenter.y + playerOffset / 2 
			);
			break;

		case Shape::Ellipse:
			Ellipse( hDC,
				playerObjCenter.x - playerOffset / 2,
				playerObjCenter.y - playerOffset / 2 + 2,
				playerObjCenter.x + playerOffset / 2,
				playerObjCenter.y + playerOffset / 2 - 2
			);
			break;

		default:
			break;
		}

		SelectObject( hDC, oldBrush );
		DeleteObject( newBrush );

		newBrush = CreateSolidBrush( target.getColor( ) );
		oldBrush = (HBRUSH)SelectObject( hDC, newBrush );

		obj = target.getPos( );
		auto targetShape = target.getShape( );

		switch ( targetShape ) {
		case Shape::Rectangle:
			Rectangle( hDC,
				obj.x,
				obj.y,
				obj.x + offset,
				obj.y + offset
			);
			break;

		case Shape::Triangle: {
			auto arr = std::array<POINT, 3>{
				POINT{ obj.x + offset / 2, obj.y },
				POINT{ obj.x, obj.y + offset },
				POINT{ obj.x + offset, obj.y + offset }
			};
			Polygon( hDC, arr.data( ), arr.size( ) );
			break;
		}

		case Shape::Circle:
			Ellipse( hDC,
				obj.x,
				obj.y,
				obj.x + offset,
				obj.y + offset
			);
			break;

		case Shape::Ellipse:
			Ellipse( hDC,
				obj.x,
				obj.y + 3,
				obj.x + offset,
				obj.y + offset - 3
			);
			break;

		default:
			break;
		}

		SelectObject( hDC, oldBrush );
		DeleteObject( newBrush );

		newBrush = CreateSolidBrush( player2.getColor( ) );
		oldBrush = (HBRUSH)SelectObject( hDC, newBrush );

		obj = player2.getPos( );
		playerObjCenter = POINT{ obj.x + offset / 2, obj.y + offset / 2 };
		playerShape = player2.getShape( );
		playerOffset = player2.getPlayerOffset( );

		switch ( playerShape ) {
		case Shape::Rectangle:
			Rectangle( hDC,
				playerObjCenter.x - playerOffset / 2,
				playerObjCenter.y - playerOffset / 2,
				playerObjCenter.x + playerOffset / 2,
				playerObjCenter.y + playerOffset / 2
			);
			break;

		case Shape::Triangle: {
			auto arr = std::array<POINT, 3>{
				POINT{ playerObjCenter.x, playerObjCenter.y - playerOffset / 2 },
				POINT{ playerObjCenter.x - playerOffset / 2, playerObjCenter.y + playerOffset / 2 },
				POINT{ playerObjCenter.x + playerOffset / 2, playerObjCenter.y + playerOffset / 2 }
			};
			Polygon( hDC, arr.data( ), arr.size( ) );
			break;
		}

		case Shape::Circle:
			Ellipse( hDC,
				playerObjCenter.x - playerOffset / 2,
				playerObjCenter.y - playerOffset / 2,
				playerObjCenter.x + playerOffset / 2,
				playerObjCenter.y + playerOffset / 2
			);
			break;

		case Shape::Ellipse:
			Ellipse( hDC,
				playerObjCenter.x - playerOffset / 2,
				playerObjCenter.y - playerOffset / 2 + 2,
				playerObjCenter.x + playerOffset / 2,
				playerObjCenter.y + playerOffset / 2 - 2
			);
			break;

		default:
			break;
		}

		SelectObject( hDC, oldBrush );
		DeleteObject( newBrush );

		if ( player1 == target ) {
			MessageBox( hWnd, L"Player 1 wins!", L"Game Over", MB_OK );
		}
		else if ( player2 == target ) {
			MessageBox( hWnd, L"Player 2 wins!", L"Game Over", MB_OK );
		}

		EndPaint( hWnd, &ps );
		break;
	}

	case WM_KEYDOWN:
		if ( wParam == 'Q' ) {
			PostQuitMessage( 0 );
			return 0;
		}
		else if ( wParam == 'W' ) {
			auto obj = player1.getPos( );
			if ( obj.y > 0 ) {
				if ( player1.movement_ == 0 ) {
					player1.movement_ = 20;
					player1.setOriginShape( );
				}

				obj.y -= offset;
				player1.setPos( obj );
				CheckCollision( player1, 0, offset );

				if ( !player1.compShape( ) ) {
					--player1.movement_;
				}
			}
		}
		else if ( wParam == 'A' ) {
			auto obj = player1.getPos( );
			if ( obj.x > 0 ) {
				if ( player1.movement_ == 0 ) {
					player1.movement_ = 20;
					player1.setOriginShape( );
				}

				obj.x -= offset;
				player1.setPos( obj );
				CheckCollision( player1, offset, 0 );

				if ( !player1.compShape( ) ) {
					--player1.movement_;
				}
			}
		}
		else if ( wParam == 'S' ) {
			auto obj = player1.getPos( );
			if ( obj.y < winRect.bottom - offset - 8 ) {	// 8을 빼는 이유는 adjustWindowRect를 하면서 늘어면 윈도우 테두리 크기 때문이다.
				if ( player1.movement_ == 0 ) {
					player1.movement_ = 20;
					player1.setOriginShape( );
				}

				obj.y += offset;
				player1.setPos( obj );
				CheckCollision( player1, 0, -offset );

				if ( !player1.compShape( ) ) {
					--player1.movement_;
				}
			}
		}
		else if ( wParam == 'D' ) {
			auto obj = player1.getPos( );
			if ( obj.x < winRect.right - offset - 8 ) {
				if ( player1.movement_ == 0 ) {
					player1.movement_ = 20;
					player1.setOriginShape( );
				}

				obj.x += offset;
				player1.setPos( obj );
				CheckCollision( player1, -offset, 0 );

				if ( !player1.compShape( ) ) {
					--player1.movement_;
				}
			}
		}
		else if ( wParam == 'I' ) {
			auto obj = player2.getPos( );
			if ( obj.y > 0 ) {
				if ( player2.movement_ == 0 ) {
					player2.movement_ = 20;
					player2.setOriginShape( );
				}

				obj.y -= offset;
				player2.setPos( obj );
				CheckCollision( player2, 0, offset );

				if ( !player2.compShape( ) ) {
					--player2.movement_;
				}
			}
		}
		else if ( wParam == 'J' ) {
			auto obj = player2.getPos( );
			if ( obj.x > 0 ) {
				if ( player2.movement_ == 0 ) {
					player2.movement_ = 20;
					player2.setOriginShape( );
				}

				obj.x -= offset;
				player2.setPos( obj );
				CheckCollision( player2, offset, 0 );

				if ( !player2.compShape( ) ) {
					--player2.movement_;
				}
			}
		}
		else if ( wParam == 'K' ) {
			auto obj = player2.getPos( );
			if ( obj.y < winRect.bottom - offset - 8 ) {	// 8을 빼는 이유는 adjustWindowRect를 하면서 늘어면 윈도우 테두리 크기 때문이다.
				if ( player2.movement_ == 0 ) {
					player2.movement_ = 20;
					player2.setOriginShape( );
				}

				obj.y += offset;
				player2.setPos( obj );
				CheckCollision( player2, 0, -offset );

				if ( !player2.compShape( ) ) {
					--player2.movement_;
				}
			}
		}
		else if ( wParam == 'L' ) {
			auto obj = player2.getPos( );
			if ( obj.x < winRect.right - offset - 8 ) {
				if ( player2.movement_ == 0 ) {
					player2.movement_ = 20;
					player2.setOriginShape( );
				}

				obj.x += offset;
				player2.setPos( obj );
				CheckCollision( player2, -offset, 0 );

				if ( !player2.compShape( ) ) {
					--player2.movement_;
				}
			}
		}
		else if ( wParam == 'R' ) {
			resetGame( );
		}

		InvalidateRect( hWnd, nullptr, true );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}
	return ( DefWindowProc( hWnd, iMessage, wParam, lParam ) );
}

void DrawBoard( HDC hdc ) {
	// Draw the grid
	for ( auto i = 0; i < 40; ++i ) {
		MoveToEx( hdc, i * offset, 0, nullptr );
		LineTo( hdc, i * offset, winRect.bottom );
	}
	for ( auto i = 0; i < 40; ++i ) {
		MoveToEx( hdc, 0, i * offset, nullptr );
		LineTo( hdc, winRect.right, i * offset );
	}
}

void CheckCollision( Object& obj, int xOffset, int yoffset ) {
	auto playerPos = obj.getPos( );
	auto playerOffset = obj.getPlayerOffset( );
	for ( auto i = 0; i < objects.size( ); ++i ) {
		auto objPos = objects[ i ].getPos( );
		
		auto playerRect = RECT{
			.left = playerPos.x,
			.top = playerPos.y,
			.right = playerPos.x + offset,
			.bottom = playerPos.y + offset
		};
		auto objRect = RECT{
			.left = objPos.x,
			.top = objPos.y,
			.right = objPos.x + offset,
			.bottom = objPos.y + offset
		};

		auto tmpRect = RECT( );
		if ( IntersectRect( &tmpRect, &playerRect, &objRect ) ) {
			switch ( objects[ i ].getObjType( ) ) {
			case ObjectType::Obstacle:
				obj.setPos( POINT{ playerPos.x + xOffset, playerPos.y + yoffset } );
				break;

			case ObjectType::ScaleUpObject:
				if ( playerOffset < 20 ) {
					obj.setPlayerOffset( playerOffset + 2 );
				}
				break;

			case ObjectType::ScaleDownObject:
				if ( playerOffset > 10 ) {
					obj.setPlayerOffset( playerOffset - 2 );
				}
				break;

			case ObjectType::ChangeColorObj:
				obj.setColor( objects[ i ].getColor( ) );
				break;

			case ObjectType::ChangeShapeObj:
				obj.setShape( static_cast<Shape>( randShape( gen ) ) );
				obj.movement_ = 20;
				break;
			}
			return;
		}
	}
}

void resetGame( ) {
	objects.clear( );
	xPos.clear( );
	yPos.clear( );

	for ( auto i = 20; i <= 760; i += 20 ) {
		xPos.push_back( i );
		yPos.push_back( i );
	}
	std::shuffle( xPos.begin( ), xPos.end( ), gen );
	std::shuffle( yPos.begin( ), yPos.end( ), gen );

	for ( auto i = 0; i < 40; ++i ) {
		if ( 0 <= i && i < 15 ) {
			objects.push_back( Object( ObjectType::Obstacle, POINT{ xPos[ i ], yPos[ i ] }, obstacleColor ) );
		}
		else if ( 15 <= i && i < 20 ) {
			objects.push_back( Object( ObjectType::ScaleUpObject, POINT{ xPos[ i ], yPos[ i ] }, scaleUpObjColor ) );
		}
		else if ( 20 <= i && i < 25 ) {
			objects.push_back( Object( ObjectType::ScaleDownObject, POINT{ xPos[ i ], yPos[ i ] }, scaleDownObjColor ) );
		}
		else if ( 25 <= i && i < 38 ) {
			objects.push_back( Object( static_cast<ObjectType>( randChangeObjType( gen ) ), POINT{ xPos[ i ], yPos[ i ] },
				RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) ) );
		}
	}

	player1.setPos( POINT{ 0, 0 } );
	player1.setColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
	player1.setShape( Shape::Rectangle );
	player1.setPlayerOffset( winRect.right / 40 );
	player1.movement_ = 20;

	player2.setPos( POINT{ 780, 0 } );
	player2.setColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
	player2.setShape( Shape::Rectangle );
	player2.setPlayerOffset( winRect.right / 40 );
	player2.movement_ = 20;

	target.setColor( RGB( randColor( gen ), randColor( gen ), randColor( gen ) ) );
	target.setShape( static_cast<Shape>( randShape( gen ) ) );
}
