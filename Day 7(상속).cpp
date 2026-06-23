#include <iostream>
#include <windows.h> 
#include <string> 

// [문제] 캐릭터 상속 및 특수 공격 시스템 구현하기
// 1. 기본형 클래스인 Character 클래스를 설계하세요.
// 2. Character를 상속받는 Warrior(전사) 클래스를 설계하세요.
// 3. main 함수 외부 혹은 내부에 부모의 참조자를 매개변수로 받는 독립 함수 void executeAttack(Character& ch)를 작성하세요.
// 4. main 함수에서 유저로부터 캐릭터 이름을 입력받아 Warrior 객체를 생성(유니폼 초기화 사용)하고,
// executeAttack 함수에 전달하여 오버라이딩된 공격이 정상 출력되는지 확인하세요.

class Character { 
protected: std::string name; // 이름
public:
	// 생성자: 입력받은 이름으로 초기화
	Character(std::string name_input) : name{ name_input } {} 

	// 가상 함수를 나타내는 virtual 키워드.
	// 부모 클래스의 함수를 자식 클래스에서 재정의하였을 때 오버라이딩이 제대로 작동하게끔 하기 위한 키워드.
	// 가상 함수 attack()을 정의. 자식 함수에서 오버라이드 진행 예정.
	virtual void attack() { std::cout << name << "이(가) 기본 공격을 합니다.\n"; }
};

// 상속받는 Warrior 클래스. class (자식 클래스) : public (부모 클래스) { ... }
// public 상속을 받으면 부모 클래스의 public, protected 멤버들이 자식 클래스에서도 그대로 유지.
// 그래서 이름을 protected 접근 지정자로 한 것.
class Warrior : public Character {
public:
	// 생성자: 부모 클래스의 생성자를 호출하여 입력받은 이름으로 초기화.
	Warrior(std::string w_name) : Character{ w_name } {}
	// 기본 공격 함수 오버라이드.
	void attack() override { std::cout << name << "이(가) 칼날 휘두르기를 사용합니다.\n"; }
};

// 부모 클래스의 참조자를 매개변수로 받아 공격을 수행하는 함수.
// 레퍼런스를 받는 이유: 하나의 함수로 수많은 자식 클래스들의 attack()을 실행하기 위함. 어떤 자식 객체든 다 받아줄 수 있음.
// 부모 클래스의 attack()이 가상 함수기 때문에, 자식 클래스가 오버라이드했다면 그게 최우선적으로 실행.
void executeAttack(Character& ch) {
	ch.attack();
}

int main() {
	SetConsoleOutputCP(65001);
	std::string input{};

	std::cout << "캐릭터의 이름을 입력하세요: ";
	std::getline(std::cin, input); // 이름이 한 줄일 수 있어서

	// Warrior 객체 생성 후 공격 함수 실행.
	Warrior my_warrior{ input };
	executeAttack(my_warrior);
}