#include <iostream>
#include <random>
#include <vector>
#include <numeric>
#include <string>
#include <limits>
#include <algorithm>

std::random_device rd;
std::mt19937 gen( rd( ) );

int main( )
{
	int num;
	std::cout << "Enter the number (50~100): ";
	std::cin >> num;

	auto vec = std::vector<int>( num - 1 );
	std::iota( vec.begin( ), vec.end( ), 1 );
	std::shuffle( vec.begin( ), vec.end( ), gen );

	auto nums = std::vector<int>( vec.begin( ), vec.begin( ) + 30 );
	auto copyNums = std::vector<int>( nums );

	for ( auto n : nums ) {
		std::cout << n << ' ';
	}
	std::cout << '\n';

	bool isPressedA = false;
	bool isPressedD = false;

	auto str = std::string( );
	auto chars = std::vector<std::string>( );
	std::cin.ignore( std::numeric_limits<std::streamsize>::max( ), '\n' );
	while ( true ) {
		std::getline( std::cin, str );

		auto tmp = std::string( );
		for ( auto c : str ) {
			if ( c != ' ' ) {
				tmp += c;
			}
			else {
				chars.push_back( tmp );
				tmp.clear( );
			}
		}
		if ( !tmp.empty( ) ) {
			chars.push_back( tmp );
		}

		if ( chars.size( ) > 6 ) {
			std::cout << "숫자는 5개까지만 입력 가능합니다.\n";
			continue;
		}

		auto cmd = chars[ 0 ];

		// char to int
		auto operands = std::vector<int>( );
		operands.reserve( chars.size( ) - 1 );
		for ( auto i = 1; i < chars.size( ); ++i ) {
			operands.push_back( std::stoi( chars[ i ] ) - 1 );
		}

		if ( cmd == "+" ) {
			auto result = nums[ operands[ 0 ] ];
			for ( auto i = 1; i < operands.size( ); ++i ) {
				result += nums[ operands[ i ] ];
			}
			std::cout << result << '\n';

			isPressedA = false;
			isPressedD = false;
		}
		else if ( cmd == "-" ) {
			auto result = nums[ operands[ 0 ] ];
			for ( auto i = 1; i < operands.size( ); ++i ) {
				result -= nums[ operands[ i ] ];
			}
			std::cout << result << '\n';

			isPressedA = false;
			isPressedD = false;
		}
		else if ( cmd == "a" ) {
			if ( !isPressedA ) {
				std::sort( copyNums.begin( ), copyNums.end( ) );
				for ( auto n : copyNums ) {
					std::cout << n << ' ';
				}
				std::cout << '\n';

				isPressedA = true;
				isPressedD = false;
			}
			else {
				for ( auto n : nums ) {
					std::cout << n << ' ';
				}
				std::cout << '\n';

				isPressedA = false;
			}
		}
		else if ( cmd == "d" ) {
			if ( !isPressedD ) {
				std::sort( copyNums.begin( ), copyNums.end( ), std::greater<int>( ) );
				for ( auto n : copyNums ) {
					std::cout << n << ' ';
				}
				std::cout << '\n';

				isPressedD = true;
				isPressedA = false;
			}
			else {
				for ( auto n : nums ) {
					std::cout << n << ' ';
				}
				std::cout << '\n';

				isPressedD = false;
			}
		}
		else if ( cmd == "m" ) {
			std::cout << *std::max_element( nums.begin( ), nums.end( ) ) << '\n';

			isPressedA = false;
			isPressedD = false;
		}
		else if ( cmd == "n" ) {
			std::cout << *std::min_element( nums.begin( ), nums.end( ) ) << '\n';

			isPressedA = false;
			isPressedD = false;
		}
		else if ( cmd == "r" ) {
			if ( isPressedA ) {
				std::sort( copyNums.begin( ), copyNums.end( ) );
				for ( auto n : copyNums ) {
					if ( n % ( operands[ 0 ] + 1 ) == 0 ) {
						std::cout << n << ' ';
					}
				}
			}
			else if ( isPressedD ) {
				std::sort( copyNums.begin( ), copyNums.end( ), std::greater<int>( ) );
				for ( auto n : copyNums ) {
					if ( n % ( operands[ 0 ] + 1 ) == 0 ) {
						std::cout << n << ' ';
					}
				}
			}
			else {
				for ( auto n : nums ) {
					if ( n % ( operands[ 0 ] + 1 ) == 0 ) {
						std::cout << n << ' ';
					}
				}
			}
			std::cout << '\n';
		}
		else if ( cmd == "s" ) {
			std::cout << "Enter the number (50~100): ";
			std::cin >> num;

			vec.clear( );
			vec.resize( num - 1 );
			std::iota( vec.begin( ), vec.end( ), 1 );
			std::shuffle( vec.begin( ), vec.end( ), gen );

			nums.clear( );
			std::copy( vec.begin( ), vec.begin( ) + 30, std::back_inserter( nums ) );
			copyNums.clear( );
			std::copy( nums.begin( ), nums.end( ), std::back_inserter( copyNums ) );

			for ( const auto& n : nums ) {
				std::cout << n << ' ';
			}
			std::cout << '\n';

			std::cin.ignore( std::numeric_limits<std::streamsize>::max( ), '\n' );
			isPressedA = false;
			isPressedD = false;
		}
		else if ( cmd == "q" ) {
			break;
		}
		else {
			std::cout << "잘못된 명령어입니다.\n";
		}

		str.clear( );
		chars.clear( );
	}
}
