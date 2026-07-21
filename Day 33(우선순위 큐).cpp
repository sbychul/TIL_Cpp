#include <iostream>
#include <windows.h> 
#include <string>
#include <queue>
#include <vector>

// (제 제미나이가 F1 좋아하는 제게 개인화가 야무지게 잘 되어버렸습니다..)
// [문제] 피트 스톱(Pit Stop) 긴급 정비 제어 시스템
// 결함 상태가 심각한 차량부터 출차되도록 만들기.
struct CarRequest {
    int car_number;
    int priority; // 긴급도 점수 (높을수록 긴급함)

    // priority_queue에서 긴급도(priority) 기준 내림차순 정렬을 위한 연산자 오버로딩
    bool operator<(const CarRequest& other) const {
        return priority < other.priority;
    }
};

void processEmergencyPitStop(const std::vector<CarRequest>& requests) {
    std::cout << "=== 긴급 정비 스케줄링 시작 ===\n";
    std::priority_queue<CarRequest> pq;
    for (CarRequest request : requests) { pq.push(request); } // 벡터 순회하며 삽입.
    while (!pq.empty()) { // 큐가 빌 때까지 반복
        int num = pq.top().car_number;
        int pr = pq.top().priority;
        std::cout << "[긴급도 " << pr << "] 차량 " << num << " 정비 시작 및 출차\n";
        pq.pop();
    }
    std::cout << "=== 모든 차량 정비 완료 ===" << std::endl;
}

int main() {
	SetConsoleOutputCP(65001);
    // 테스트 케이스
    std::vector<CarRequest> requests = {
        { 44, 2 },  // 해밀턴 (긴급도 2: 타이어 마모)
        { 16, 5 },  // 르클레르 (긴급도 5: 엔진 연기 - 최우선!)
        { 3, 1 },   // 베르스타펜 (긴급도 1: 정기 피트스톱)
        { 14, 3 }   // 알론소 (긴급도 3: 프론트 윙 파손)
    };

    processEmergencyPitStop(requests);
}