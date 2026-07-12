#include <iostream>
#include <windows.h> 
#include <string>

// << : 값을 스트림에 밀어넣는 삽입 연산자라는 이름의 함수이다.
// std::cout은 사실 출력 스트림 클래스의 객체.
// std::cout << 10; 이라는 코드를 만나면, std::ostream::operator<<(std::cout, 10); 라고 컴파일러는 해석한다.

// 연산자 오버로딩 시에 왜 friend를 붙여야 할까?
// <<를 클래스의 멤버 함수로 만들면, 연산자 왼쪽에는 무조건 자기 자신이 와야 함.
// 호출할 때 (클래스) << std::cout; 라는 말도 안 되는 모양이 되어 버린다.
// 따라서 익숙한 모양으로 만드려면 연산자 왼쪽에 std::ostream이 오고 오른쪽에 (클래스)가 오는 외부 전역 함수로 선언해야 한다.
// 이 때, 외부 전역 함수는 private를 못 보니까, friend를 선언하는 거다.

// [문제] F1 드라이버 프로필 직렬화 및 스트림 출력 시스템
class F1Driver {
private:
	std::string driver_name; // 이름
	std::string team_name; // 소속 팀명
	int championship_titles; // 드챔 횟수
	// 삽입 연산자 오버로딩을 위해 friend 선언
	friend std::ostream& operator<<(std::ostream& os, const F1Driver& driver);
public:
	// 생성자
	F1Driver(std::string name, std::string team, int titles) : driver_name{ name }, team_name{ team }, championship_titles{ titles } {}
};

// 클래스 바깥에서 삽입 연산자 오버로딩 전역 함수 구현
std::ostream& operator<<(std::ostream& os, const F1Driver& driver) {
	os << "[" << driver.team_name << "] " << driver.driver_name << " (월드 챔피언 횟수: " << driver.championship_titles << "회)";
	return os; // 연속 출력을 위하여 반드시 os 참조자를 다시 반환해야 한다.
}

int main() {
	SetConsoleOutputCP(65001);
	// 드라이버 객체 생성
	F1Driver lewis{ "Lewis Hamilton", "Scuderia Ferrari HP", 7 };
	F1Driver max{ "Max Verstappen", "Oracle Red Bull Racing", 4 };

	// 자바에서 toString 오버라이드한 느낌...
	std::cout << lewis << "\n" << max << std::endl;
}