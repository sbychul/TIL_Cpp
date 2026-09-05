#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <windows.h>

// 오늘의 실습 과제: 제네릭 수강 편람 보관소 (GenericCatalog<T>)
// 학기 중 수강 편람 정보나 교재 목록 등 다양한 데이터를 공통적인 인터페이스로 관리할 수 있는 제네릭 컨테이너 GenericCatalog<T>를 만듭니다.
// 인덱스 연산자([])를 오버로딩하여 안전한 데이터 접근을 지원하고, 스트림 출력(<<)을 통해 전체 목록을 서식에 맞게 출력하도록 설계합니다.

// 과목 정보 클래스
class Subject {
private:
    std::string code; // 과목코드
    std::string title; // 과목명
public:
    // 생성자
    Subject(std::string subjectCode, std::string subjectTitle) : code{ subjectCode }, title{ subjectTitle } {}
    // Getter
    std::string getCode() const { return code; }
    std::string getTitle() const { return title; }

    // 삽입 연산자 오버로딩
    friend std::ostream& operator<<(std::ostream& os, const Subject& s) {
        os << "[과목 코드: " << s.code << "] " << s.title;
        return os;
    }
};

// 컨테이너 클래스
template <typename T>
class GenericCatalog {
private:
    std::vector<T> items; // 보관할 벡터
public:
    GenericCatalog() = default;

    // 추가 함수
    void addItem(T item) { items.push_back(item); }
    // 크기 반환 함수
    size_t size() const { return items.size(); }
    // 벡터 비우기 함수
    void clear() { items.clear(); }

    // 배열 접근 연산자 오버로딩
    T& operator[](size_t index) {
        if (index > items.size()) { throw std::out_of_range("잘못된 인덱스 접근입니다."); }
        return items[index];
    }
    // 읽기 전용 오버로딩
    const T& operator[](size_t index) const {
        if (index = items.size()) { throw std::out_of_range("잘못된 인덱스 접근입니다."); }
        return items[index];
    }
    // 삽입 연산자 오버로딩
    friend std::ostream& operator<<(std::ostream& os, GenericCatalog gc) {
        os << "[편람 목록]" << std::endl;
        int i = 1;
        for (T item : gc.items) { os << i++ << ". " << item << std::endl; }
        return os;
    }
};

int main() {
    SetConsoleOutputCP(65001);

    // 1. Subject 타입을 담는 제네릭 카탈로그 생성
    GenericCatalog<Subject> subjectCatalog;

    subjectCatalog.addItem(Subject("CS201", "C++프로그래밍"));
    subjectCatalog.addItem(Subject("CS202", "알고리즘"));
    subjectCatalog.addItem(Subject("CS203", "시스템프로그래밍"));

    std::cout << "=== 1. 편람 목록 출력 (operator<<) ===" << std::endl;
    std::cout << subjectCatalog << std::endl;

    std::cout << "=== 2. 인덱스 연산자([]), 정상 접근 테스트 ===" << std::endl;
    std::cout << "0번 인덱스 과목: " << subjectCatalog[0] << std::endl;
    std::cout << "2번 인덱스 과목: " << subjectCatalog[2] << std::endl;

    std::cout << "\n=== 3. 인덱스 초과 예외 테스트 ===" << std::endl;
    try {
        std::cout << "10번 인덱스 과목 조회 시도..." << std::endl;
        std::cout << subjectCatalog[10] << std::endl; // 예외 발생
    }
    catch (const std::out_of_range& e) {
        std::cout << "[범위 초과 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 4. 기본 타입(std::string) 템플릿 테스트 ===" << std::endl;
    GenericCatalog<std::string> memoCatalog;
    memoCatalog.addItem("학사일정 확인");
    memoCatalog.addItem("중간고사 대비 스터디");
    std::cout << memoCatalog << std::endl;

    std::cout << "=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}