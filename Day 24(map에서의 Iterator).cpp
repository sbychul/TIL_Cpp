#include <iostream>
#include <windows.h> 
#include <string>
#include <map> // map 사용을 위한 헤더
#include <vector>
#include <algorithm>

// [문제] F1 컨스트럭터 챔피언십 포인트 관리 및 안전 조회 시스템
// 특정 팀의 포인트를 안전하게 조회하는 독립 함수
void printTeamPoint(std::map<std::string, int>& table, std::string team_name) {
	auto it = table.find(team_name); // 임시 이터레이터 소환
	// 만약 map에 없는 것을 find() 함수에 넣는다면 find() 함수에서 .end() 이터레이터가 반환된다.
	if (it == table.end()) { std::cout << "존재하지 않는 팀입니다.\n"; return; }

	// 찾았다면 해당 team_name의 이터레이터를 반환한다.
	// map의 이터레이터는 pair 객체를 가리키므로, ->first(Key), ->second(Value) 으로 접근한다.
	std::cout << "[조회 성공] [" << team_name << "]의 현재 컨스트럭터 챔피언십 포인트: " << it->second << "점\n";
}

int main() {
	SetConsoleOutputCP(65001);
	// std::string을 Key로, int를 Value로 가지는 map 생성.
	std::map<std::string, int> team_standings;

	// map 상에 삽입하는 방법 1. insert 함수 이용하기
	// 1) 유니폼 초기화 사용 시
	team_standings.insert({ "Mercedes", 333 }); 
	// 2) std::pair 명시적 사용 시
	team_standings.insert(std::pair<std::string, int>("Ferrari", 255));
	// 3) std::make_pair 사용 시
	team_standings.insert(std::make_pair("McLaren", 179));

	// map 상에 삽입하는 방법 2. 대괄호 연산자 사용하기
	team_standings["Red Bull"] = 129;

	printTeamPoint(team_standings, "Ferrari"); // 성공 케이스 확인
	printTeamPoint(team_standings, "Williams"); // 실패 케이스 확인

	// 이터레이터를 활용하여 전체 순회 및 데이터 출력하기
	std::cout << "\n[2026 F1 CONSTRUCTORS' CHAMPIONSHIP] (팀명 순 정렬)\n";
	auto map_it = team_standings.begin();
	for (; map_it != team_standings.end(); map_it++) {
		std::cout << "팀명: " << map_it->first << " / 포인트: " << map_it->second << "점\n";
	}

	// 만약 Value(포인트 순)로 정렬하고 싶다면?
	// 맵의 모든 데이터를 정렬이 가능한 벡터(std::vector)로 복사.
	std::vector<std::pair<std::string, int>> vec(team_standings.begin(), team_standings.end());

	// 람다 함수를 이용해 Value(점수) 기준 내림차순 정렬.
	std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // 포인트가 큰 순서대로
	});

	auto vec_it = vec.begin();
	std::cout << "\n[2026 F1 CONSTRUCTORS' CHAMPIONSHIP] (포인트 순 정렬)\n";
	for (; vec_it != vec.end(); vec_it++) {
		std::cout << "팀명: " << vec_it->first << " / 포인트: " << vec_it->second << "점\n";
	}
}