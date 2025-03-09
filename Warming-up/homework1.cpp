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

	std::cout << "���� �Է�: ";
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

	// strs�� spaceCnt�� ������ �ϴ��ϴ��� ��Ű�� ���� 0 �߰�
	// strs�� ������ ����, �� '.'�� ������ ���ڴ� ������ ���� ������ spaceCnt�� 0 �߰�
	spaceCnt.push_back( 0 );

	while ( true ) {
		std::cout << "0: ����\n";
		std::cout << "1: ���� ���̱�\n";
		std::cout << "2: ���� �ø���\n";
		std::cout << "3: ���� �������� ����\n";
		std::cout << "4: ���� ����ġ�� ����\n";
		std::cout << "5: ���ĺ� ���� ����\n";
		std::cout << "a~z: �Է� ���� ��ҹ��� ����\n";

		char cmd;
		std::cin >> cmd;

		if ( cmd == '0' ) {
			break;
		}

		// ���ڿ��� ���Ե� ���� �ϳ��� ���̱�
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

		// ���� �ϳ��� �ø���
		else if ( cmd == '2' ) {
			for ( auto i = 0; i < strs.size( ); ++i ) {
				++spaceCnt[ i ];
			}
			printSentence( strs, spaceCnt );

			isPressedN3 = false;
			isPressedN4 = false;
		}

		//a~z �Է� �ޱ�
		else if ( cmd >= 'a' && cmd <= 'z' ) {
			// �Է� ���� ���� ��ҹ��� ����
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
		
		// �������� ����
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

		// �������� ����
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

		// ���ĺ� ���� ����
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