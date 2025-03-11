#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

// 검사 받을 때 디버그 모드로 돌리기

auto monthDays = std::array<int, 12>{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
auto leapMonthDays = std::array<int, 12>{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
bool IsLeapYear( int year );
bool CheckException( int id, int grade, int bMonth, int bDay, int year, int aMonth, int aCount );

class Student {
public:
	Student( ) = default;
	
	Student( const std::string& name, const std::string& id, const std::string& grade,
		const std::string& birth, const std::string& year, const std::string& month, const std::string& count)
		: name_( name ), id_( id ), grade_( grade ), birth_( birth ), year_( year ), month_( month ), count_( count ) {}

	~Student( ) = default;

public:
	void print( ) const {
		std::cout << "이름: " << name_ << ", 학번: " << id_ << ", 학년: " << grade_;
		std::cout << ", 생일: " << birth_ << ", 출석 현황: " << year_ << "년 " << month_ << "월 " << count_ << "회\n";
	}

	bool existName( const std::string& name ) const {
		return name_ == name;
	}

	bool existId( const std::string& id ) const {
		return id_ == id;
	}

	void modifyId( const std::string& newId ) {
		id_ = newId;
	}

	void modifyMonth( const std::string& month ) {
		month_ = month;
	}

	void modifyCount( const std::string& count ) {
		count_ = count;
	}

	void modifyAll( const std::string& id, const std::string& grade, 
		const std::string& birth, const std::string& month, const std::string& count ) {
		id_ = id;
		grade_ = grade;
		birth_ = birth;
		month_ = month;
		count_ = count;
	}

	bool isLeapYear( ) const {
		auto year = std::stoi( year_ );
		return ( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0;
	}

	int getYear( ) const {
		return std::stoi( year_ );
	}

	const std::string& getName( ) const {
		return name_;
	}

	int getId( ) const {
		return std::stoi( id_ );
	}

	int getGrade( ) const {
		return std::stoi( grade_ );
	}

	int getBirth( ) const {
		return std::stoi( birth_ );
	}

private:
	std::string name_;
	std::string id_;
	std::string grade_;
	std::string birth_;
	std::string year_;
	std::string month_;
	std::string count_;
};

auto studentData = std::vector<Student>( );

bool CompareName( const Student& lhs, const Student& rhs );
bool CompareId( const Student& lhs, const Student& rhs );
bool CompareGrade( const Student& lhs, const Student& rhs );
bool CompareBirth( const Student& lhs, const Student& rhs );

int main( )
{
	studentData.push_back( Student( "우상훈", "100", "4", "0420", "2020", "3", "3" ) );
	studentData.push_back( Student( "장명운", "101", "4", "1209", "2021", "3", "4" ) );
	studentData.push_back( Student( "이종진", "102", "4", "0530", "2022", "3", "5" ) );
	studentData.push_back( Student( "홍길동", "103", "1", "0101", "2023", "3", "6" ) );
	auto copyData = std::vector<Student>( studentData );

	bool isPressed1 = false;
	bool isPressed2 = false;
	bool isPressed3 = false;
	bool isPressed4 = false;

	while ( true ) {
		std::cout << "a - 학생 정보 추가(입력: 이름 학번 학년 생일 년도 출석월 출석횟수)\n";
		std::cout << "p - 모든 학생 정보 출력\n";
		std::cout << "1/2/3/4 - 이름/학번/학년/생일 순으로 오름차순 정렬\n";
		std::cout << "m 이름 - 정보 수정(수정: 학번 학년 생일 출석월 출석횟수)\n";
		std::cout << "d 이름 - 정보 삭제\n";
		std::cout << "s 이름 - 학생 정보 검색\n";
		std::cout << "7 이름 새학번 - 학번 변경\n";
		std::cout << "8 이름 월 새횟수 - 출석 횟수 변경\n";
		std::cout << "q - 종료\n";

		auto sen = std::string( );
		std::getline( std::cin, sen );

		auto strs = std::vector<std::string>( );
		auto tmp = std::string( );
		for ( auto c : sen ) {
			if ( c != ' ' ) {
				tmp += c;
			}
			else {
				strs.push_back( tmp );
				tmp.clear( );
			}
		}
		if ( !tmp.empty( ) ) {
			strs.push_back( tmp );
		}

		auto cmd = strs[ 0 ];

		if ( cmd == "a" ) {
			auto id = std::stoi( strs[ 2 ] );
			auto grade = std::stoi( strs[ 3 ] );
			auto bMonth = std::stoi( strs[ 4 ].substr( 0, 2 ) );
			auto bDay = std::stoi( strs[ 4 ].substr( 2, 2 ) );
			auto year = std::stoi( strs[ 5 ] );
			auto aMonth = std::stoi( strs[ 6 ] );
			auto aCount = std::stoi( strs[ 7 ] );

			bool flag = false;
			
			if ( CheckException( id, grade, bMonth, bDay, year, aMonth, aCount ) ) {
				std::cout << '\n';
				continue;
			}

			for ( const auto& student : studentData ) {
				if ( student.existName( strs[ 1 ] ) || student.existId( strs[ 2 ] ) ) {
					std::cout << "이름 혹은 학번 중복\n";
					flag = true;
					break;
				}
			}

			if ( !flag ) {
				studentData.push_back( Student( strs[ 1 ], strs[ 2 ], strs[ 3 ], strs[ 4 ], strs[ 5 ], strs[ 6 ], strs[ 7 ] ) );
				copyData.push_back( Student( strs[ 1 ], strs[ 2 ], strs[ 3 ], strs[ 4 ], strs[ 5 ], strs[ 6 ], strs[ 7 ] ) );
				isPressed1 = false;
				isPressed2 = false;
				isPressed3 = false;
				isPressed4 = false;
			}
		}
		else if ( cmd == "p" ) {
			for ( const auto& student : studentData ) {
				student.print( );
			}
			isPressed1 = false;
			isPressed2 = false;
			isPressed3 = false;
			isPressed4 = false;
		}
		else if ( cmd == "1" ) {
			if ( !isPressed1 ) {
				std::sort( copyData.begin( ), copyData.end( ), CompareName );
				for ( const auto& student : copyData ) {
					student.print( );
				}
				isPressed1 = true;
				isPressed2 = false;
				isPressed3 = false;
				isPressed4 = false;
			}
			else {
				for ( const auto& student : studentData ) {
					student.print( );
				}
				isPressed1 = false;
			}
		}
		else if ( cmd == "2" ) {
			if ( !isPressed2 ) {
				std::sort( copyData.begin( ), copyData.end( ), CompareId );
				for ( const auto& student : copyData ) {
					student.print( );
				}
				isPressed1 = false;
				isPressed2 = true;
				isPressed3 = false;
				isPressed4 = false;
			}
			else {
				for ( const auto& student : studentData ) {
					student.print( );
				}
				isPressed2 = false;
			}
		}
		else if ( cmd == "3" ) {
			if ( !isPressed3 ) {
				std::sort( copyData.begin( ), copyData.end( ), CompareGrade );
				for ( const auto& student : copyData ) {
					student.print( );
				}
				isPressed1 = false;
				isPressed2 = false;
				isPressed3 = true;
				isPressed4 = false;
			}
			else {
				for ( const auto& student : studentData ) {
					student.print( );
				}
				isPressed3 = false;
			}
		}
		else if ( cmd == "4" ) {
			if ( !isPressed4 ) {
				std::sort( copyData.begin( ), copyData.end( ), CompareBirth );
				for ( const auto& student : copyData ) {
					student.print( );
				}
				isPressed1 = false;
				isPressed2 = false;
				isPressed3 = false;
				isPressed4 = true;
			}
			else {
				for ( const auto& student : studentData ) {
					student.print( );
				}
				isPressed4 = false;
			}
		}
		else if ( cmd == "m" ) {
			isPressed1 = false;
			isPressed2 = false;
			isPressed3 = false;
			isPressed4 = false;

			// 이름 / 학번 학년 생일 출석월 출석횟수
			auto id = std::stoi( strs[ 2 ] );
			auto grade = std::stoi( strs[ 3 ] );
			auto bMonth = std::stoi( strs[ 4 ].substr( 0, 2 ) );
			auto bDay = std::stoi( strs[ 4 ].substr( 2, 2 ) );
			auto aMonth = std::stoi( strs[ 5 ] );
			auto aCount = std::stoi( strs[ 6 ] );

			bool idFlag = false;
			for ( const auto& student : studentData ) {
				if ( student.existId( strs[ 2 ] ) ) {
					if ( student.getName( ) != strs[ 1 ] ) {
						std::cout << "학번 중복\n";
						idFlag = true;
						break;
					}
				}
			}

			if ( idFlag ) {
				std::cout << '\n';
				continue;
			}
			
			for ( auto it = studentData.begin( ); ; ++it ) {
				if ( it == studentData.end( ) ) {
					std::cout << "존재하지 않는 이름입니다.\n";
					break;
				}
				if ( it->existName( strs[ 1 ] ) ) {
					if ( !CheckException( id, grade, bMonth, bDay, it->getYear( ), aMonth, aCount ) ) {
						it->modifyAll( strs[ 2 ], strs[ 3 ], strs[ 4 ], strs[ 5 ], strs[ 6 ] );
						copyData.clear( );
						std::copy( studentData.begin( ), studentData.end( ), std::back_inserter( copyData ) );
						break;
					}
					else {
						break;
					}
				}
			}
		}
		else if ( cmd == "d" ) {
			isPressed1 = false;
			isPressed2 = false;
			isPressed3 = false;
			isPressed4 = false;

			for ( auto it = studentData.begin( ); ; ++it ) {
				if ( it == studentData.end( ) ) {
					std::cout << "존재하지 않는 이름입니다.\n";
					break;
				}
				if ( it->existName( strs[ 1 ] ) ) {
					it = studentData.erase( it );
					copyData.clear( );
					std::copy( studentData.begin( ), studentData.end( ), std::back_inserter( copyData ) );
					break;
				}
			}
		}
		else if ( cmd == "s" ) {
			isPressed1 = false;
			isPressed2 = false;
			isPressed3 = false;
			isPressed4 = false;

			for ( auto it = studentData.begin( ); ; ++it ) {
				if ( it == studentData.end( ) ) {
					std::cout << "존재하지 않는 이름입니다.\n";
					break;
				}
				if ( it->existName( strs[ 1 ] ) ) {
					it->print( );
					break;
				}
			}
		}
		else if ( cmd == "7" ) {
			isPressed1 = false;
			isPressed2 = false;
			isPressed3 = false;
			isPressed4 = false;

			bool idFlag = false;
			for ( const auto& student : studentData ) {
				if ( student.existId( strs[ 2 ] ) ) {
					if ( student.getName( ) != strs[ 1 ] ) {
						std::cout << "학번 중복\n";
						idFlag = true;
						break;
					}
				}
			}

			if ( idFlag ) {
				std::cout << '\n';
				continue;
			}

			auto id = std::stoi( strs[ 2 ] );
			if ( id < 100 || id > 500 ) {
				std::cout << "학번은 100부터 500까지 가능합니다.\n\n";
				continue;
			}

			for ( auto it = studentData.begin( ); ; ++it ) {
				if ( it == studentData.end( ) ) {
					std::cout << "존재하지 않는 이름입니다.\n";
					break;
				}
				if ( it->existName( strs[ 1 ] ) ) {
					it->modifyId( strs[ 2 ] );
					copyData.clear( );
					std::copy( studentData.begin( ), studentData.end( ), std::back_inserter( copyData ) );
					break;
				}
			}
		}
		else if ( cmd == "8" ) {
			isPressed1 = false;
			isPressed2 = false;
			isPressed3 = false;
			isPressed4 = false;

			// 이름 월 / 새횟수
			auto name = strs[ 1 ];
			auto month = std::stoi( strs[ 2 ] );
			auto count = std::stoi( strs[ 3 ] );

			for ( auto it = studentData.begin( ); ; ++it ) {
				if ( it == studentData.end( ) ) {
					std::cout << "존재하지 않는 이름입니다.\n\n";
					break;
				}
				if ( it->existName( name ) ) {
					if ( month < 1 || month > 12 ) {
						std::cout << "월이 올바르지 않습니다.\n";
						break;
					}
					it->modifyMonth( strs[ 2 ] );

					if ( it->isLeapYear( ) ) {
						if ( count < 0 || count > leapMonthDays[ month - 1 ] ) {
							std::cout << "출석 횟수가 올바르지 않습니다.\n";
							break;
						}
					}
					else {
						if ( count < 0 || count > monthDays[ month - 1 ] ) {
							std::cout << "출석 횟수가 올바르지 않습니다.\n";
							break;
						}
					}

					it->modifyCount( strs[ 3 ] );
					copyData.clear( );
					std::copy( studentData.begin( ), studentData.end( ), std::back_inserter( copyData ) );
					break;
				}
			}
		}
		else if ( cmd == "q" ) {
			break;
		}

		std::cout << '\n';
	}
}

bool IsLeapYear( int year ) {
	return ( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0;
}

bool CheckException( int id, int grade, int bMonth, int bDay, int year, int aMonth, int aCount ) {
	if ( id < 100 || id > 500 ) {
		std::cout << "학번은 100부터 500까지 가능합니다.\n";
		return true;
	}
	if ( grade < 1 || grade > 4 ) {
		std::cout << "학년은 1부터 4까지 가능합니다.\n";
		return true;
	}
	if ( bMonth < 1 || bMonth > 12 || bDay < 1 || bDay > leapMonthDays[ bMonth - 1 ] ) {
		std::cout << "생일이 올바르지 않습니다.\n";
		return true;
	}

	if ( year < 1 || year > 9999 ) {
		std::cout << "년도가 올바르지 않습니다.\n";
		return true;
	}

	if ( IsLeapYear( year ) ) {
		if ( aMonth < 1 || aMonth > 12 || aCount < 0 || aCount > leapMonthDays[ aMonth - 1 ] ) {
			std::cout << "출석 현황이 올바르지 않습니다.\n";
			return true;
		}
	}
	else {
		if ( aMonth < 1 || aMonth > 12 || aCount < 0 || aCount > monthDays[ aMonth - 1 ] ) {
			std::cout << "출석 현황이 올바르지 않습니다.\n";
			return true;
		}
	}

	return false;
}

bool CompareName( const Student& lhs, const Student& rhs ) {
	return lhs.getName( ) < rhs.getName( );
}

bool CompareId( const Student& lhs, const Student& rhs ) {
	return lhs.getId( ) < rhs.getId( );
}

bool CompareGrade( const Student& lhs, const Student& rhs ) {
	if ( lhs.getGrade( ) == rhs.getGrade( ) ) {
		return lhs.getId( ) < rhs.getId( );
	}
	return lhs.getGrade( ) < rhs.getGrade( );
}

bool CompareBirth( const Student& lhs, const Student& rhs ) {
	return lhs.getBirth( ) < rhs.getBirth( );
}
