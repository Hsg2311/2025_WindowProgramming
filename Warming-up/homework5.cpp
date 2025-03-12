#include <iostream>
#include <random>
#include <array>
#include <vector>
#include <conio.h>

std::random_device rd;
std::mt19937 gen( rd( ) );

constexpr auto size = 20;
constexpr auto base = '-';
constexpr auto wall = 'L';
constexpr auto item = 'o';
constexpr auto p1 = 'x';
constexpr auto p2 = '@';

char board[ size ][ size ];
auto p1_x = 0, p1_y = 0, p2_x = 0, p2_y = 0;
auto p1_score = 0, p2_score = 0;
auto itemCount = 10;
bool p1_turn = true;

void InitBoard( );
void PrintBoard( );
void MovePlayer( char player, int& px, int& py, int dx, int dy );
void PlayGame( );

int main( )
{
	InitBoard( );
	PlayGame( );
}

void InitBoard( ) {
	for ( auto y = 0; y < size; ++y ) {
		for ( auto x = 0; x < size; ++x ) {
			board[ x ][ y ] = base;
		}
	}
	
	std::vector<std::pair<int, int>> obstacles{ { 3, 4 }, { 5, 2 }, { 4, 4 } };
	for ( auto [w, h] : obstacles ) {
		std::uniform_int_distribution<> uidW{ 0, size - 1 - w };
		std::uniform_int_distribution<> uidH{ 0, size - 1 - h };
		auto x = uidW( gen );
		auto y = uidH( gen );
		bool valid = false;

		while ( !valid ) {
			valid = true;
			for ( auto i = 0; i < h; ++i ) {
				for ( auto j = 0; j < w; ++j ) {
					if ( board[ x + j ][ y + i ] != base ) {
						x = uidW( gen );
						y = uidH( gen );
						valid = false;
						break;
					}
				}
				if ( !valid ) {
					break;
				}
			}
		}

		for ( auto i = 0; i < h; ++i ) {
			for ( auto j = 0; j < w; ++j ) {
				board[ x + j ][ y + i ] = wall;
			}
		}
	}

	std::uniform_int_distribution<> uidX{ 0, size - 1 };
	std::uniform_int_distribution<> uidY{ 0, size - 1 };

	for ( auto i = 0; i < itemCount; ++i ) {
		auto x = uidX( gen );
		auto y = uidY( gen );

		while ( board[ x ][ y ] != base ) {
			x = uidX( gen );
			y = uidY( gen );
		}
		board[ x ][ y ] = item;
	}

	p1_x = 0, p1_y = 0;
	p2_x = size - 1, p2_y = size - 1;
	board[ p1_x ][ p1_y ] = p1;
	board[ p2_x ][ p2_y ] = p2;
}

void PrintBoard( ) {
	std::system( "cls" );
	for ( auto y = 0; y < size; ++y ) {
		for ( auto x = 0; x < size; ++x ) {
			std::cout << board[ x ][ y ] << ' ';
		}
		std::cout << '\n';
	}
	std::cout << "P1(x) Score: " << p1_score << " | P2(@) Score: " << p2_score << '\n';
	if ( p1_turn ) {
		std::cout << "P1(x) Turn\n";
	}
	else {
		std::cout << "P2(@) Turn\n";
	}
}

void MovePlayer( char player, int& px, int& py, int dx, int dy ) {
	auto x = px + dx;
	auto y = py + dy;

	if ( x < 0 || x >= size || y < 0 || y >= size ) {
		return;
	}

	if ( board[ x ][ y ] == wall ) {
		return;
	}

	if ( board[ x ][ y ] == item ) {
		if ( player == p1 ) {
			++p1_score;
		}
		else {
			++p2_score;
		}
		--itemCount;
	}

	board[ px ][ py ] = base;
	board[ x ][ y ] = player;
	px = x;
	py = y;	
}

void PlayGame( ) {
	while ( itemCount > 0 ) {
		PrintBoard( );
		char cmd = _getch( );

		if ( p1_turn ) {
			switch ( cmd ) {
			case 'w':
				MovePlayer( p1, p1_x, p1_y, 0, -1 );
				break;
			case 's':
				MovePlayer( p1, p1_x, p1_y, 0, 1 );
				break;
			case 'a':
				MovePlayer( p1, p1_x, p1_y, -1, 0 );
				break;
			case 'd':
				MovePlayer( p1, p1_x, p1_y, 1, 0 );
				break;
			default:
				break;
			}
		}
		else {
			switch ( cmd ) {
			case 'i':
				MovePlayer( p2, p2_x, p2_y, 0, -1 );
				break;
			case 'k':
				MovePlayer( p2, p2_x, p2_y, 0, 1 );
				break;
			case 'j':
				MovePlayer( p2, p2_x, p2_y, -1, 0 );
				break;
			case 'l':
				MovePlayer( p2, p2_x, p2_y, 1, 0 );
				break;
			default:
				break;
			}
		}

		p1_turn = !p1_turn;
	}

	// 게임 종료
	PrintBoard( );
	if ( p1_score > p2_score ) {
		std::cout << "P1(x) Win!\n";
	}
	else if ( p1_score < p2_score ) {
		std::cout << "P2(@) Win!\n";
	}
	else {
		std::cout << "Draw!\n";
	}
}
