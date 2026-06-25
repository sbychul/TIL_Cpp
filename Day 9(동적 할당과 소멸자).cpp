#include <iostream>
#include <windows.h> 

// [문제] 동적 정수형 컨테이너 클래스 구현하기
// 1. 동적 배열을 관리하는 CustomArray 클래스를 설계하세요.
// 2. main 함수에서 크기가 3인 CustomArray 객체를 생성(지역 변수 형태로 생성)합니다.
// 3. set 함수를 이용해 인덱스 0, 1, 2에 각각 임의의 정수를 넣고 print()로 확인하세요.
// 4. main 함수가 끝날 때 소멸자가 자동으로 터지는지 콘솔 메시지를 확인하세요.

class CustomArray {
private:
	int* arr;
	int size;

public:
	// 생성자, 배열의 크기를 인자로 받아 동적 배열(new int[]) 할당.
	CustomArray(int size_input) : size{ size_input } {
		if (size <= 0) { std::cout << "배열 크기 입력이 잘못되었습니다."; return; }
		arr = new int[size];
		std::cout << size_input << "크기의 배열이 힙에 할당되었습니다.\n"; }

	// 해당 인덱스에 값을 저장하는 함수
	void set(int index, int value) { arr[index] = value; }

	// 모든 배열의 요소를 출력하는 함수
	void print() {
		for (int i = 0; i < this->size; i++) { std::cout << arr[i] << " "; }
		std::cout << std::endl;
	}

	// 소멸자, 클래스명 앞 물결표를 붙여 정의.
	~CustomArray() {
		delete[] arr; // delete - C의 free와 같음
		std::cout << "소멸자 호출: 동적 배열이 해제되었습니다." << std::endl;
	}
};

int main() {
	SetConsoleOutputCP(65001);
	CustomArray arr = CustomArray(3);
	arr.set(0, 1);
	arr.set(1, 3);
	arr.set(2, 5);
	arr.print();
}