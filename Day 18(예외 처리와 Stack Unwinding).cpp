#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 예외 안전성이 검증된 커스텀 배열 클래스
// 정수 배열을 안전하게 관리하는 SafeArray 클래스를 설계하세요.

class SafeArray {
private:
	int size; // 정수 배열 크기
	int* arr; // 동적 할당 정수 포인터
public:
	// 생성자: 크기 받아 힙 메모리 할당 후 메시지 출력
	SafeArray(int size_input) : size{ size_input } {
		arr = new int[size] {0};
		std::cout << "배열 생성됨\n";
	}

	// 소멸자: 동적 할당된 메모리를 해제
	~SafeArray() {
		delete[] arr;
		// 스택 언와인딩 검증용 메시지 출력부
		std::cout << "소멸자 호출: 메모리 해제 완료\n";
	}

	// 인덱스에 해당하는 값 set하는 함수
	void set(int index, int value) { 
		// 예외 발생 시 예외를 던져 준다. main 함수의 try-catch 블록에서 잡아낼 예정.
		if (index < 0 || index >= size) { throw std::out_of_range("배열의 인덱스 범위를 벗어났습니다!"); } 
		arr[index] = value; // 예외가 아니라면 배열 내 값 설정.
	}
};

// main 함수 외부에 독립 함수 void testArray()를 작성.
// 억지로 예외를 던져주는 함수이다. 과연 main 함수 내부에서 잘 받는지 확인하자.
void testArray() {
	SafeArray dummy{ 3 }; // 크기 3의 더미 배열 생성
	dummy.set(5, 99); // 억지로 예외 소환하기. 잘 출력되는지 확인하자.
}

int main() {
	SetConsoleOutputCP(65001);
	try { // C++의 예외 처리도 자바처럼 try블록 내에서 예외를 감지, throw로 예외를 던지며, catch 블록에서 이를 잡아낸다.
		testArray(); // 억지로 예외를 불러내는 함수
	}
	// 표준 예외 클래스 std::exception을 참조로 포착. 예외 발생 시 실행.
	// 어떤 예외가 던져질 지 모를 때, 진짜 모든 걸 다 잡는 catch문은:
	// catch (...) { ... } 이다. 대신 로그를 확인할 수 없기에(e.what() 불가) 최후의 보루로 사용한다.
	catch (const std::exception& e) { 
		std::cout << "예외 발생: " << e.what() << std::endl; // e.what()으로 설정한 에러 메시지 출력.

		// 스택 언와인딩은 그래서 뭐냐?
		// try 블록 내부에서 예외(throw)가 발생하면 catch 블록을 찾을 때까지 실행 중인 함수를 역순으로 빠져나간다.
		// 이 때, 스택 영역에 생성된 모든 지역 객체들의 소멸자가 자동으로 호출되면서 메모리가 정리된다.
		// 스택? 역순? 스택 되감기(Unwinding).
		// 
		// 따라서 출력 로그를 확인하였을 때:
		// 
		// 배열 생성됨
		// 소멸자 호출 : 메모리 안전하게 해제 완료
		// 예외 발생: 배열의 인덱스 범위를 벗어났습니다!
		// 
		// 형식으로 출력된다. catch문으로 가는 과정에서 스택 언와인딩이 발생, 소멸자 호출이 catch 블록의 코드보다 먼저 일어난다.
	}
}