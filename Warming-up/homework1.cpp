#include <iostream>
#include <string>
#include <vector>
#include <array>

void printSentence( const std::vector<std::string>&, const std::vector<int>& );

int main( )
{
	auto sentence = std::string( );

	auto strs = std::vector<std::string>( );
	auto spaceCnt = std::vector<int>( );
	auto alphabetCnt = std::array<int, 26>( );

	std::cout << "���� �Է�: ";
	std::getline( std::cin, sentence );
	
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
		}

		// ���� �ϳ��� �ø���
		else if ( cmd == '2' ) {
			for ( auto i = 0; i < strs.size( ); ++i ) {
				++spaceCnt[ i ];
			}
			printSentence( strs, spaceCnt );
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

			printSentence( strs, spaceCnt );
		}
		
		else if ( cmd == '3' ) {

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