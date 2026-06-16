#include <iostream> // iostream 헤더 파일에 정의된 객체를 이용하여 입출력 수행.
#include <windows.h> // 한글 출력이 안 돼서... 윈도우 헤더

int main() {
	SetConsoleOutputCP(65001); // 출력 코드 페이지를 UTF-8로 고정

	int num1{ 0 }, num2{ 0 }; // C++의 변수 초기화 방법 - 유니폼 초기화

	std::cout << "첫 번째 정수를 입력하세요: ";	// cout << 글 - 글을 콘솔에 출력하기
	std::cin >> num1;							// cin >> 변수 - 변수에 값 콘솔로 입력받기
	std::cout << "두 번째 정수를 입력하세요: ";
	std::cin >> num2;

	std::cout << "덧셈 결과: " << num1 + num2 << '\n';
	std::cout << "뺄셈 결과: " << num1 - num2 << '\n';
	std::cout << "나눗셈 결과: " << num1 / num2 << '\n'; // 단순 줄바꿈
	// std::endl - 출력 버퍼를 비우는 작업까지 수행, 대량의 데이터 출력 시 속도 저하 가능성 있음.
	std::cout << "곱셈 결과: " << num1 * num2 << std::endl; 
}