#include <iostream> // iostream 헤더 파일에 정의된 객체를 이용하여 입출력 수행.
#include <windows.h> // 한글 출력을 위한 윈도우 헤더

// [문제] 짝수 판별 및 누적 계산기
// 사용자로부터 0이 아닌 정수를 계속해서 입력받습니다.
// 입력받은 수가 짝수라면 "Even"을 출력합니다.
// 입력받은 수가 홀수라면 "Odd"를 출력합니다.
// 만약 사용자가 0을 입력하면 프로그램을 종료하고, 그동안 입력된 모든 짝수의 합을 출력합니다.

int main() {
	SetConsoleOutputCP(65001);
	int num{ 0 };
	int even_sum{ 0 };

	// 입력을 성공적으로 받는 동안 true, 입력이 끝나거나 잘못된 데이터가 들어오면 false가 됨.
	// = 잘못된 데이터를 변수에 넣을 때까지 무한 반복.
	while (std::cin >> num) {
		// 0 입력 시 반복 종료
		if (num == 0) { break; } 

		// 짝수라면 Even을 출력 및 총합 변수에 값을 더함.
		if (num % 2 == 0) {
			std::cout << "Even" << "\n";
			even_sum += num;
		} else { std::cout << "Odd" << "\n"; } // 홀수라면 Odd만 출력 후 다음 반복 진행
	}
	// 짝수 총합 출력부
	std::cout << "입력된 짝수의 총합: " << even_sum << std::endl;
}