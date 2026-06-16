#include <iostream> // iostream 헤더 파일에 정의된 객체를 이용하여 입출력 수행.
#include <clocale> // 로케일 헤더 추가, 콘솔(프로그램)의 출력 형식을 변경.

int main() {
	std::setlocale(LC_ALL, ""); // 프로그램의 출력을 시스템 기본 환경으로 설정.
	int num1{ 0 }, num2{ 0 }; // C++의 변수 초기화 방법 - 유니폼 초기화

	std::cout << "첫 번째 정수를 입력하세요: ";	// cout << 글 - 글을 콘솔에 출력하기
	std::cin >> num1;							// cin >> 변수 - 변수에 값 콘솔로 입력받기
	std::cout << "두 번째 정수를 입력하세요: ";
	std::cin >> num2;

	std::cout << "덧셈 결과: " << num1 + num2 << std::endl;

}