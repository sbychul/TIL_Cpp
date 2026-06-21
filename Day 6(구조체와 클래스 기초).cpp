#include <iostream>
#include <windows.h> 
#include <string>

// C++의 구조체는 클래스처럼 함수도 내부에 포함할 수 있음.
// 구조체: 별도의 접근 제어 지시자 없다면 구성 요소들이 public.
// 클래스: 기본적으로 구성 요소들이 private.

// [문제] 성적 관리 학생 클래스 구현하기
// Student 클래스를 설계하세요.
// 멤버 변수(private) : 이름(std::string name), C++ 점수(int cpp_score)
// 생성자(public) : 이름과 C++ 점수를 인자로 받아 유니폼 초기화({})를 수행합니다.
// 멤버 함수(public) :
//	  void printInfo() : "[이름]님의 C++ 점수: [점수]점" 형태로 출력합니다.
//    bool isPass() : C++ 점수가 60점 이상이면 true, 미만이면 false를 반환합니다.
// main 함수에서 유저로부터 학생의 이름과 점수를 입력받아 Student 객체를 생성합니다.
// 생성된 객체의 printInfo()를 호출하여 정보를 출력하고, isPass()의 결과에 따라 "합격입니다!" 또는 "재시험 대상입니다."를 출력하세요.

class Student {
private: // private: 클래스 외부에서 직접 수정 불가.
	std::string name;
	int cpp_score;

public: // public: 외부 공개용 메소드
	// 생성자, 이름과 점수를 받아 유니폼 초기화 진행.
	// {} 내부는 추가적인 객체 생성 시 로직이 있다면 작성하는 본문.
	Student(std::string new_name, int new_score) : name{ new_name }, cpp_score{ new_score } {}

	void printInfo() { std::cout << name << "님의 C++ 점수: " << cpp_score << "점\n"; }
	bool isPass() { return cpp_score >= 60; }
};

int main() {
	SetConsoleOutputCP(65001);
	std::string name_input{};
	int score_input{ 0 };

	std::cout << "학생의 이름을 입력하세요: ";
	std::getline(std::cin, name_input);

	std::cout << "학생의 점수를 입력하세요: ";
	if (std::cin >> score_input) {}
	else { std::cout << "잘못된 입력입니다."; return 1; } // 예외 처리
	
	// 입력받은 값으로 객체 생성. 중괄호를 써서 객체 생성도 유니폼 초기화 방식으로 진행.
	Student new_student{ name_input, score_input }; 
	new_student.printInfo();

	if (new_student.isPass()) { std::cout << "합격입니다!"; }
	else { std::cout << "재시험 대상입니다."; }
}