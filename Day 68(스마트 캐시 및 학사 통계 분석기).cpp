#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iomanip>
#include <windows.h>
using namespace std; // 교수님께서 오리엔테이션에서 사용하라고 하셨기에 추가

// 오늘의 실습 과제: 스마트 캐시 및 학사 통계 분석기 (SmartCache)
// 오늘의 과제는 2학기 강의계획서의 제11강(연관 컨테이너 std::map과 std::vector),
// 제8강(함수 호출 연산자 중복: operator()), 제14강(예외 처리)을 결합한 "스마트 캐시 및 학사 통계 분석기 (SmartCache)"입니다.
// 
// [문제 설명]
// 학사 데이터베이스 조회를 모사하여, 학번(Key)을 기준으로 학생의 평점(Value)을 빠르게 조회하고 관리하는 캐시 시스템을 구현합니다.
// 캐시 조회 시 함수 객체(operator())를 활용하여 캐시 히트(Hit)와 미스(Miss) 통계를 추적하고, 용량 초과 시 오래된 데이터를 갱신하거나 예외를 발생시키도록 설계합니다.

// 학생 성적 클래스
class StudentScore {
private:
    string studentId; // 학번
    string name; // 이름
    double gpa; // 학점
public:
    // Constructor
    StudentScore() = default;
    StudentScore(string id, string studentName, double studentGpa) : studentId{ id }, name{ studentName }, gpa{ studentGpa } {}

    // Getter
    string getId() const { return studentId; }
    string getName() const { return name; }
    double getGpa() const { return gpa; }

    // 삽입 연산자 오버로딩, [학번: studentId] name (평점: gpa) 형식.
    friend ostream& operator<<(ostream& os, const StudentScore& sc) {
        os << "[학번: " << sc.getId() << "] " << sc.getName() << " (평점: " << sc.getGpa() << ")" << endl;
        return os;
    }
};

// 캐시 적중률 추적 함수 객체
class CacheStats {
private:
    int hitCount; // 적중 횟수, 기본값 0
    int missCount; // 실패 횟수, 기본값 0
public:
    // Constructor
    CacheStats() : hitCount{ 0 }, missCount{ 0 } {}

    // 함수 호출 연산자 오버로딩, Functor.
    // 클래스 객체를 일반 함수 호출문처럼 사용할 수 있게 됨.
    // 일반 함수와 달리 객체 내부의 상태 값을 멤버 변수로 계속 유지할 수 있다는 장점이 있다.
    void operator()(bool isHit) {
        if (isHit) { hitCount++; }
        else { missCount++; }
    }
    // 총 조회수 대비 히트율 계산.
    double getHitRatio() const {
        if (hitCount + missCount == 0) { return 0.0; } // 조회수가 0이면 그대로 0.0 반환.
        return static_cast<double>(hitCount) / (hitCount + missCount) * 100;
    }
    // 누적 Hit, Miss 수, 적중률 출력 함수
    void printStats() const {
        cout << "Hit: " << hitCount << "회 / Miss: " << missCount << "회 (적중률: " << getHitRatio() << "%)" << endl;
    }
};

// 캐시 관리 클래스
class SmartCache {
private:
    map<string, StudentScore> cacheMap; // 학번 기준 캐시 저장용 map
    size_t cap; // 최대 용량
    CacheStats stats; // 캐시 통계 추적 객체
public:
    // Constructor, cap 받아 초기화
    SmartCache(size_t capacity) : cap{ capacity } {}

    void put(const StudentScore& score) {
        // 새로운 키인지 확인 (이미 있는 key라면 갱신을 해야 하기 때문)
        bool isNewKey = (cacheMap.find(score.getId()) == cacheMap.end());
        // 새로운 키가 들어오려 하는데 꽉 찼다면 예외 투척
        if (isNewKey && cacheMap.size() >= cap) { throw overflow_error("캐시 용량이 가득 찼습니다."); }
        cacheMap[score.getId()] = score; 
    }

    StudentScore get(const string& studentId) {
        if (cacheMap.contains(studentId)) {
            stats(true); // functor 활용, 객체 자체를 함수 호출문처럼 사용, 위에서 정의한 함수 호출 연산자 오버로딩문 시행.
            return cacheMap[studentId];
        }
        stats(false);
        throw runtime_error("캐시에 존재하지 않는 학번입니다."); // 없는 학번이면 예외를 던져버림
    }

    void printAll() const {
        for (auto it = cacheMap.begin(); it != cacheMap.end(); it++) { cout << it->second; }
    }

    // 현재 통계 객체를 반환
    const CacheStats& getStats() const { return stats; }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    // 최대 용량 3개짜리 캐시 생성
    SmartCache cache(3);

    cout << "=== 1. 캐시 데이터 적재 ===" << endl;
    cache.put(StudentScore("2024001", "홍길동", 4.15));
    cache.put(StudentScore("2024002", "김철수", 3.80));
    cache.put(StudentScore("2024003", "이영희", 4.35));
    cout << "-> 기본 3명 데이터 적재 완료!\n" << endl;

    cout << "=== 2. 캐시 용량 초과 예외 테스트 ===" << endl;
    try {
        cout << "4번째 데이터(2024004, 박민수) 적재 시도..." << endl;
        cache.put(StudentScore("2024004", "박민수", 3.50));
    }
    catch (const std::overflow_error& e) {
        cout << "[용량 초과 감지] " << e.what() << endl;
    }

    cout << "\n=== 3. 캐시 조회 및 적중률 추적 (operator() 활용) ===" << endl;
    try {
        // 정상 조회 (Hit)
        cout << "조회(2024001): " << cache.get("2024001") << endl;
        cout << "조회(2024003): " << cache.get("2024003") << endl;

        // 미존재 조회 (Miss)
        cout << "조회(2024999): 시도 중..." << endl;
        cache.get("2024999");
    }
    catch (const std::runtime_error& e) {
        cout << "[조회 실패 감지] " << e.what() << endl;
    }

    cout << "\n=== 4. 현재 캐시 보관 현황 (std::map 자동 정렬) ===" << endl;
    cache.printAll();

    cout << "\n=== 5. 캐시 성능 통계 리포트 ===" << endl;
    cache.getStats().printStats();

    cout << "\n=== 프로그램 정상 종료 ===" << endl;
    return 0;
}