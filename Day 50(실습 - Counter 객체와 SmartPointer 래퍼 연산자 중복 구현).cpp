#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>

// 오늘의 과제: Counter 객체와 SmartPointer 래퍼 연산자 중복 구현

class Counter {
private:
    int value; // 값을 저장하는 멤버 변수
    static int totalCount; // static 멤버 변수 선언, 생성된 Counter 객체 수를 추적하는 변수
public:
    // 값을 받아 카운터 객체에 저장.
    Counter(int val = 0) : value(val) {
        totalCount++; // 총 카운터 객체 수를 나타내는 변수의 값을 1 늘림.
        std::cout << "[Counter 생성] 현재 전체 객체 수: " << totalCount << std::endl;
    }

    // 복사 생성자 (TODO 2에서 복사될 때도 totalCount가 증가하도록 하기 위함)
    Counter(const Counter& other) : value(other.value) {
        totalCount++;
    }

    // 소멸자
    ~Counter() {
        totalCount--;
        std::cout << "[Counter 소멸] 현재 전체 객체 수: " << totalCount << std::endl;
    }

    // getter
    int getValue() const { return value; }

    // static 멤버 함수
    static int getTotalCount() { return totalCount; }

    // TODO 1: 전위 증감 연산자 (++c) 구현 -> 자기 자신의 참조자(Counter&) 반환
    Counter& operator++() {
        value++; // 값을 1 올리고
        return *this; // 객체 자기 자신의 참조자를 반환
    }

    // TODO 2: 후위 증감 연산자 (c++) 구현 -> int 더미 인자 필요, 변경 전 복사본 반환
    Counter operator++(int) { 
        Counter temp = *this; // 변경 전 복사본을 미리 복사.
        value++; // 진짜 값은 1 증가시킴.
        return temp; // 외부에 반환하는 건 변경 전 객체.
    }

    // TODO 3: friend 키워드를 사용한 << 연산자 중복 선언
    friend std::ostream& operator<<(std::ostream& os, const Counter& c);
};

// static 멤버 변수 외부 정의 및 초기화 (필수!)
int Counter::totalCount = 0;

// TODO 3 구현: << 연산자 전역 함수
std::ostream& operator<<(std::ostream& os, const Counter& c) {
    os << "Counter(" << c.value << ")";
    return os;
}


// 포인터 연산자 중복을 위한 간단한 스마트 포인터 클래스
class SmartPointer {
private:
    Counter* ptr;

public:
    SmartPointer(Counter* p = nullptr) : ptr(p) {}
    ~SmartPointer() { delete ptr; }

    // TODO 4: operator-> 중복 (내부 포인터 ptr 반환하여 멤버 접근 허용)
    Counter* operator->() const { return ptr; }

    // TODO 5: operator* 중복 (역참조 *ptr 객체 자신 반환)
    Counter& operator*() const { return *ptr; }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. static 멤버 및 증감 연산자 테스트 ===" << std::endl;
    {
        Counter c1(10);
        std::cout << "초기 c1: " << c1 << std::endl;

        std::cout << "전위 ++c1: " << ++c1 << std::endl; // 11
        std::cout << "후위 c1++: " << c1++ << std::endl; // 11 출력 후 12가 됨
        std::cout << "후위 연산 후 c1: " << c1 << std::endl; // 12
    }

    std::cout << "\n=== 2. SmartPointer 및 연산자(->, *) 테스트 ===" << std::endl;
    {
        SmartPointer sp(new Counter(100));

        // operator-> 호출: Counter의 getValue() 직접 호출
        std::cout << "sp->getValue(): " << sp->getValue() << std::endl;

        // operator* 호출: Counter 객체 자체를 역참조해서 << 연산자로 출력
        std::cout << "*sp 출력: " << *sp << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}