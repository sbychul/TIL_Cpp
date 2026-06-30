#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 안전한 메모리 덮어쓰기가 가능한 스마트 인트 클래스
// 정수 하나를 동적으로 관리하는 SmartInt 클래스를 설계하세요.

class SmartInt {
private:
	// 멤버 변수(private) : 정수형 포인터(int* val_ptr)
	int* val_ptr;
public:
	// 생성자(public) : 정수를 입력받아 힙 영역에 단일 객체(new int{ value })로 할당.
	SmartInt(int value) { val_ptr = new int{ value }; }
	// Getter(public) : int get()->포인터가 가리키는 값을 반환.
	int get() { return *val_ptr; }

	// 대입 연산자 오버로딩(public) : SmartInt & operator=(const SmartInt& other)
	// 이미 생성되어 있는 두 객체 사이 값이 대입될 때 호출 (예: i1 = i2;)
	SmartInt& operator=(const SmartInt& other) {
		// 함수 내부 진입 시 "대입 연산자 호출됨"을 출력.
		std::cout << "대입 연산자 호출됨." << std::endl;

		// 자기 대입 검사.
		// 이게 없다면 자기 자신의 메모리를 지운 후 이미 지워진 메모리를 참조하려 함. (멸망)
		if (this == &other) { return *this; }

		// 기존 갖고 있던 값의 메모리 할당 해제.
		delete val_ptr;

		// 복사 대상의 데이터를 Deep Copy.
		val_ptr = new int{ *other.val_ptr };

		// 연속 대입이 가능하도록 자기 자신의 참조자를 반환.
		return *this;
	}

	// 소멸자
	~SmartInt() {
		if (val_ptr == nullptr) { return; }
		delete val_ptr;
	}
};


int main() {
	SetConsoleOutputCP(65001);
	// 각각의 객체 생성
	SmartInt s1{ 10 };
	SmartInt s2{ 20 };

	std::cout << "[대입 연산자 호출 이전]\ns1의 값: " << s1.get() << "\n";
	std::cout << "s2의 값: " << s2.get() << "\n";

	std::cout << "\n[대입 연산자 호출 이후]\n";
	// 대입 연산자 실행
	s2 = s1;

	// 복사가 잘 진행되었는지 확인
	std::cout << "s1의 값: " << s1.get() << "\n";
	std::cout << "s2의 값: " << s2.get() << "\n";

	std::cout << "\n[자기 대입 실행 이후 보호 여부 확인]\n";
	// 대입 연산자, 자기 대입 실행. 값이 10으로 유지되어야 함.
	s1 = s1;
	std::cout << "s1의 값: " << s1.get() << "\n";
}