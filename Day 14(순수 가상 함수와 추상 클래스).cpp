#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 영웅 스킬 시스템 뼈대 설계하기
// 추상 클래스인 HeroBase 클래스를 설계하세요.
class HeroBase {
protected:
	// 멤버 변수(protected) : 영웅 이름(std::string name)
	std::string name;
public:
	// 생성자(public) : 이름을 받아 초기화.
	HeroBase(std::string name_input) : name{ name_input } {}

	// 순수 가상 함수. 본문을 아예 작성하지 않고, 상속받는 클래스가 반드시 오버라이드해야 하는 함수.
	// 순수 가상 함수가 하나라도 있는 클래스는 추상 클래스가 되어 단독으로 객체를 생성할 수 없음.
	// 그러나, 추상 클래스라고 해서 모든 함수가 순수 가상 함수일 필요는 없음. (멤버 변수도 가질 수 있고, 일반 함수도 가질 수 있고, 생성자도 있음)
	// 만약 자식 클래스가 오버라이드하지 않는다면 자식 클래스도 추상 클래스로 취급한다.
	virtual void useUltimate() = 0; // 궁극기 사용 함수.
};

// HeroBase를 상속받는 Genji 클래스를 설계하세요.
class Genji : public HeroBase {
public:
	// 생성자에서 부모 생성자를 호출해 이름을 "Genji"로 설정.
	Genji() : HeroBase{ "Genji" } {}
	// useUltimate()를 오버라이딩하여 "[이름]이(가) 용검을 뽑습니다!"를 출력.
	void useUltimate() override { std::cout << name << "이(가) 용검을 뽑습니다!\n"; }
};

// HeroBase를 상속받는 Tracer 클래스를 설계하세요.
class Tracer : public HeroBase {
public:
	// 생성자에서 부모 생성자를 호출해 이름을 "Tracer"로 설정.
	Tracer() : HeroBase{ "Tracer" } {}
	// useUltimate()를 오버라이딩하여 "[이름]이(가) 펄스 폭탄을 부착합니다!"를 출력.
	void useUltimate() override { std::cout << name << "이(가) 펄스 폭탄을 부착합니다!\n"; }
};

// main 함수 외부에 부모의 참조자를 매개변수로 받는 독립 함수 void activateSkill(HeroBase& hero)를 작성하세요.
// 부모 타입의 포인터나 참조자를 통해, 어떤 자식 객체가 들어오더라도 일관된 인터페이스(함수 호출)로 다룰 수 있음.
void activateSkill(HeroBase& hero) {
	hero.useUltimate();
}

// main 함수에서 Genji와 Tracer 객체를 각각 생성하고,
// activateSkill 함수에 차례로 전달하여 각 영웅의 궁극기가 정상적으로 다형성을 통해 출력되는지 확인하세요.
int main() {
	SetConsoleOutputCP(65001);
	// 각 객체 생성
	Genji genji{};
	Tracer tracer{};

	// 스킬 사용 함수에 전달.
	activateSkill(genji);
	activateSkill(tracer);
}