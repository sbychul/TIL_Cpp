#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 종합 성적 관리 및 장학 사정 시스템 (ScholarshipGradeBook)
// 강의계획서의 제6·7강(상속과 가상 함수 다형성), 제8·9강(연산자 중복: <, ==, <<),
// 제11강(STL std::vector, std::sort), 제12강(파일 영속화 직렬화), 제14강(사용자 정의 예외)까지 5개 영역을 유기적으로 결합했습니다.

// 커스텀 예외 클래스
class GradeException : public std::exception {
private: std::string msg; // 예외 메시지
public: 
    // 생성자를 통해 메시지 설정, what()을 통해 해당 메시지를 출력
    GradeException(std::string message) : msg{ message } {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// 기반이 될 추상 클래스
class CourseGrade {
private:
    std::string courseCode; // 과목 코드
    std::string courseName; // 과목명
    int rawScore; // 원점수
public:
    CourseGrade(std::string code, std::string name, int score) : courseCode{ code }, courseName{ name }, rawScore{ score } {}
    virtual ~CourseGrade() {} // 가상 소멸자 선언

    // 순수 가상 함수
    virtual double calculateWeightedScore() const = 0;
    virtual void printDetails(std::ostream& os) const = 0;

    // 연산자 오버로딩
    // 동등 연산자, 과목 코드가 같다면 같은 과목.
    bool operator==(const CourseGrade& other) const { return courseCode == other.courseCode; }
    // 관계 연산자, 가중 점수 기준 내림차순 정렬
    bool operator<(const CourseGrade& other) const {
        // 가중 점수가 같다면 과목 코드 오름차순
        if (calculateWeightedScore() == other.calculateWeightedScore()) { return courseCode < other.courseCode; }
        return calculateWeightedScore() > other.calculateWeightedScore();
    }
    // 삽입 연산자, printDetails 호출.
    friend std::ostream& operator<<(std::ostream& os, const CourseGrade& grade) { grade.printDetails(os); return os; }

    // Getters
    std::string getCode() const { return courseCode; }
    std::string getName() const { return courseName; }
    int getRawScore() const { return rawScore; }
};

// 하위 클래스, 전공 과목
class MajorCourse : public CourseGrade {
private: int credits; // 학점
public:
    MajorCourse(std::string code, std::string name, int score, int credit) : credits{ credit }, CourseGrade(code, name, score) {}
    // 전공 가중치는 10%, 1.1을 곱해서 반환, 최대는 100.0점.
    double calculateWeightedScore() const override { return min(100.0, getRawScore() * 1.1); }
    // [전공] [code] name (rawScore점) -> 가중평가: weightedScore점 형태로 출력.
    void printDetails(std::ostream& os) const override { os << "[전공] [" << getCode() << "] " << getName() << "(" <<
        getRawScore() << "점) -> 가중평가:" << calculateWeightedScore() << "점" << std::endl; }
};

class ElectiveCourse : public CourseGrade {
public:
    ElectiveCourse(std::string code, std::string name, int score) : CourseGrade(code, name, score) {}
    // 교양은 가중치 없음, 그대로 반환
    double calculateWeightedScore() const override { return getRawScore(); }
    // [교양] [code] name (rawScore점) -> 가중평가: weightedScore점 형태로 출력.
    void printDetails(std::ostream& os) const override {
        os << "[교양] [" << getCode() << "] " << getName() << "(" <<
            getRawScore() << "점) -> 가중평가:" << calculateWeightedScore() << "점" << std::endl;
    }
};

// 성적 관리 및 사정 클래스
class ScholarshipGradeBook {
private: std::vector<CourseGrade*> courses;
public:
    // 생성자와 소멸자
    ScholarshipGradeBook() = default;
    ~ScholarshipGradeBook() { for (CourseGrade* c : courses) { delete c; } }

    // 추가 함수
    void addCourse(CourseGrade* c) {
        // 이미 있는 과목 코드인지 확인
        for (int i = 0; i < courses.size(); i++) {
            // 있다면 받은 CourseGrade의 할당을 해제하고 예외를 던짐.
            if (*courses[i] == *c) { delete c; throw GradeException("이미 등록된 과목 코드입니다."); }
        }
        courses.push_back(c); // 없으면 추가.
    }
    // 정렬 함수, sort에 역참조 비교 람다 함수를 집어넣어 관계 연산자 오버로딩한 것을 호출
    void sortGrades() { std::sort(courses.begin(), courses.end(), [](const CourseGrade* a, const CourseGrade* b) {
        return *a < *b;
        }); }
    // 전체 점수 가중 평균을 계산하는 함수
    double getTopStudentScholarshipCut(double minAverage) {
        if (courses.size() == 0) { return 0.0; } // 과목이 아무것도 없다면 0.0 반환
        double avg{ 0.0 };
        for (CourseGrade* c : courses) { avg += c->calculateWeightedScore(); }
        avg = avg / courses.size(); // 전체 가중 평균 계산.
        if (avg < minAverage) { throw GradeException("장학 기준 성적 미달입니다."); } // 최저 평균 기준 미만이라면 예외를 던짐
        return avg; // 아니라면 그대로 평균 점수를 반환
    }
    // 성적표 파일 저장 함수
    void exportReport(const std::string& filename) {
        std::ofstream outFile(filename);
        // 예외 처리
        if (!outFile.is_open()) { throw std::runtime_error("파일을 여는 데 실패했습니다."); }
        for (CourseGrade* c : courses) { outFile << *c; } // 전체 성적표를 파일로 저장
    }
    // 전체 목록 및 평균 출력 함수
    friend std::ostream& operator<<(std::ostream& os, const ScholarshipGradeBook& book) {
        double avg{ 0.0 };
        os << "[전체 과목 목록]\n";
        for (CourseGrade* c : book.courses) {
            os << *c;
            avg += c->calculateWeightedScore();
        }
        os << "전체 가중 평균: " << avg / book.courses.size() << std::endl;
        return os;
    }
};

int main() {
    SetConsoleOutputCP(65001);

    ScholarshipGradeBook book;

    std::cout << "=== 1. 과목 성적 등록 및 중복 검사 ===" << std::endl;
    try {
        book.addCourse(new MajorCourse("CS201", "C++프로그래밍", 90, 3));      // 가중치: 99.0
        book.addCourse(new MajorCourse("CS202", "알고리즘", 85, 3));           // 가중치: 93.5
        book.addCourse(new ElectiveCourse("GE101", "실용영어", 95));           // 가중치: 95.0
        book.addCourse(new MajorCourse("CS203", "시스템프로그래밍", 80, 3));   // 가중치: 88.0
        std::cout << "-> 과목 등록 완료!\n" << std::endl;

        std::cout << "동일 코드(CS201) 중복 등록 시도..." << std::endl;
        book.addCourse(new MajorCourse("CS201", "고급C++", 100, 3));
    }
    catch (const GradeException& e) {
        std::cout << "[예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 2. 가중 성적 순 정렬 및 출력 ===" << std::endl;
    book.sortGrades();
    std::cout << book << std::endl;

    std::cout << "=== 3. 장학 사정 기준 검사 ===" << std::endl;
    try {
        double avg = book.getTopStudentScholarshipCut(90.0); // 기준: 평균 90점 이상
        std::cout << "-> 장학금 수혜 대상 확정! (가중 평균: " << avg << "점)" << std::endl;

        std::cout << "\n엄격한 기준(평균 98점) 사정 시도..." << std::endl;
        book.getTopStudentScholarshipCut(98.0);
    }
    catch (const GradeException& e) {
        std::cout << "[사정 탈락] " << e.what() << std::endl;
    }

    std::cout << "\n=== 4. 파일 영속화 리포트 출력 ===" << std::endl;
    try {
        book.exportReport("day67_grade_report.txt");
        std::cout << "-> 'day67_grade_report.txt' 파일 저장 완료!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[파일 에러] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}