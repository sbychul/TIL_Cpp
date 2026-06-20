#include <iostream>
#include <windows.h> 
#include <vector>

// [문제] 점수 조정 및 합격 판별기
// main 함수에서 유저로부터 시험 점수 3개를 입력받아 std::vector<int>에 저장합니다.
// 점수를 보정해 주는 함수 void adjustScores(std::vector<int> &scores)를 작성하세요.
// 이 함수는 전달받은 벡터의 원본을 직접 수정합니다.
// 각 점수가 60점 미만이라면 보너스 점수 10점을 더해주고, 100점을 초과하게 되면 100점으로 고정합니다. (60점 이상은 그대로 유지)
// main 함수로 돌아와, 보정된 점수들을 범위 기반 for 문을 사용하여 한 줄에 하나씩 출력하세요.

// 벡터의 주소를 받아 그 자체를 수정하는 함수.
void adjustScores(std::vector<int>& scores) {
	for (int i = 0; i < scores.size(); i++) {
		if (scores[i] < 60) { scores[i] += 10; } // 60점 미만, 보너스 10점.
		if (scores[i] > 100) { scores[i] = 100; } // 100점 초과라면 100점으로 조정.
	}
}

int main() {
	SetConsoleOutputCP(65001);
	std::vector<int> scores; // 점수를 담을 벡터 선언.
	int n{ 0 }, num_input{ 0 };

	std::cout << "입력할 점수의 개수를 입력하세요: ";
	std::cin >> n;

	for (int i = 0; i < n; i++) {
		std::cout << i + 1 << "번째 점수를 입력하세요: ";
		if (std::cin >> num_input) { // 예외 처리, 잘못된 입력이 들어오지 않았을 때만 벡터에 push.
			scores.push_back(num_input);
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

	adjustScores(scores);
	for (int i = 0; i < scores.size(); i++) {
		std::cout << i + 1 << "번째 최종 점수: " << scores[i] << "\n";
	}
}