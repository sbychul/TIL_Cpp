#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 범용 데이터 배열 최대값 검색기

// 템플릿 함수 정의. T를 하나의 임의의 타입으로 사용.
// 임의의 타입 배열과 크기를 받아 최대값을 반환하는 템플릿 함수.
template <typename T>
T findmax(const T arr[], int size) {
	if (size <= 0) { return T{}; } // 타입별 기본 초기화 값을 반환
	T max_val = arr[0]; // 배열의 첫 요소를 초기값으로 설정

	for (int i = 1; i < size; i++) { // 순회하며 비교
		if (arr[i] > max_val) { max_val = arr[i]; }
	}

	return max_val;
}

int main() {
	SetConsoleOutputCP(65001);
	// 타입이 다른 두 개의 배열을 준비
	int int_arr[5] = { 3, 9, 5, 1, 7 };
	double double_arr[4] = { 2.2, 5.5, 1.1, 4.4 };

	int int_arr_max = findmax(int_arr, 5);
	double d_arr_max = findmax(double_arr, 4);
	int size_is_zero = findmax(int_arr, 0);

	std::cout << "정수 배열 최대값: " << int_arr_max << "\n";
	std::cout << "실수 배열 최대값: " << d_arr_max << "\n";
	std::cout << "예외 상황 (배열 크기 0) 반환 값: " << size_is_zero << std::endl;
}