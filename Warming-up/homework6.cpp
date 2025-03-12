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
auto cursor = ' ';

auto words = std::vector<std::string>{ "computer", "engineering", "software", "spring", "mathematics" };
auto currWord = std::string( );
auto dispWord = std::string( );
bool active = false;
bool correct = false;
bool open = false;

void InitGrid( );
void PrintGrid( );
void MoveCursor( int );
void ShuffleGrid( );
void SelectLetter( );
void SelectWord( int );
bool IsCorrect( char );

int main( )
{
	InitGrid( );
	while ( true ) {
		PrintGrid( );

		if ( !active ) {
			if ( correct ) {
				std::cout << "정답입니다! - 단어: " << currWord << '\n';
				correct = false;
			}

			for ( auto i = 0; i < words.size( ); ++i ) {
				std::cout << i + 1 << ") " << words[ i ] << " ";
			}
			std::cout << '\n';
			std::cout << "단어 선택(1~5) or 게임 종료\n";
			char cmd = _getch( );

			if ( cmd >= '1' && cmd <= '5' ) {
				SelectWord( cmd - '0' - 1 );
			}
			else if ( cmd == '0' ) {
				break;
			}
		}

		std::cout << '\n';
		std::cout << "1/2/3/4 - 좌/우/상/하 이동\n";
		std::cout << "Enter - 문자 선택\n";
		std::cout << "5 - 문자 랜덤 재배치\n";
		std::cout << "9 - 배열 초기화\n";
		std::cout << "0 - 종료\n";

		char cmd = _getch( );

		if ( cmd >= '1' && cmd <= '4' ) {
			MoveCursor( cmd - '0' );
		}
		else if ( cmd == '\r' ) {
			SelectLetter( );
		}
		else if ( cmd == '5' ) {
			ShuffleGrid( );
		}
		else if ( cmd == '9' ) {
			InitGrid( );
			cursorX = 0;
			cursorY = 0;
			active = false;
		}
		else if ( cmd == 'h' ) {
			open = !open;
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
			else if ( grid[ y ][ x ] == '@' ) {
				std::cout << "\033[33m" << grid[ y ][ x ] << "\033[0m ";
			}
			else {
				std::cout << grid[ y ][ x ] << ' ';
			}
		}
		std::cout << '\n';
	}
	if ( open ) {
		std::cout << "맞춰야 할 단어 : " << currWord << '\n';
	}
	if ( active ) {
		std::cout << "Word: " << dispWord << '\n';
	}
}

void MoveCursor( int dir ) {
	switch ( dir ) {
	case 1:
		--cursorX;
		if ( cursorX < 0 ) {
			cursorX = col - 1;
		}
		break;

	case 2:
		++cursorX;
		if ( cursorX >= col ) {
			cursorX = 0;
		}
		break;

	case 3:
		--cursorY;
		if ( cursorY < 0 ) {
			cursorY = row - 1;
		}
		break;

	case 4:
		++cursorY;
		if ( cursorY >= row ) {
			cursorY = 0;
		}
		break;

	default:
		break;
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

void SelectLetter( ) {
	if ( active && IsCorrect( cursor ) ) {
		for ( auto i = 0; i < currWord.size( ); ++i ) {
			if ( currWord[ i ] == cursor && dispWord[ i ] == '*' ) {
				dispWord[ i ] = cursor;
				break;
			}
		}

		grid[ cursorY ][ cursorX ] = 'O';
		fixed[ cursorY ][ cursorX ] = true;

		if ( dispWord == currWord ) {
			active = false;
			correct = true;
		}
	}
	else {
		std::cout << '\a';
		grid[ cursorY ][ cursorX ] = '@';
		fixed[ cursorY ][ cursorX ] = true;
	}
}

void SelectWord( int idx ) {
	currWord = words[ idx ];
	dispWord.clear( );
	dispWord.resize( currWord.size( ), '*' );
	active = true;
	PrintGrid( );
}

bool IsCorrect( char letter ) {
	for ( auto c : currWord ) {
		if ( c == letter ) {
			return true;
		}
	}
	return false;
}
