#include <iostream>
#include <windows.h> 
#include <vector>
#include <string>
#include <algorithm>

// [문제] 영웅 선택률 통계 프로그램
// 영웅 정보를 저장할 Hero 구조체 또는 클래스를 설계하세요.
// 멤버 변수: 영웅 이름(std::string name), 플레이 횟수(int play_count), 승률(double win_rate)
// 사용자로부터 등록할 영웅의 수 N을 입력받습니다. (잘못된 입력 시 예외 처리 필수!)
// 이어서 N개 영웅의 이름, 플레이 횟수, 승률을 공백을 두고 연속으로 입력받아 std::vector<Hero>에 저장합니다.
// 입력이 끝나면, 아래의 우선순위 규칙에 맞게 영웅들을 정렬하여 이름, 플레이 횟수, 승률을 예쁘게 출력하세요.
// 1순위: 플레이 횟수가 많은 영웅이 우선 (내림차순)
// 2순위: 플레이 횟수가 같다면, 승률이 높은 영웅이 우선(내림차순)
// 3순위: 플레이 횟수와 승률이 모두 같다면, 영웅 이름 사전 순서가 앞서는 영웅이 우선(오름차순)

class Hero {
public:
	std::string name;
	int play_count;
	double win_rate;
	Hero(std::string name_input, int count_input, double rate_input) : name{ name_input }, play_count{ count_input }, win_rate{ rate_input } {}
};

// 정렬 함수
bool compareHeroes(Hero& a, Hero& b) {
	if (a.play_count == b.play_count) { // 플레이 횟수가 같을 때
		if (a.win_rate == b.win_rate) { return a.name < b.name; } // 승률이 같다면 사전 순
		else { return a.win_rate > b.win_rate; } // 승률이 다르다면 승률 순
	}
	return a.play_count > b.play_count; // 2순위로 넘어갈 이유조차 없다면 플레이 횟수 순
}

int main() {
	SetConsoleOutputCP(65001);
	std::vector<Hero> heroes{}; // 빈 벡터 선언
	int plays_input{ 0 }, heroes_count{ 0 }; // 정수 입력을 위한 변수
	double rate_input{ 0 }; // 실수 입력을 위한 변수
	std::string name_input{}; // 이름(문자열) 입력을 위한 변수

	std::cout << "등록할 영웅의 수를 입력하세요: ";
	if (std::cin >> heroes_count) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); } // 문자열 입력을 위한 버퍼 비우기
	else { std::cout << "잘못된 입력입니다." << std::endl; return 1; } // 예외 처리

	std::cout << "\n[영웅 등록]\n";
	for (int i = 0; i < heroes_count; i++) {
		std::cout << i + 1 << "번째 영웅의 이름을 입력하세요: ";
		std::getline(std::cin, name_input);

		std::cout << i + 1 << "번째 영웅의 플레이 횟수를 입력하세요: ";
		if (std::cin >> plays_input) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
		else { // 예외 처리, 반복 무효화
			std::cout << "잘못된 입력입니다.\n";
			i--;
			plays_input = 0;
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			continue;
		}

		std::cout << i + 1 << "번째 영웅의 승률을 입력하세요: ";
		if (std::cin >> rate_input) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
		else { // 예외 처리, 반복 무효화
			std::cout << "잘못된 입력입니다.\n";
			i--;
			rate_input = 0;
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			continue;
		}
		
		// 새 객체 생성하여 벡터에 삽입.
		heroes.push_back(Hero{ name_input, plays_input, rate_input });
	}
	// sort 함수에 비교 함수를 넣어 기준에 맞게 정렬
	std::sort(heroes.begin(), heroes.end(), compareHeroes);

	// 출력부
	std::cout << "\n[영웅 통계 입력 결과]\n";
	for (int i = 0; i < heroes.size(); i++) {
		std::cout << i + 1 << ": " << heroes[i].name << "(" << heroes[i].play_count << "판, 승률: " << heroes[i].win_rate << "%)\n";
	}
}