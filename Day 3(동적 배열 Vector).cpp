#include <iostream>
#include <windows.h> 
#include <vector> // 벡터 사용을 위한 헤더
#include <limits> // 예외 처리를 위한 헤더

// [문제] 역순 정수 배열과 평균 이상 출력하기
// 사용자로부터 정수 N을 입력받습니다.
// 이어서 N개의 정수를 입력받아 std::vector에 순서대로 저장합니다.
// 입력이 끝나면, 입력된 순서의 역순(뒤에서부터)으로 요소를 한 줄에 하나씩 출력합니다.
// 마지막 줄에는 입력된 모든 정수의 평균값(실수형)을 출력하세요.

int main() {
	SetConsoleOutputCP(65001);
	std::vector<int> v; // 비어 있는 정수형 벡터 선언
	int n{ 0 }, num_input{ 0 }, sum{ 0 };
	double avg{ 0 };

	std::cout << "입력할 정수의 개수를 입력하세요: ";
	std::cin >> n; // 정수 N 입력받음
	
	for (int i = 0; i < n; i++) {
		std::cout << i + 1 << "번째 값을 입력하세요: ";
		if (std::cin >> num_input) { // 예외 처리, 잘못된 입력이 들어오지 않았을 때만 벡터에 push.
			v.push_back(num_input);
			sum += num_input;
		} 
		else { // 잘못된 값 입력 시 이번 반복을 무효화.
			std::cout << "잘못된 입력입니다.\n";
			i -= 1;
			num_input = 0;
			std::cin.clear(); // 에러 상태 플래그 초기화
			// 입력 버퍼 비우기, 최대 크기만큼의 문자를 줄바꿈 문자를 만날 때까지 ignore
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); 
		}
	}
	// C++의 형변환 연산자 static_cast를 이용한 캐스팅 후 평균 계산
	avg = static_cast<double>(sum) / n;

	std::cout << "입력 결과(역순)\n";
	// v.size() - 반환 타입이 부호 없는 정수. i가 0보다 작아질 때의 문제를 방지하기 위해 형변환.
	for (int i = static_cast<int>(v.size()) - 1; i >= 0; i--) {
		std::cout << v[i] << '\n';
	}
	std::cout << "평균: " << avg;
}