#include <iostream>
#include <windows.h> 
#include <string>
#include <unordered_map>
#include <vector>

// [문제] F1 포메이션 랩(Formation Lap) 미복귀 드라이버 추적 시스템
// F1 레이스 시작 전, 모든 드라이버는 포메이션 랩을 돌며 타이어 온도를 올립니다.
// 모든 차량이 피트 그리드에 정상적으로 복귀해야 하지만, 도중에 차량 결함이나 스핀으로 인해 그리드에 복귀하지 못한 낙오 드라이버가 발생했습니다.
// 출발 그리드에 대기 중이던 전체 드라이버 명단과 피트 그리드에 정상 복귀한 드라이버 명단이 주어질 때,
// 복귀하지 못한 단 한 명의 드라이버 이름을 찾아내는 고성능 시스템을 구축하세요.

std::string findMissingDriver(const std::vector<std::string>& participant, const std::vector<std::string>& completion) {
	std::unordered_map<std::string, int> driver_counts;
	// 참여자 명단을 순회하며 각 드라이버의 등장 횟수를 맵에 누적
	for (const std::string& name : participant) {
		driver_counts[name]++;
	}

	// 완주자 명단을 순회하며 각 드라이버의 등장 횟수를 맵에서 차감
	// 등장 횟수(count)가 0이 된다면 정상적으로 완주했음을 의미.
	for (const std::string& name : completion) {
		driver_counts[name]--;
	}

	// iterator를 활용하여 pair.second가 0이 아닌 드라이버를 찾아낸다.
	for (const auto& pair : driver_counts) {
		if (pair.second != 0) { return pair.first; }
		// 단 한 명이라고 문제에서 주어졌으니 그냥 드라이버명만 반환하고 종료.
		// 여러 명이라면 새 vector를 만들어 거기에 추가해서 반환하던지 해야겠지요?
	}
}

int main() {
	SetConsoleOutputCP(65001);
	// 결과 확인용 벡터
	std::vector<std::string> participant = { "Hamilton", "Leclerc", "Verstappen", "Alonso", "Leclerc" };
	std::vector<std::string> completion = { "Hamilton", "Alonso", "Verstappen", "Leclerc" };
	// 결과: 동명이인 중 한 명이 완주하지 못했으므로 "Leclerc"가 출력되어야 함.

	// 결과 확인
	std::cout << "미복귀 드라이버: " << findMissingDriver(participant, completion) << std::endl;
}