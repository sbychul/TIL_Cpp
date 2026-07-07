#include <iostream>
#include <windows.h> 
#include <string>
#include <memory>

// 순환 참조란?
// 각기 다른 객체가 서로를 shared_ptr로 참조하고 있다면
// 객체 하나를 지우려 해도 다른 객체가 이를 참조하고 있어 delete가 안 됨,
// 참조 계수가 0이 되지 않아 영원히 메모리에 남는 (메모리 누수) 현상이 발생하는 것.

// [문제] 순환 참조가 방지된 길드(Guild)와 길드원(Member) 시스템
// 전방 선언
class Guild;

// Member 클래스
class Member {
private:
	std::string name; // 이름
	std::weak_ptr<Guild> his_guild; // 소속 길드를 가리킬 weak_ptr
public:
	// 생성자, 이름 받아 초기화
	Member(std::string name_input) : name{ name_input } { std::cout << "길드원 [" << name << "] 생성\n"; }
	// 소멸자, 단순 메시지 출력
	~Member() { std::cout << "길드원 [" << name << "] 소멸\n";}
	// 길드 지정 함수와 이름 출력 함수는 일단 선언만 해 두기.
	// (전방 선언만으로는 컴파일러가 Guild 클래스의 내부를 모르기에 길드 클래스의 완전한 구현 이후 구현해야 함)
	void setGuild(std::shared_ptr<Guild> guild);
	void printGuildName();
};

// Guild 클래스
class Guild {
private:
	std::string	guild_name; // 이름
	std::shared_ptr<Member> leader; // 길드장을 가리킬 공유 포인터
public:
	// 생성자, 이름 받아 초기화
	Guild(std::string name) : guild_name{ name } { std::cout << "길드 [" << guild_name << "] 창설\n"; }
	// 소멸자, 단순 메시지 출력
	~Guild() { std::cout << "길드 [" << guild_name << "] 해체 및 메모리 반환\n"; }
	// Getter, 이름 반환 용도
	std::string getGuildName() { return guild_name; }
	// 길드장 지정 함수
	void setLeader(std::shared_ptr<Member> member) { leader = member; }
};

// 길드 지정 함수 구현부
void Member::setGuild(std::shared_ptr<Guild> guild) { his_guild = guild; }

// 길드명 출력 함수 구현부
void Member::printGuildName() {
	std::shared_ptr<Guild> temp = his_guild.lock();
	if (temp == nullptr) {
		// weak_ptr은 참조 계수를 증가시키지 않고, 알맹이 객체에 직접 접근하는 -> 연산자가 없다.
		// 따라서 자원을 사용하기 위해서는 lock() 함수를 사용해 임시 shared_ptr로 변환해서 안전한 지 검증 후 사용한다.
		std::shared_ptr<Guild> temp = his_guild.lock(); // 임시 shared_ptr을 획득
		if (temp == nullptr) { // 만약 임시 포인터에서 아무 것도 얻지 못했다면 (= 길드가 없다면)
			std::cout << "소속된 길드가 없거나 길드가 해체되었습니다.\n";
			return;
		}
		// 임시 shared_ptr이기 때문에 -> 연산자로 해당 Guild 객체의 Getter에 접근할 수 있다.
		std::cout << name << "의 소속 길드명: " << temp->getGuildName() << "\n";
	}
}

int main() {
	SetConsoleOutputCP(65001);
	std::shared_ptr<Guild> t1_ptr = std::make_shared<Guild>("T1"); // 길드 생성
	std::shared_ptr<Member> goat_ptr = std::make_shared<Member>("Faker"); // 멤버 객체 생성

	t1_ptr->setLeader(goat_ptr); // 길드(T1)은 길드장(Faker)를 shared_ptr로 소유
	goat_ptr->setGuild(t1_ptr); // 길드원(Faker)는 길드(T1)을 weak_ptr로 소유
	goat_ptr->printGuildName(); // 길드명 출력 함수 정상 작동 여부 확인
	
	// 프로그램이 종료되기 직전 길드 객체와 길드원 객체가 정상적으로 소멸되는지 소멸자의 출력을 확인하자.
	// 참조 계수가 0인 길드 객체(T1)가 먼저 소멸하고,
	// 길드원(Faker)을 참조하고 있던 길드 객체가 소멸하여 길드원 객체도 소멸한다.
}