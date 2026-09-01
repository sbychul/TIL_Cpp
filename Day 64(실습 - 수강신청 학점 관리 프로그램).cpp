#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <windows.h>

// 오늘의 실습 과제: 수강신청 학점 관리기 (CourseManager)
// 오늘의 과제는 2학기 강의계획서의 제4강(객체 포인터와 객체 배열), 제8강(연산자 중복: operator+=, operator<<),
// 제11강(STL std::vector)을 활용한 "수강신청 학점 및 과목 관리 시스템 (CourseManager)"입니다.

// 각 과목을 나타낼 클래스
class Course {
private:
    std::string courseName; // 강의명
    int credits; // 학점, 기본값 3
    std::string professorName; // 교수명
public:
    // 생성자, 과목명, 교수명과 학점을 입력받아 초기화
    Course(std::string name, std::string professor, int credit = 3) : courseName{ name }, professorName{ professor }, credits{ credit } {}

    // Getter
    std::string getCourseName() const { return courseName; }
    int getCredits() const { return credits; }
    std::string getProfessorName() const { return professorName; }
};

// 수강 관리 클래스
class CourseManager {
private:
    std::vector<Course> courseList; // 등록된 과목 목록
    int maxCredits; // 최대 수강 가능 학점, 기본값 19
    int currentCredits; // 현재 신청된 총 학점
public:
    // 생성자, 최대 학점을 받아 초기화
    CourseManager(int max = 19) : maxCredits{ max }, currentCredits{ 0 } {}

    // 대입 연산자 오버로딩 (과목 등록 용도)
    CourseManager& operator+=(const Course& course) {
        if (currentCredits + course.getCredits() > maxCredits) { // 최대 학점 초과 시
            throw std::runtime_error("최대 허용 학점을 초과할 수 없습니다.");
        }
        courseList.push_back(course); // 목록에 넣고
        currentCredits += course.getCredits(); // 현재 학점 누적
        return *this; // 연속 대입을 위하여 본인 포인터 반환
    }

    // 삽입 연산자 오버로딩, 현재 등록된 모든 과목 정보와 총 신청 학점/최대 학점을 출력
    friend std::ostream& operator<<(std::ostream& os, const CourseManager& cm) {
        os << "[수강신청 내역]\n";
        int i = 1; // 몇 번째 과목인지 표시하기 위함
        for (Course course : cm.courseList) { // 순회하며 출력
            os << i++ << ". " << course.getCourseName() << " (" << course.getCredits() << "학점) - 담당: " << course.getProfessorName() << std::endl;
        }
        os << "총 신청 학점: " << cm.currentCredits << "/" << cm.maxCredits << "학점" << std::endl;
        return os;
    }

    // 과목 신청 취소 함수
    bool removeCourse(const std::string& name) {
        // 이터레이터로 순회하며 과목명이 같은 게 있는지 찾음.
        for (auto it = courseList.begin(); it != courseList.end(); it++) {
            // 찾았다면 지우고 지우기 성공했다는 true 반환
            if (it->getCourseName() == name) {
                currentCredits -= it->getCredits();
                courseList.erase(it);
                return true;
            }
        }
        return false; // 반복문을 끝까지 다 돌았다면 못 찾았다
    }
};

int main() {
    SetConsoleOutputCP(65001);

    CourseManager manager(19); // 최대 19학점 신청 가능

    std::cout << "=== 1. 정상 수강신청 (+= 연산자 활용) ===" << std::endl;
    try {
        // 실제 본인 수강신청 내역... 교수명만 미공개
        manager += Course("C++", "김교수", 2);
        manager += Course("알고리즘", "이교수", 3);
        manager += Course("시스템프로그래밍", "박교수", 3);
        manager += Course("컴퓨터구조", "최교수", 3);
        manager += Course("모바일소프트웨어", "정교수", 3);
        manager += Course("교양 과목", "남교수", 3);
        manager += Course("영어회화", "강교수", 1); // 현재 총 18학점
        std::cout << "-> 과목 등록 완료!\n" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[등록 에러] " << e.what() << std::endl;
    }

    std::cout << manager << std::endl;

    std::cout << "=== 2. 학점 초과 예외 테스트 ===" << std::endl;
    try {
        // 18학점 상태에서 3학점 추가 시도 -> 21학점으로 19학점 초과
        std::cout << "추가 과목(데이터베이스, 3학점) 신청 시도..." << std::endl;
        manager += Course("데이터베이스", "한교수", 3);
    }
    catch (const std::runtime_error& e) {
        std::cout << "[예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 3. 과목 취소 테스트 ===" << std::endl;
    if (manager.removeCourse("영어회화")) {
        std::cout << "-> '영어회화' 취소 완료 (1학점 차감)\n" << std::endl;
    }

    std::cout << manager << std::endl;

    return 0;
}