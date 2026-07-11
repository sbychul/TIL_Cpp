#include <iostream>
#include <windows.h> 
#include <string>

// 자바나 파이썬의 클래스 변수처럼
// C++도 static 키워드를 붙이면 해당 클래스로 생성된 모든 객체가 하나의 메모리 공간을 공유한다.

// [문제] F1 피트 스톱(Pit Stop) 차량 관리 및 통합 타이어 상태 검사 시스템
class F1Car {
private:
	std::string driver_name; // 드라이버명
	int tyre_wear; // 타이어 내구도

	// 피트 레인에 있는 차량 수(static). 모든 F1Car 클래스 객체가 이를 공유한다.
	// 클래스 내부에선 선언만 하고, 클래스 외부에서 반드시 초기화를 해주어야 한다.
	static int total_cars_in_pit; 
	
	// 클래스 내부에서 SecurityInspection 함수를 향해 friend 선언.
	// 이 함수는 F1Car 클래스의 private 멤버를 Getter/Setter 없이 일반 public 멤버처럼 자유롭게 접근할 수 있다.
	// 구현은 클래스 외부에서 진행한다.
	// 연산자 오버로딩 중 출력 스트림 연산자를 구현할 때 대표적으로 사용한다.
	friend void tyreInfoInspection(const F1Car& car);
public:
	// 생성자, 이름을 받아 초기화. 타이어 내구도의 디폴트 매개변수(기본값)은 100.
	F1Car(std::string name, int wear = 100) : driver_name{ name }, tyre_wear{ wear } {
		total_cars_in_pit++; // 객체 생성이 피트 레인 진입인 컨셉이다.
		std::cout << "[피트 인] " << name << "의 차량이 피트 레인에 진입합니다. (현재 피트 인 중인 차량: " << total_cars_in_pit << "대)\n";
	}
	// 소멸자, 피트 아웃
	~F1Car() {
		total_cars_in_pit--;
		std::cout << "[피트 아웃] " << driver_name << "의 차량이 트랙으로 복귀합니다.\n";
	}
	// 정적 멤버 함수. 객체 생성 없이 F1Car::getTotalCars() 로 접근할 수 있다.
	static int getTotalCars() { return total_cars_in_pit; }
};

// friend 함수 구현부.
// 완전한 외부 독립 함수임에도 private 멤버에 접근할 수 있다.
void tyreInfoInspection(const F1Car& car) {
	if (car.tyre_wear <= 40) { // 타이어 내구도 멤버에 접근, 40 이하라면
		std::cout << "[알림] " << car.driver_name << "의 타이어 상태가 좋지 않습니다. 타이어 교체가 필요합니다. (현재 내구도: " << car.tyre_wear << ")\n";
		return;
	}
	std::cout << "[알림] " << car.driver_name << "의 타이어 상태가 양호합니다.\n";
}

// 전역 공간에서 정적 변수 초기화.
// 외부 초기화 문법은 private이고 뭐고 통과다.
// 클래스 내부에 작성하는 것은 해당 변수명이 존재할 것이라는 선언에 불과하기 때문에, 외부에서 초기화해 주어야 한다.
int F1Car::total_cars_in_pit{ 0 };

int main() {
	SetConsoleOutputCP(65001);
	{
		// 드라이버 넘버를 넣어보니 어째 분기에 딱 맞길래...
		F1Car hamilton44{ "Lewis Hamilton", 44 };
		F1Car verstappen3{ "Max Verstappen", 3 };

		// 타이어 상태 확인 함수 사용
		tyreInfoInspection(hamilton44);
		tyreInfoInspection(verstappen3);

		// 정적 함수 호출
		std::cout << "현재 피트인 차량 수: " << F1Car::getTotalCars() << "\n";
	} // 중괄호 블록 종료, 객체가 소멸한다.

	std::cout << "[중괄호 블록 종료] 현재 피트인 차량 수: " << F1Car::getTotalCars() << "\n";
}