#include <boost\serialization\serialization.hpp>
#include <boost\serialization\string.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\archive\binary_iarchive.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class UserAccount
{
	private:
		string name;
		string id;
		string pw;
		int age;

		friend class boost::serialization::access; //직렬화해주는분께서 접근 할 수 있도록 friend선언
		template<class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& name;
				ar& id;
				ar& pw;
				ar& age;
			}
	public:
		UserAccount(string _name, string _id, string _pw, int _age)
		{
			name = _name;
			id = _id;
			pw = _pw;
			age = _age;
		}
		UserAccount(){}
		void Print(void)
		{
			cout<<"name:"<<name<<endl;
			cout<<"id:"<<id<<endl;
			cout<<"pw:"<<pw<<endl;
			cout<<"age:"<<age<<endl;
		}
};

int main(void)
{
	string name, id, pw;
	char pointer[64];
	int age;
	cout<<"input name:";
	cin>>name;
	cout<<"input id:";
	cin>>id;
	cout<<"input pw:";
	cin>>pw;
	cout<<"input age:";
	cin>>age;
	UserAccount account(name,id,pw,age);

	ofstream out; //쓰기 스트림 생성
	out.open("serialization.test",ios_base::binary); //바이너리 모드로 파일을 열었습니다.
	boost::archive::binary_oarchive out_archive(out); //연 스트림을 넘겨주어서 직렬화객체 초기화
	out_archive<<account; //쓰기
	out.close(); //닫기

	ifstream in; //읽기 스트림 생성
	UserAccount read_account; //받을 객체 생성
	in.open("serialization.test",ios_base::binary); //바이너리모드로 파일을 열었습니다.
	boost::archive::binary_iarchive in_archive(in); //연 스트림을 넘겨주어서 직렬화객체 초기화
	in_archive>>read_account; //읽기
	read_account.Print(); //닫기

	return 0;
}
