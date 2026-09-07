#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <windows.h>
using namespace std;

// 오늘의 실습 과제: 성적 변환 및 장학 펀드 집계기 (GradeEvaluator)
// 강의계획서의 제11강(STL 알고리즘: std::transform, std::accumulate), 제8강(산술 및 스트림 연산자 오버로딩: operator+, operator<<),
// 제14강(예외 처리)을 아우르는 "수강생 성적 변환 및 장학 펀드 집계기 (GradeEvaluator)"입니다.
// 
// [문제 설명]
// 학생들의 원점수를 곡선 보정(Curving) 알고리즘을 통해 가산 변환하고, 변환된 점수를 기준으로 장학금 지급 대상을 선별하여 총 소요 장학 기금을 산출하는 시스템입니다.

// 학생 정보 클래스
class StudentRecord {
private:
    string id; // 학번
    string name; // 이름
    double rawScore; // 원점수
public:
    // Constructor
    StudentRecord() = default; // 매개변수 없는 생성자
    StudentRecord(string studentId, string studentName, double score) : id{ studentId }, name{ studentName }, rawScore{ score } {}

    // Getter
    string getId() const { return id; }
    string getName() const { return name; }
    double getRawScore() const { return rawScore; }

    // 연산자 오버로딩
    // double bonus를 더하면 해당 점수가 가산된 새 객체를 반환 (최대 점수는 100.0점)
    StudentRecord operator+(double bonus) const { return StudentRecord(id, name, min((rawScore + bonus), 100.0)); }
    // 삽입 연산자 오버로딩 ("[id] name: rawScore점" 형식)
    friend ostream& operator<<(ostream& os, const StudentRecord& record) {
        os << "[" << record.id << "] " << record.name << ": " << record.rawScore << "점" << endl;
        return os;
    }
};

// 정렬을 위한 함수, 원점수 기준 내림차순 정렬
bool compareRawScore(const StudentRecord& s1, const StudentRecord& s2) {
    return s1.getRawScore() > s2.getRawScore();
}

// 장학 펀드 집계 클래스
class ScholarshipFund {
private:
    double totalBudget; // 총 가용 예산
    double disbursedAmount; // 누적 지급 예산
public:
    // Constructor
    ScholarshipFund(double budget) : totalBudget{ budget }, disbursedAmount{ 0 } {}

    // += 연산자 오버로딩
    ScholarshipFund& operator+=(double grant) {
        // 입력받은 지급액 + 지금까지 총 지급액이 예산보다 크다면 예외 던지기
        if (disbursedAmount + grant > totalBudget) { throw runtime_error("장학 예산이 초과되었습니다."); }
        disbursedAmount += grant; // 아니라면 반영
        return *this; // 연속 대입을 위하여 *this 반환.
    }

    // Getter (잔여 예산 게터)
    double getRemainingBudget() { return totalBudget - disbursedAmount; }
};

// 성적 평가 및 집계 시스템
class GradeEvaluator {
private: vector<StudentRecord> records; // 정보를 보관할 벡터를 멤버 변수로 가짐
public:
    GradeEvaluator() = default;

    // 학생 추가 함수
    void addStudent(const StudentRecord& record) { records.push_back(record); }

    // 모든 학생의 점수에 bonus 가산 함수
    void applyCurveBonus(double bonus) {
        // std::transform : 특정 범위의 컨테이너 원소들을 순회하며 전달된 단항/이항 함수(또는 람다식)을 적용한 뒤,
        // 그 결과를 다른 컨테이너 또는 자기 자신에 저장하는 함수. for 루프 없이 일괄 변환하거나 정규화할 때 효율적이다.
        // std::transform(어디부터, 어디까지, 결과를 어디에 작성할 지, 함수); 
        transform(records.begin(), records.end(), records.begin(), [bonus](const StudentRecord& record) { return record + bonus; });
    }

    // 전체 학생 점수의 평균을 산출하는 함수
    double calculateClassAverage() const {
        // 학생이 없다면 0.0
        if (records.size() == 0) { return 0.0; }
        // std::accumulate : 범위 내의 모든 요소를 누적할 때 사용. 덧셈 뿐만 아니라 커스텀 람다/이항 연산자를 넘겨서
        // 객체 목록의 특정 멤버 총합을 한 줄로 계산할 수 있다.
        // std::accumulate(어디부터, 어디까지, 초기 값, 함수);
        double totalRawScore = accumulate(records.begin(), records.end(), 0.0, [](double currentSum, const StudentRecord& record)
            { return currentSum + record.getRawScore(); });
        return totalRawScore / records.size(); // 이후 평균 계산하여 반환
    }

    // 장학금 수여 함수
    void awardScholarships(ScholarshipFund& fund, double cutoffScore, double grantAmount) {
        sort(records.begin(), records.end(), compareRawScore);
        try { 
            for (StudentRecord s : records) {
                if (s.getRawScore() >= cutoffScore) {
                    fund += grantAmount;
                    cout << "장학금 지급: [" << s.getId() << "] " << s.getName() << " (" << grantAmount << "만원)" << endl;
                }
            }
        }
        catch (const exception& e) {
            throw; // 예외 그대로 재전파
        }
    }

    // 전체 학생 명단 출력 함수
    void printAll() const {
        for (StudentRecord s : records) { cout << s; }
    }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    GradeEvaluator evaluator;
    evaluator.addStudent(StudentRecord("2024001", "홍길동", 85.0));
    evaluator.addStudent(StudentRecord("2024002", "김철수", 78.5));
    evaluator.addStudent(StudentRecord("2024003", "이영희", 92.0));
    evaluator.addStudent(StudentRecord("2024004", "박민수", 96.0));

    cout << "=== 1. 초기 성적 명단 ===" << endl;
    evaluator.printAll();
    cout << "초기 전체 평균: " << evaluator.calculateClassAverage() << "점\n" << endl;

    cout << "=== 2. transform 활용 보너스 가산 (각 5.0점) ===" << endl;
    evaluator.applyCurveBonus(5.0);
    evaluator.printAll();
    cout << "보정 후 전체 평균: " << evaluator.calculateClassAverage() << "점\n" << endl;

    cout << "=== 3. 장학금 지급 사정 (기준: 90점 이상, 인당 50만원) ===" << endl;
    ScholarshipFund fund(120.0); // 총 예산 120만원 (2명분 가능, 3명째 초과 예상)

    try {
        evaluator.awardScholarships(fund, 90.0, 50.0);
    }
    catch (const runtime_error& e) {
        cout << "[예외 발생] " << e.what() << endl;
    }

    cout << "남은 장학 예산: " << fund.getRemainingBudget() << "만원" << endl;

    cout << "\n=== 프로그램 정상 종료 ===" << endl;
    return 0;
}