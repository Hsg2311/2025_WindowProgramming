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
		std::cout << "�̸�: " << name_ << ", �й�: " << id_ << ", �г�: " << grade_;
		std::cout << ", ����: " << birth_ << ", �⼮ ��Ȳ: " << month_ << "�� " << count_ << "ȸ\n";
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
	studentData.push_back( Student( "�����", 100, 4, "0420", 3, 3 ) );
	studentData.push_back( Student( "����", 101, 4, "1209", 3, 4 ) );
	studentData.push_back( Student( "������", 102, 4, "0530", 3, 5 ) );
	studentData.push_back( Student( "ȫ�浿", 103, 1, "0101", 3, 6 ) );

	while ( true ) {
		std::cout << "a - �л� ���� �߰�\n";
		std::cout << "p - ��� �л� ���� ���\n";
		std::cout << "1/2/3/4 - �̸�/�й�/�г�/���� ������ �������� ����\n";
		std::cout << "m �̸� - ���� ����\n";
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