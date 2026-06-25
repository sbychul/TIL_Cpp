#include <iostream>
#include <windows.h> 
#include <map>
#include <string>

// [문제] 과일 투표 결과 집계기
// 1. 사용자로부터 투표할 과일 이름의 개수 N을 입력받습니다.
// 2. 이어서 N개의 과일 이름을 공백이나 줄바꿈을 두고 연속으로 입력받아 std::map<std::string, int>에 각 과일별 투표 횟수를 누적합니다.
// 3. 입력이 모두 끝나면, 현재까지 투표를 받은 모든 과일의 이름과 득표수를 사전 순(과일 이름 오름차순)으로 출력하세요.
// 4. 마지막 줄에는 유저가 알고 싶어 하는 특정 과일의 이름을 추가로 입력받아 해당 과일의 득표수를 출력하세요. (만약 투표 명단에 없는 과일이라면 0을 출력해야 합니다.)

int main() {
	SetConsoleOutputCP(65001);
	int count{ 0 }; // 투표 횟수
	std::string name_input{}; // 이름 입력
	std::map<std::string, int> votes{}; // 투표 결과 저장할 map

	std::cout << "투표할 과일 이름의 수를 입력하세요: ";
	if (std::cin >> count) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
	else { std::cout << "잘못된 입력입니다."; return -1; }

	for (int i = 0; i < count; i++) {
		std::cout << "투표할 과일의 이름을 입력하세요: ";
		std::getline(std::cin, name_input);
		// 처음 들어오면 0으로 초기화되는 성질을 이용, 1씩 더해 줌.
		votes[name_input] += 1;
	}

	std::cout << "\n[투표 결과]\n";
	for (const auto& item : votes) {
		std::cout << item.first << ": " << item.second << "표\n";
	}

	std::cout << "\n득표수를 확인할 과일의 이름을 입력하세요: ";
	std::getline(std::cin, name_input);
	std::cout << name_input << "의 득표 수는 " << votes[name_input] << "표 입니다." << std::endl;
}