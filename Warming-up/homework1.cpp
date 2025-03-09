#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

void printSentence( const std::vector<std::string>&, const std::vector<int>& );

int main( )
{
	auto sentence = std::string( );

	auto strs = std::vector<std::string>( );
	auto spaceCnt = std::vector<int>( );
	auto alphabetCnt = std::array<int, 26>( );

	bool isPressedN3 = false;
	bool isPressedN4 = false;

	std::cout << "문장 입력: ";
	std::getline( std::cin, sentence );

	auto copySentence = std::string( sentence );
	copySentence.erase( std::remove( copySentence.begin( ), copySentence.end( ), ' ' ), copySentence.end( ) );
	
	auto s = std::string( );
	for ( auto i = 0; i < sentence.size( ); ++i ) {
		if ( sentence[ i ] != ' ' ) {
			s.push_back( sentence[ i ] );
		}
		else {
			if ( !s.empty( ) ) {
				strs.push_back( s );
				s.clear( );
			}
		}

		if ( sentence[ i ] == '.' ) {
			strs.push_back( s );
			s.clear( );
		}
	}

	int count = 0;
	for ( auto i = 0; i < sentence.size( ); ++i ) {
		if ( sentence[ i ] == ' ' ) {
			++count;
		}
		else {
			if ( count != 0 ) {
				spaceCnt.push_back( count );
				count = 0;
			}
		}
	}

	// strs와 spaceCnt의 개수를 일대일대응 시키기 위해 0 추가
	// strs의 마지막 문자, 즉 '.'를 포함한 문자는 공백이 없기 때문에 spaceCnt에 0 추가
	spaceCnt.push_back( 0 );

	while ( true ) {
		std::cout << "0: 종료\n";
		std::cout << "1: 공백 줄이기\n";
		std::cout << "2: 공백 늘리기\n";
		std::cout << "3: 문장 오름차순 정렬\n";
		std::cout << "4: 문장 내림치순 정렬\n";
		std::cout << "5: 알파벳 개수 세기\n";
		std::cout << "a~z: 입력 문자 대소문자 변경\n";

		char cmd;
		std::cin >> cmd;

		if ( cmd == '0' ) {
			break;
		}

		// 문자열에 포함된 공백 하나씩 줄이기
		if ( cmd == '1' ) {
			for ( auto i = 0; i < strs.size( ); ++i ) {
				if ( spaceCnt[ i ] > 0 ) {
					--spaceCnt[ i ];
				}
			}
			printSentence( strs, spaceCnt );

			isPressedN3 = false;
			isPressedN4 = false;
		}

		// 공백 하나씩 늘리기
		else if ( cmd == '2' ) {
			for ( auto i = 0; i < strs.size( ); ++i ) {
				++spaceCnt[ i ];
			}
			printSentence( strs, spaceCnt );

			isPressedN3 = false;
			isPressedN4 = false;
		}

		//a~z 입력 받기
		else if ( cmd >= 'a' && cmd <= 'z' ) {
			// 입력 받은 문자 대소문자 변경
			for ( auto& s : strs ) {
				for ( auto& c : s ) {
					if ( c == cmd || c == std::toupper( cmd ) ) {
						c = std::isupper( c ) ? std::tolower( c ) : std::toupper( c );
					}
				}
			}

			copySentence.clear( );
			for ( auto& s : strs ) {
				copySentence += s;
			}

			printSentence( strs, spaceCnt );

			isPressedN3 = false;
			isPressedN4 = false;
		}
		
		// 오름차순 정렬
		else if ( cmd == '3' ) {
			if ( !isPressedN3 ) {
				std::sort( copySentence.begin( ), copySentence.end( ) );
				std::cout << copySentence << '\n';
				isPressedN3 = true;
				isPressedN4 = false;
			}
			else {
				printSentence( strs, spaceCnt );
				isPressedN3 = false;
			}
		}

		// 내림차순 정렬
		else if ( cmd == '4' ) {
			if ( !isPressedN4 ) {
				std::sort( copySentence.begin( ), copySentence.end( ), std::greater<char>( ) );
				std::cout << copySentence << '\n';
				isPressedN4 = true;
				isPressedN3 = false;
			}
			else {
				printSentence( strs, spaceCnt );
				isPressedN4 = false;
			}
		}

		// 알파벳 개수 세기
		else if ( cmd == '5' ) {
			for ( auto& s : strs ) {
				for ( auto c : s ) {
					if ( std::isalpha( c ) ) {
						++alphabetCnt[ std::tolower( c ) - 'a' ];
					}
				}
			}

			for ( auto i = 0; i < alphabetCnt.size( ); ++i ) {
				std::cout << static_cast<char>( i + 'a' ) << ": " << alphabetCnt[ i ] << '\n';
			}

			isPressedN3 = false;
			isPressedN4 = false;
		}
	}
}

void printSentence( const std::vector<std::string>& strs, const std::vector<int>& spaceCnt ) {
	for ( auto i = 0; i < strs.size( ); ++i ) {
		std::cout << strs[ i ];
		for ( auto j = 0; j < spaceCnt[ i ]; ++j ) {
			std::cout << " ";
		}
	}
	std::cout << '\n';
}