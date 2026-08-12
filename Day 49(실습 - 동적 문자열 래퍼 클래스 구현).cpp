#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
#include <utility>
#include <windows.h>

// 오늘의 과제: 동적 문자열 래퍼 클래스 (String) 구현
// 동적 메모리를 관리하는 String 클래스를 직접 구현하면서 복사 / 이동의 차이를 검증해보세요.

class String {
private:
    char* data;
    size_t length;

public:
    // 일반 생성자
    String(const char* str = "") { // 문자열을 받아
        length = std::strlen(str); // 문자열의 길이를 알아내
        data = new char[length + 1]; // 동적 할당
        std::strcpy(data, str); // 이후 입력받은 문자열을 클래스 멤버인 동적 배열에 복사
        std::cout << "[생성자] \"" << data << "\" 메모리 할당" << std::endl;
    }

    // 소멸자
    ~String() {
        if (data) {
            std::cout << "[소멸자] \"" << data << "\" 메모리 해제" << std::endl;
            delete[] data;
        }
        else { // 이미 nullptr일 경우
            std::cout << "[소멸자] nullptr 메모리 해제 (무시됨)" << std::endl;
        }
    }

    // 깊은 복사를 수행하는 복사 생성자 구현
    String(const String& other) { 
        length = other.length; // 크기를 받아와
        data = new char[length + 1]; // 동적 할당
        for (int i = 0; i < length + 1; i++) { data[i] = other.data[i]; } // 복사 진행
    }

    // 포인터 소유권을 훔쳐오는 이동 생성자(Move Constructor) 구현
    String(String&& other) noexcept { 
        data = other.data; // other의 주소를 그대로 받아옴
        other.data = nullptr; // other의 연결을 끊어버림
        length = other.length; // other의 length 정보도 가져옴
    }

    // 이동 대입 연산자(Move Assignment Operator) 구현
    String& operator=(String&& other) noexcept { 
        if (this == &other) { return *this; } // 만약 자기 자신을 이동 대입하려 한다면 자기 자신을 반환
        delete[] data; // 기존 갖고 있던 데이터를 해제
        data = other.data; // 이후 이동 생성자처럼 받아오기.
        other.data = nullptr;
        length = other.length;

        return *this; // 이후 연속 대입이 가능하도록 자기 자신의 참조자를 반환
    }

    void print() const {
        if (data) std::cout << data << std::endl;
        else std::cout << "(empty)" << std::endl;
    }
};

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. 깊은 복사 생성자 테스트 ===" << std::endl;
    {
        String s1("Hello");
        String s2 = s1; // 복사 생성자 호출되어야 함 (Double Free 안 터져야 성공!)
        std::cout << "s1: "; s1.print();
        std::cout << "s2: "; s2.print();
    }

    std::cout << "\n=== 2. 이동 생성자 (std::move) 테스트 ===" << std::endl;
    {
        String s3("World");
        String s4 = std::move(s3); // 이동 생성자 호출! s3의 소유권이 s4로 이동
        std::cout << "s3 (원본): "; s3.print(); // (empty) 출력되어야 함
        std::cout << "s4 (이동됨): "; s4.print(); // World 출력되어야 함
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}