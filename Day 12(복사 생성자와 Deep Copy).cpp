#include <iostream>
#include <windows.h> 

// [문제] 깊은 복사가 구현된 데이터 박스 클래스
// 정수 하나를 동적으로 관리하는 DataBox 클래스를 설계하세요.
// 멤버 변수(private) : 정수형 포인터(int* ptr)
// 생성자(public) : 정수 값을 인자로 받아 힙 영역에 new int{ value }로 동적 할당합니다.
// Getter / Setter(public) : 
// int get() : 포인터가 가리키는 값을 반환합니다.
// void set(int value) : 포인터가 가리키는 공간에 새로운 값을 저장합니다.
// 복사 생성자(public) : 원본 DataBox 객체를 받아 깊은 복사를 수행하도록 구현하세요. (내부에서 "복사 생성자가 호출되었습니다."를 출력)
// 소멸자(public) : 할당된 포인터 메모리를 delete로 안전하게 해제합니다. (내부에서 "메모리 해제 완료"를 출력)

class DataBox {
private:
	int* ptr;
public:
	// 기본 생성자
	DataBox(int value) { ptr = new int{ value }; }

	// 복사 생성자 (Deep Copy를 구현)
	// 매개변수는 반드시 원본 수정을 막기 위해 const 여야 하고,
	// 무한 루프 복사를 방지하기 위한 &(레퍼런스) 형태여야 한다.
	DataBox(const DataBox& other) {
		// 원본과 동일한 크기의 새로운 메모리를 할당
		ptr = new int;
		// 내용물 복사 진행
		ptr[0] = other.ptr[0];
		std::cout << "복사 생성자가 호출되었습니다.\n";
	}

	// 소멸자
	~DataBox() {
		delete ptr;
		std::cout << "메모리 해제 완료\n";
	}

	// Getter/Setter
	int get() { return *ptr; }
	void set(int value) { 
		if (ptr == nullptr) { std::cout << "메모리 공간이 할당되어 있지 않습니다.\n"; return; }
		*ptr = value;
	}
};

// main 함수에서 초기값 10을 가진 box1 객체를 생성합니다.
// DataBox box2 = box1; (또는 DataBox box2{ box1 };) 문법을 사용해 box1을 복사한 box2를 생성합니다.
// box2.set(20); 을 통해 복사본의 값만 수정합니다.
// box1.get()과 box2.get()을 각각 출력하여, 깊은 복사가 잘 이루어져 box1의 값이 10으로 안전하게 유지되는지 확인하세요.

int main() {
	SetConsoleOutputCP(65001);
	DataBox box1{ 10 };
	DataBox box2{ box1 }; // 복사 생성자 호출
	box2.set(20); // Setter를 이용하여 box2의 값만 수정

	std::cout << "box1의 값: " << box1.get() << "\n";
	std::cout << "box2의 값: " << box2.get() << "\n";
}