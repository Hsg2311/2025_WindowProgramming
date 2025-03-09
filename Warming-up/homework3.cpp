#include <iostream>
#include <random>
#include <vector>
#include <conio.h>

constexpr auto row = 10;
constexpr auto col = 13;

std::random_device rd;
std::mt19937 gen( rd( ) );

auto grid = std::vector<std::vector<char>>( row, std::vector<char>( col, ' ' ) );
auto fixed = std::vector<std::vector<bool>>( row, std::vector<bool>( col, false ) );
auto cursorX = 0;
auto cursorY = 0;
auto target = ' ';
auto cursor = ' ';

void InitGrid( );
void PrintGrid( );
void MoveCursor( int );
void SelectTarget( );
void ShuffleGrid( );

int main( )
{
	InitGrid( );
	while ( true ) {
		PrintGrid( );

		std::cout << '\n';
		std::cout << "a~z - 타겟 설정\n";
		std::cout << "1/2/3/4 - 좌/우/상/하 이동\n";
		std::cout << "Enter - 문자 선택\n";
		std::cout << "5 - 문자 랜덤 재배치\n";
		std::cout << "9 - 배열 초기화\n";
		std::cout << "0 - 종료\n";

		char cmd = _getch( );

		if ( cmd >= 'a' && cmd <= 'z' ) {
			target = cmd;
		}
		else if ( cmd >= '1' && cmd <= '4' ) {
			MoveCursor( cmd - '0' );
		}
		else if ( cmd == '\r' ) {
			SelectTarget( );
		}
		else if ( cmd == '5' ) {
			ShuffleGrid( );
		}
		else if ( cmd == '9' ) {
			InitGrid( );
			cursorX = 0;
			cursorY = 0;
			target = ' ';
		}
		else if ( cmd == '0' ) {
			break;
		}
	}
}

void InitGrid( ) {
	auto chars = std::vector<char>( );
	chars.reserve( row * col );
	for ( auto i = 0; i < 5; ++i ) {
		for ( auto j = 'a'; j <= 'z'; ++j ) {
			chars.push_back( j );
		}
	}
	std::shuffle( chars.begin( ), chars.end( ), gen );

	int idx = 0;
	for ( auto y = 0; y < row; ++y ) {
		for ( auto x = 0; x < col; ++x ) {
			grid[ y ][ x ] = chars[ idx++ ];
		}
	}

	for ( auto y = 0; y < row; ++y ) {
		for ( auto x = 0; x < col; ++x ) {
			fixed[ y ][ x ] = false;
		}
	}
}

void PrintGrid( ) {
	std::system( "cls" );
	for ( auto y = 0; y < row; ++y ) {
		for ( auto x = 0; x < col; ++x ) {
			if ( x == cursorX && y == cursorY ) {
				std::cout << "\033[31m" << grid[ y ][ x ] << "\033[0m ";
				cursor = grid[ y ][ x ];
			}
			else if ( grid[ y ][ x ] == 'O' ) {
				std::cout << "\033[32m" << grid[ y ][ x ] << "\033[0m ";
			}
			else {
				std::cout << grid[ y ][ x ] << ' ';
			}
		}
		std::cout << '\n';
	}
	std::cout << "Target: " << target << '\n';
}

void MoveCursor( int dir ) {
	switch ( dir ) {
	case 1:
		cursorX = std::max( 0, cursorX - 1 );
		break;

	case 2:
		cursorX = std::min( col - 1, cursorX + 1 );
		break;

	case 3:
		cursorY = std::max( 0, cursorY - 1 );
		break;

	case 4:
		cursorY = std::min( row - 1, cursorY + 1 );
		break;

	default:
		break;
	}
}

void SelectTarget( ) {
	if ( cursor == target ) {
		grid[ cursorY ][ cursorX ] = 'O';
		fixed[ cursorY ][ cursorX ] = true;
	}
	else {
		std::cout << '\a';
	}
}

void ShuffleGrid( ) {
	auto chars = std::vector<char>( );
	chars.reserve( row * col );
	for ( auto y = 0; y < row; ++y ) {
		for ( auto x = 0; x < col; ++x ) {
			if ( !fixed[ y ][ x ] ) {
				chars.push_back( grid[ y ][ x ] );
			}
		}
	}
	std::shuffle( chars.begin( ), chars.end( ), gen );

	int idx = 0;
	for ( auto y = 0; y < row; ++y ) {
		for ( auto x = 0; x < col; ++x ) {
			if ( !fixed[ y ][ x ] ) {
				grid[ y ][ x ] = chars[ idx++ ];
			}
		}
	}
}
