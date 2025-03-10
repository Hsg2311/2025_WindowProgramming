#include <iostream>
#include <string>
#include <vector>

class Student {
public:
	Student( ) = default;
	Student( const std::string& name, int id, int grade, const std::string& birth, int month, int count )
		: name_( name ), id_( id ), grade_( grade ), birth_( birth ), month_( month ), count_( count ) { }

	~Student( ) = default;

public:
	void print( ) const {
		std::cout << "이름: " << name_ << ", 학번: " << id_ << ", 학년: " << grade_;
		std::cout << ", 생일: " << birth_ << ", 출석 현황: " << month_ << "월 " << count_ << "회\n";
	}

	bool exist( const std::string& name ) const {
		return name_ == name;
	}

	void modifyId( int newId ) {
		id_ = newId;
	}

private:
	std::string name_;
	int id_;
	int grade_;
	std::string birth_;
	int month_;
	int count_;
};

auto studentData = std::vector<Student>( );

int main( )
{
	studentData.push_back( Student( "우상훈", 100, 4, "0420", 3, 3 ) );
	studentData.push_back( Student( "장명운", 101, 4, "1209", 3, 4 ) );
	studentData.push_back( Student( "이종진", 102, 4, "0530", 3, 5 ) );
	studentData.push_back( Student( "홍길동", 103, 1, "0101", 3, 6 ) );

	while ( true ) {
		std::cout << "a - 학생 정보 추가\n";
		std::cout << "p - 모든 학생 정보 출력\n";
		std::cout << "1/2/3/4 - 이름/학번/학년/생일 순으로 오름차순 정렬\n";
		std::cout << "m 이름 - 정보 수정\n";
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

		}
		else if ( cmd == "p" ) {
			for ( const auto& student : studentData ) {
				student.print( );
			}
		}
		else if ( cmd == "d" ) {
			auto name = strs[ 1 ];
			for ( auto it = studentData.begin( ); it != studentData.end( ); ) {
				if ( it->exist( name ) ) {
					it = studentData.erase( it );
				}
				else {
					++it;
				}
			}
		}
		else if ( cmd == "s" ) {
			auto name = strs[ 1 ];
			for ( const auto& student : studentData ) {
				if ( student.exist( name ) ) {
					student.print( );
				}
			}
		}
		else if ( cmd == "7" ) {
			auto name = strs[ 1 ];
			auto newId = std::stoi( strs[ 2 ] );
			for ( auto& student : studentData ) {
				if ( student.exist( name ) ) {
					student.modifyId( newId );
				}
			}
		}
		else if ( cmd == "q" ) {
			break;
		}

		std::cout << '\n';
	}
}