#include <iostream>
#include <windows.h> 
#include <string>
#include <unordered_map>

// std::unordered_map
// 내부적으로 해시 테이블을 사용, 정렬이 보장되지 않음. 대신 탐색/삽입/삭제 속도가 O(1)
// 정렬이 필요 없고 속도가 중요할 때 사용한다.

// [문제] F1 레이서 드라이버 오브 더 데이(Driver of the Day) 투표 집계 시스템

// 투표 결과를 안전하게 확인하는 독립 함수
void checkVoteResult(std::unordered_map<std::string, int>& registry, std::string name) {
	auto registry_it = registry.find(name);
	// 단순 map과 같이, 없다면 find가 .end()를 반환한다.
	if (registry_it == registry.end()) { std::cout << "[조회 실패] 명단에 없는 드라이버입니다\n"; return; }
	// 똑같이 first(키), second(값)으로 접근할 수 있다.
	std::cout << "[조회 성공] " << name << "의 득표 수: " << registry_it->second << "표\n";
}

int main() {
	SetConsoleOutputCP(65001);
	// 드라이버 이름을 Key, 득표 수를 Value로 가지는 맵 생성
	std::unordered_map<std::string, int> vote_registry;
	// 투표 원본 로그 역할을 할 문자열 배열
	std::string votes[] = {
		"Hamilton", "Leclerc", "Hamilton", "Verstappen",
		"Leclerc", "Hamilton", "Norris", "Verstappen"
	};
	// 배열을 순회하며 map에 득표 수를 누적
	for (const std::string& driver : votes) {
		vote_registry[driver]++;
	}

	// 조회 성공/실패에 따른 출력 결과 확인
	checkVoteResult(vote_registry, "Hamilton");	// 성공 시 결과 확인
	checkVoteResult(vote_registry, "Bearman"); // 실패 시 결과 확인

	// map을 순회하며 결과 출력
	std::cout << "\n=== 최종 투표 집계 결과 (해시 순서)===\n";
	auto vote_it = vote_registry.begin();
	for (; vote_it != vote_registry.end(); vote_it++) {
		std::cout << vote_it->first << ": " << vote_it->second << "표\n";
	}

	// 득표수로 정렬이 너무 하고 싶다면 어제 했던 vector로 변환하여 정렬하는 방법을 씁시다.
}