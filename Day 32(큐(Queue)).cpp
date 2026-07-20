#include <iostream>
#include <windows.h> 
#include <string>
#include <queue>
#include <vector>

// [문제] 피트 스톱 차량 진입 및 정비 스케줄러 구현
void simulatePitStop(const std::vector<int>& entry_order) {
	std::queue<int> entry_queue; // 빈 큐 선언

	// 순회하며 큐에 삽입
	for (int number : entry_order) { 
		entry_queue.push(number);
		std::cout << "차량 [" << number << "] 피트 레인 진입\n";
	}

	std::cout << "------------------------------\n";

	// 출차 작업, 큐가 빌 때까지 반복
	while (!entry_queue.empty()) {
		std::cout << "차량 [" << entry_queue.front() << "] 정비 완료 및 출차\n";
		entry_queue.pop(); // 빼주기
	}
}

int main() {
	SetConsoleOutputCP(65001);
	std::vector<int> entry_order = { 44, 16, 1, 14 };
	// 해밀턴(44), 르클레르(16), 노리스(1), 알론소(14) 순서 진입

	// 들어온 순서대로 나가는지 확인하자.
	simulatePitStop(entry_order);
}