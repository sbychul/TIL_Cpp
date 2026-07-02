#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 범용 듀얼 데이터 저장소(SimplePair) 클래스 설계
// 두 개의 동일한 타입 데이터를 저장하는 클래스 템플릿 SimplePair를 설계하세요.

// 클래스 템플릿: 클래스 내부에 등장하는 멤버 변수나 함수의 반환 타입을 고정하지 않고,
// 객체를 생성할 때 지정하는 타입으로 유연하게 결정되도록 만든다.
template <typename T>
class SimplePair {
private:
	// 멤버 변수, 객체를 생성할 때 타입을 결정.
	T first_val, second_val;
public:
	// 생성자: 두 개의 데이터를 인자로 받아 멤버 변수 초기화
	SimplePair(T first, T second) : first_val{ first }, second_val{ second } {}
	// Getter
	T getFirst() { return first_val; }
	T getSecond() { return second_val; }
	// 교환 함수
	void swapValues() {
		T temp{ first_val };
		first_val = second_val;
		second_val = temp;
	}
};

int main() {
	SetConsoleOutputCP(65001);
	// 템플릿 함수와 달리, 인자 값을 보고 추론하는 것이 아닌 <> 안에 어떤 타입으로 만들지 명시해 주어야 한다.
	SimplePair intpair = SimplePair<int>(10, 20); // int 타입으로 만든다는 것을 명시
	SimplePair strpair = SimplePair<std::string>("Seoul", "Busan"); // string 타입으로 만든다는 것을 명시

	std::cout << "[정수 페어 테스트]\n교환 전: " << intpair.getFirst() << ", " << intpair.getSecond();
	intpair.swapValues(); // 교환 함수 호출
	std::cout << "\n교환 후: " << intpair.getFirst() << ", " << intpair.getSecond() << std::endl;

	std::cout << "\n[문자열 페어 테스트]\n교환 전: " << strpair.getFirst() << ", " << strpair.getSecond();
	strpair.swapValues(); // 교환 함수 호출
	std::cout << "\n교환 후: " << strpair.getFirst() << ", " << strpair.getSecond() << std::endl;
}