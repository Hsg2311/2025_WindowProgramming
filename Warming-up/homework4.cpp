#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

// �˻� ���� �� ����� ���� ������

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
		std::cout << "�̸�: " << name_ << ", �й�: " << id_ << ", �г�: " << grade_;
		std::cout << ", ����: " << birth_ << ", �⼮ ��Ȳ: " << year_ << "�� " << month_ << "�� " << count_ << "ȸ\n";
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
	studentData.push_back( Student( "�����", "100", "4", "0420", "2020", "3", "3" ) );
	studentData.push_back( Student( "����", "101", "4", "1209", "2021", "3", "4" ) );
	studentData.push_back( Student( "������", "102", "4", "0530", "2022", "3", "5" ) );
	studentData.push_back( Student( "ȫ�浿", "103", "1", "0101", "2023", "3", "6" ) );
	auto copyData = std::vector<Student>( studentData );

	bool isPressed1 = false;
	bool isPressed2 = false;
	bool isPressed3 = false;
	bool isPressed4 = false;

	while ( true ) {
		std::cout << "a - �л� ���� �߰�(�Է�: �̸� �й� �г� ���� �⵵ �⼮�� �⼮Ƚ��)\n";
		std::cout << "p - ��� �л� ���� ���\n";
		std::cout << "1/2/3/4 - �̸�/�й�/�г�/���� ������ �������� ����\n";
		std::cout << "m �̸� - ���� ����(����: �й� �г� ���� �⼮�� �⼮Ƚ��)\n";
		std::cout << "d �̸� - ���� ����\n";
		std::cout << "s �̸� - �л� ���� �˻�\n";
		std::cout << "7 �̸� ���й� - �й� ����\n";
		std::cout << "8 �̸� �� ��Ƚ�� - �⼮ Ƚ�� ����\n";
		std::cout << "q - ����\n";

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
					std::cout << "�̸� Ȥ�� �й� �ߺ�\n";
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

			// �̸� / �й� �г� ���� �⼮�� �⼮Ƚ��
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
						std::cout << "�й� �ߺ�\n";
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
					std::cout << "�������� �ʴ� �̸��Դϴ�.\n";
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
					std::cout << "�������� �ʴ� �̸��Դϴ�.\n";
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
					std::cout << "�������� �ʴ� �̸��Դϴ�.\n";
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
						std::cout << "�й� �ߺ�\n";
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
				std::cout << "�й��� 100���� 500���� �����մϴ�.\n\n";
				continue;
			}

			for ( auto it = studentData.begin( ); ; ++it ) {
				if ( it == studentData.end( ) ) {
					std::cout << "�������� �ʴ� �̸��Դϴ�.\n";
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

			// �̸� �� / ��Ƚ��
			auto name = strs[ 1 ];
			auto month = std::stoi( strs[ 2 ] );
			auto count = std::stoi( strs[ 3 ] );

			for ( auto it = studentData.begin( ); ; ++it ) {
				if ( it == studentData.end( ) ) {
					std::cout << "�������� �ʴ� �̸��Դϴ�.\n\n";
					break;
				}
				if ( it->existName( name ) ) {
					if ( month < 1 || month > 12 ) {
						std::cout << "���� �ùٸ��� �ʽ��ϴ�.\n";
						break;
					}
					it->modifyMonth( strs[ 2 ] );

					if ( it->isLeapYear( ) ) {
						if ( count < 0 || count > leapMonthDays[ month - 1 ] ) {
							std::cout << "�⼮ Ƚ���� �ùٸ��� �ʽ��ϴ�.\n";
							break;
						}
					}
					else {
						if ( count < 0 || count > monthDays[ month - 1 ] ) {
							std::cout << "�⼮ Ƚ���� �ùٸ��� �ʽ��ϴ�.\n";
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
		std::cout << "�й��� 100���� 500���� �����մϴ�.\n";
		return true;
	}
	if ( grade < 1 || grade > 4 ) {
		std::cout << "�г��� 1���� 4���� �����մϴ�.\n";
		return true;
	}
	if ( bMonth < 1 || bMonth > 12 || bDay < 1 || bDay > leapMonthDays[ bMonth - 1 ] ) {
		std::cout << "������ �ùٸ��� �ʽ��ϴ�.\n";
		return true;
	}

	if ( year < 1 || year > 9999 ) {
		std::cout << "�⵵�� �ùٸ��� �ʽ��ϴ�.\n";
		return true;
	}

	if ( IsLeapYear( year ) ) {
		if ( aMonth < 1 || aMonth > 12 || aCount < 0 || aCount > leapMonthDays[ aMonth - 1 ] ) {
			std::cout << "�⼮ ��Ȳ�� �ùٸ��� �ʽ��ϴ�.\n";
			return true;
		}
	}
	else {
		if ( aMonth < 1 || aMonth > 12 || aCount < 0 || aCount > monthDays[ aMonth - 1 ] ) {
			std::cout << "�⼮ ��Ȳ�� �ùٸ��� �ʽ��ϴ�.\n";
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
