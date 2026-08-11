#include <iostream>
#include <memory>
#include <windows.h> 
#include <string>

// 오늘의 과제: 동적 정수 배열 관리자 (IntArrayManager) 구현
// 2학기 강의계획서의 제1~2주차(C++ 기초, 메모리 구조, C와 C++의 결정적 차이)를 기반으로 한 응용 과제,
// C++ 메모리 할당과 참조자의 동작 원리를 직접 검증하는 클래스를 구현해 보세요.

class IntArrayManager {
private:
    int* data;
    size_t length;
public:
    // 생성자, 크기를 받아 동적 할당.
    IntArrayManager(size_t size) : length(size) {
        data = new int[size](); 
        std::cout << "[생성자] 크기 " << length << "의 동적 배열 할당 완료" << std::endl;
    }

    // 소멸자
    ~IntArrayManager() {
        delete[] data; // delete로 동적 할당 해제
        std::cout << "[소멸자] 동적 배열 해제 완료" << std::endl;
    }

    // index 위치의 원소를 '참조자(int&)'로 반환하는 at 메서드
    int& at(size_t index) { 
        if (index >= length) { throw std::out_of_range("인덱스 범위 초과"); } // 인덱스 범위 초과 시 예외를 던짐
        return data[index];
    }

    // 배열의 크기를 반환하는 메서드
    size_t getSize() const { return length; }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. new/delete 및 참조자 반환 테스트 ===" << std::endl;
    {
        IntArrayManager arr(5);

        // 테스트: at()을 참조자로 반환받아 값 수정
        arr.at(0) = 10;
        arr.at(1) = 20;

        std::cout << "arr.at(0) 값: " << arr.at(0) << std::endl;
    } // <- 여기서 arr의 소멸자가 호출되어야 함

    std::cout << "\n=== 2. std::unique_ptr (RAII) 테스트 ===" << std::endl;
    {
        // std::make_unique를 사용해 IntArrayManager 객체 생성
        auto smartArr = std::make_unique<IntArrayManager>(3);
        smartArr->at(0) = 99;
        std::cout << "smartArr->at(0): " << smartArr->at(0) << std::endl;
    } // <- delete를 부르지 않아도 스마트 포인터에 의해 자동 해제

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}