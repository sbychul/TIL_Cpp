#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <windows.h>

// 오늘의 실습 과제: 스마트 출결 관리기 (AttendanceManager)
// 강의계획서의 제6강(상속과 기반 클래스), 제7강(가상 함수와 다형성), 제11강(STL std::vector),
// 제14강(예외 처리)을 결합한 "스마트 캠퍼스 출결 관리 시스템 (AttendanceManager)"입니다.
// 캠퍼스 내 구성원(학생, 교수)의 신원 정보를 기반 클래스로 추상화하고, 구성원별로 다르게 동작하는 출석 체크 로직을 다형성으로 구현합니다.
// 출결 관리 클래스는 다양한 구성원 객체를 포인터 벡터로 일괄 관리하며, 전자 태그를 태그할 때의 출결 처리 및 예외 상황을 처리합니다.

// 기반이 될 추상 클래스
class Person {
private:
    std::string id; // 학번 또는 교번
    std::string name; // 이름
public:
    // 생성자
    Person(std::string personId, std::string personName) : id{ personId }, name{ personName } {}
    // 가상 소멸자 선언
    virtual ~Person() = default;
    // 카드 태그 시 동작할 순수 가상 함수
    virtual void tagCard() = 0;
    // 정보 출력 가상 함수
    virtual void printInfo() const = 0;

    // Getter
    std::string getId() const { return id; }
    std::string getName() const { return name; }
};

// 하위 클래스 1. 학생
class Student : public Person {
private: int attendanceCount; // 추가 멤버 변수: 출석 횟수, 초기값 0
public:
    // 생성자
    Student(std::string studentId, std::string studentName) : attendanceCount{ 0 }, Person(studentId, studentName) {}
    // 학생: 출석 횟수를 1 늘리고 메시지를 출력
    void tagCard() override {
        attendanceCount++; 
        std::cout << "[학생] " << getName() << "(학번: " << getId() << ") 출석 체크 완료. (현재 출석: " << attendanceCount << "회)" << std::endl;
    }
    // 정보 출력 함수 오버라이딩
    void printInfo() const override {
        std::cout << "[학생] " << getName() << " (학번: " << getId() << ") / (출석 횟수: " << attendanceCount << "회)" << std::endl;
    }
};

// 하위 클래스 2. 교수
class Professor : public Person {
private: std::string labRoom; // 추가 멤버 변수: 연구실 번호
public:
    // 생성자
    Professor(std::string profId, std::string profName, std::string labRoomNum) : labRoom{ labRoomNum }, Person(profId, profName) {}
    // 교수: 연구동 출입 인증 메시지 출력
    void tagCard() override {
        std::cout << "[교수] " << getName() << "교수님 (교번: " << getId() << ") 연구동 출입 인증 완료. (연구실: " << labRoom << ")" << std::endl;
    }
    // 정보 출력 함수 오버라이딩
    void printInfo() const override {
        std::cout << "[교수] " << getName() << " (교번: " << getId() << ") / (연구실: " << labRoom << ")" << std::endl;
    }
};

// 출결 관리 메인 클래스
class AttendanceManager {
private: std::vector<Person*> memberList; // 멤버들의 포인터를 갖는 벡터
public:
    // 생성자와 소멸자
    AttendanceManager() = default;
    ~AttendanceManager() { for (Person* p : memberList) { delete p; } }

    // 추가 함수
    void registerPerson(Person* p) { memberList.push_back(p); }
    // id를 검색하여 일치하는 객체의 tagCard 호출
    void swipeCard(const std::string& id) {
        for (Person* p : memberList) { 
            if (p->getId() == id) { p->tagCard(); return; }
        }
        // 반복문을 다 돌았는데도 return을 안 만났기에 해당 id가 없다는 것.
        throw std::runtime_error("등록되지 않은 사용자입니다.");
    }
    // 등록된 모든 구성원의 printInfo() 호출하는 함수
    void printAllMembers() const {
        for (Person* p : memberList) { p->printInfo(); }
    }
};

int main() {
    SetConsoleOutputCP(65001);

    AttendanceManager manager;

    std::cout << "=== 1. 구성원 등록 ===" << std::endl;
    manager.registerPerson(new Student("2024001", "홍길동"));
    manager.registerPerson(new Student("2024002", "김영희"));
    manager.registerPerson(new Professor("P101", "이교수", "5호관 301호"));
    std::cout << "-> 총 3명의 구성원이 등록되었습니다.\n" << std::endl;

    std::cout << "=== 2. 카드 태그(출결/출입) 테스트 ===" << std::endl;
    try {
        manager.swipeCard("2024001"); // 홍길동 출석 (1회)
        manager.swipeCard("2024001"); // 홍길동 출석 (2회)
        manager.swipeCard("P101");    // 이교수 출입

        std::cout << "\n미등록 카드 태그 시도..." << std::endl;
        manager.swipeCard("9999999"); // 미등록 -> 예외 발생
    }
    catch (const std::runtime_error& e) {
        std::cout << "[인증 실패] " << e.what() << std::endl;
    }

    std::cout << "\n=== 3. 전체 구성원 현황 출력 ===" << std::endl;
    manager.printAllMembers();

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}