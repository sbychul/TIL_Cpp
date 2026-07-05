#include <iostream>
#include <windows.h> 
#include <string>
#include <memory> // 스마트 포인터 헤더

// [문제] 스마트 포인터를 활용한 무기(Weapon) 관리 시스템
// 동적으로 생성될 Weapon 클래스를 설계.
class Weapon {
private: std::string weapon_name;
public:
	// 생성자, 이름을 받아 초기화.
	Weapon(std::string name) : weapon_name{ name } { std::cout << "무기 [" << weapon_name << "] 생성 완료\n"; }
	// 소멸자, 무기가 파괴되었음을 명시. (메모리 초기화는 알아서 스마트 포인터가 해줄 것이기에 따로 적지 않는다.)
	~Weapon() { std::cout << "무기 [" << weapon_name << "] 파괴 및 메모리 자동 반환\n";  }
	// 기능 함수 (공격)
	void attack() { std::cout << "[" << weapon_name << "]으로 공격합니다!\n"; }
};

// 무기 소유권을 완전히 넘겨받아 사용(공격)하는 독립 함수.
// std::unique_ptr은 특정 힙 메모리 자원을 오직 자기 혼자서만 독점적으로 소유하는 스마트 포인터.
// 특징 1: 복사 금지. 독점해야 하므로, 복사는 문법적으로 철저히 금지. (복사 생성자, 복사 대입 연산자 X)
// 특징 2: 이동 가능. std::move를 통해 소유권을 다른 unique_ptr로 이동하는 것은 가능. 어제 배운 이동 연산이 발동.
void equipWeapon(std::unique_ptr<Weapon> weapon) { weapon->attack(); }

int main() {
	SetConsoleOutputCP(65001);
	// make_unique를 통해 안전하게 객체 생성. 스마트 포인터 dragonBlade에 해당 객체를 할당한다.
	// make_unique : 내부에서 알아서 new를 안전하게 처리하고, 미세한 메모리 누수 위험까지 완벽하게 차단해 준다.
	std::unique_ptr dragonBlade = std::make_unique<Weapon>("용검");

	// equipWeapon 함수에 dragonBlade를 전달, 독점 포인터이므로 std::move() 함수를 사용해 소유권을 완전히 넘겨주며 호출한다.
	equipWeapon(std::move(dragonBlade));

	// equipWeapon 함수가 종료되는 순간, 소유권을 넘겨받았던 매개변수 weapon이 스택에서 소멸하면서
	// (dragonBlade 변수가 갖고 있던)Weapon 객체가 자동으로 파괴되어야 한다.
	// 따라서 main함수가 끝나기 전, 여기서 확인하기 전 파괴 메시지가 출력되어야 정상이다.
	// 추가적으로 nullptr인지 확인하며 소멸자가 잘 작동했는지 확인한다.
	if (dragonBlade == nullptr) { std::cout << "장착 후 기존 무기 슬롯 비워짐" << std::endl; return 0; }

	// return 0을 안 만나면 문제가 생긴 거다...
	std::cout << "어? 이게 출력되면 안 되는데?" << std::endl;
	return 1;
}