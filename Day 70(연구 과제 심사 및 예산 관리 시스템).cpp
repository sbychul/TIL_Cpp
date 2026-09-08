#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <windows.h>

using namespace std;

// 오늘의 실습 과제: 학술 연구 과제 심사 및 예산 관리 시스템 (ResearchProjectManager)
// [문제 설명]
// 교내 연구 지원 센터에서 접수된 연구 과제(교수 주도 과제, 학생 주도 과제)를 심사하고 예산을 관리하는 시스템을 구현합니다.
// 연구비 지원은 심사 점수에 비례하여 책정되며, 전체 승인된 연구비가 총 지원 한도를 넘지 않도록 관리합니다.
// 
// 제6강(static 멤버 및 정적 함수), 제7강(프렌드 및 복합 연산자 오버로딩: +=, <<), 제9·10강(추상 클래스 기반 상속 및 순수 가상 함수 다형성),
// 제11강(STL vector, algorithm), 제12·13강(입출력 포맷 및 파일 영속화), 제14강(사용자 정의 예외 처리)의 6개 영역을 종합적으로 다룹니다.

// 사용자 정의 예외 클래스
class BudgetException : public exception {
private: string message; // e.what()을 사용하면 나올 메시지
public:
    BudgetException(string msg) : message{ msg } {}
    const char* what() const noexcept override { return message.c_str(); }
};

// 최상위 추상 클래스
class ResearchProject {
private:
    string id;
    string title;
    double score;
public:
    static int projectCounter;
    ResearchProject(string id, string title, double score) : id{ id }, title{ title }, score{ clamp(score, 0.0, 100.0) } {}

    // 순수 가상 함수들
    // 최종 승인 예산 연산 함수
    virtual double calculateApprovedBudget() const = 0; 
    // 과제 정보 출력 함수
    virtual void print(ostream& os) const = 0;

    // Getters
    string getId() const { return id; }
    string getTitle() const { return title; }
    double getScore() const { return score; }
    // static 멤버 함수, 현재까지 생성된 총 프로젝트 수를 반환
    static int getProjectCount() { return projectCounter; }

    // 관계 연산자 오버로딩, score 기준 내림차순 정렬, 점수가 같다면 id 기준 오름차순 정렬
    bool operator<(const ResearchProject& other) const {
        if (score == other.score) { return id < other.id; }
        return score > other.score;
    }
    // 삽입 연산자 오버로딩, 과제 정보 출력 함수를 내부에서 실행.
    friend ostream& operator<<(ostream& os, const ResearchProject& project) {
        project.print(os);
        return os;
    }
};

// 최상위 추상 클래스 static 변수 초기화
int ResearchProject::projectCounter = 0;

// 하위 클래스: 교수 연구 과제
class FacultyProject : public ResearchProject {
private:
    string labname;
    double baseFund; // 기본 신청금
public: 
    FacultyProject(string id, string name, double score, string labname, double fund) : labname{ labname }, baseFund{ fund }, ResearchProject(id, name, score) {}

    // 최종 승인 예산 연산 함수 오버라이딩
    double calculateApprovedBudget() const override {
        if (getScore() >= 80.0) { return baseFund * 1.2; } // 일을 잘 하면 돈을 20% 더 줍니다
        return baseFund; // 아니면 그냥 요청한 만큼 받아가세요
    }
    // 정보 출력 함수 오버라이딩
    void print(ostream& os) const override {
        os << "[교수 과제] [" << getId() << "] " << getTitle() << " (연구실: " << labname << ", 점수: "
            << getScore() << "점) -> 승인 예산: " << calculateApprovedBudget() << "만원";
        // "[교수 과제] [id] title (연구실: labName, 점수: score점) -> 승인 예산: budget만원" 형식으로 출력됨.
    }
};

// 하위 클래스: 학부생 연구 과제
class StudentProject : public ResearchProject {
private:
    string advisor; // 지도교수명
    double stipend; // 고정 활동비
public:
    StudentProject(string id, string name, double score, string advisor, double stipend) : advisor{ advisor }, stipend{ stipend }, ResearchProject(id, name, score) {}

    // 최종 승인 예산 연산 함수 오버라이딩
    double calculateApprovedBudget() const override {
        if (getScore() >= 90.0) { return stipend + 50.0; } // 일을 잘 하면 돈을 50만원 더 줍니다
        return stipend; // 아니면 고정 활동비에 만족하십시오
    }
    // 정보 출력 함수 오버라이딩
    void print(ostream& os) const override {
        os << "[학생 과제] [" << getId() << "] " << getTitle() << " (지도교수: " << advisor << ", 점수: "
            << getScore() << "점) -> 승인 예산: " << calculateApprovedBudget() << "만원";
        // "[학생 과제] [id] title (지도교수: advisor, 점수: score점) -> 승인예산: budget만원" 형식으로 출력됨.
    }
};

// 총괄 관리 매니저 클래스
class ResearchProjectManager {
private:
    vector<ResearchProject*> projects; // 동적 배열 연구 과제 목록
    double maxBudgetLimit; // 아쉽게도 학교는 돈이 무제한이 아닙니다
    double totalDisbursed; // 현재까지 사용된 예산
public:
    ResearchProjectManager(double budget) : maxBudgetLimit{ budget }, totalDisbursed{ 0.0 } {}
    ~ResearchProjectManager() { for (ResearchProject* p : projects) { delete p; ResearchProject::projectCounter--; } }

    // 추가 함수
    void addProject(ResearchProject* p) { projects.push_back(p); ResearchProject::projectCounter++; }
    // 점수 기준 내림차순 벡터 정렬 함수
    // 포인터 벡터이기 때문에 comparator를 넘겨준다(람다식으로 넘김)
    void sortProjects() { sort(projects.begin(), projects.end(), [](const ResearchProject* a, const ResearchProject* b) { return *a < *b; }); }
    // 벡터 전체 순회, 예산 한도에 한해 전부 승인하는 함수.
    void approveBudgets() {
        for (ResearchProject* p : projects) {
            double budget = p->calculateApprovedBudget();
            // 만약 얘를 주면 한도 초과라면 끝
            if (totalDisbursed + budget > maxBudgetLimit) { throw BudgetException("총 지원 한도 예산을 초과하여 승인이 중단되었습니다."); }
            totalDisbursed += budget;
            // 승인 완료 문구 출력
            cout << "-> [" << p->getId() << "] 승인 완료 (" << budget << " / 누적: " << totalDisbursed << ")" << endl;
        }
    }
    // 파일 영속화 함수
    void exportReport(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open()) { throw runtime_error("파일을 여는 데 문제가 발생했습니다."); }
        for (ResearchProject* p : projects) { outFile << *p << endl;; }
    }
    // 전체 출력 함수
    void printAll() const {
        for (ResearchProject* p : projects) { cout << *p << endl; }
    }
};

int main() {
    SetConsoleOutputCP(65001);

    // 총 가용 지원 한도: 500.0만원
    ResearchProjectManager manager(500.0);

    // 1. 프로젝트 등록
    manager.addProject(new FacultyProject("FP01", "인공지능 비전 연구", 92.0, "AI연구실", 200.0));  // 92점 -> 200 * 1.2 = 240만원
    manager.addProject(new StudentProject("SP01", "모바일 웹 최적화", 95.0, "김교수", 80.0));        // 95점 -> 80 + 50 = 130만원
    manager.addProject(new FacultyProject("FP02", "임베디드 OS 설계", 75.0, "시스템랩", 150.0));     // 75점 -> 150만원 (누적: 520만원 -> 한도 초과 예상)
    manager.addProject(new StudentProject("SP02", "알고리즘 시각화 툴", 82.0, "이교수", 60.0));      // 82점 -> 60만원

    cout << "=== 1. 총 생성된 프로젝트 개수 (static 멤버) ===" << endl;
    cout << "현재 등록된 전체 과제 수: " << ResearchProject::getProjectCount() << "개\n" << endl;

    cout << "=== 2. 과제 심사 점수 기준 정렬 (내림차순) ===" << endl;
    manager.sortProjects();
    manager.printAll();

    cout << "\n=== 3. 예산 승인 사정 및 초과 예외 검사 ===" << endl;
    try {
        manager.approveBudgets();
    }
    catch (const BudgetException& e) {
        cout << "[예외 감지] " << e.what() << endl;
    }

    cout << "\n=== 4. 심사 결과 파일 영속화 리포트 출력 ===" << endl;
    try {
        manager.exportReport("day70_research_report.txt");
        cout << "-> 'research_report.txt' 파일 저장 완료!" << endl;
    }
    catch (const exception& e) {
        cout << "[파일 에러] " << e.what() << endl;
    }

    cout << "\n=== 프로그램 정상 종료 ===" << endl;
    return 0;
}