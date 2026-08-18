#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <windows.h>

// 종합 과제 2탄: 스마트 미디어 보관함 (MediaLibrary)
// 난이도를 '중상'으로 살짝 끌어올린 종합 실기 과제 2탄입니다.
// 이번에는 C++ 객체지향 설계의 핵심인 대입 연산자 깊은 복사(Copy Assignment Operator), 객체 슬라이싱 방지(Object Slicing),
// 템플릿 기반 순회 반복 연산자 오버로딩([]), 사용자 정의 예외 클래스 상속을 결합한 종합 과제입니다.
// 추상 미디어 클래스를 바탕으로 동적 문자열을 관리하는 파생 클래스를 정의하고, 이를 안전하게 관리하는 템플릿 보관함 클래스를 완성하세요.

// 1. 사용자 정의 예외 클래스
// std::exception을 상속받아 what()을 오버라이딩하는 CustomEmptyException
class CustomEmptyException : public std::exception {
public:
    const char* what() const noexcept override {
        return "보관함이 비어 있습니다.";
    }
};

// 2. 최상위 추상 클래스: Media
class Media {
public:
    virtual ~Media() {}
    virtual void play() const = 0;
};

// 3. 파생 클래스: Video
class Video : public Media {
private:
    char* title;
    int duration; // 재생 시간 (초)

public:
    Video(const char* t, int d) : duration(d) {
        title = new char[strlen(t) + 1];
        strcpy(title, t);
    }

    ~Video() override {
        delete[] title;
    }

    // 복사 생성자
    Video(const Video& other) : duration(other.duration) {
        title = new char[strlen(other.title) + 1];
        strcpy(title, other.title);
    }

    // 깊은 복사를 수행하는 복사 대입 연산자(operator=) 구현
    Video& operator=(const Video& other) {
        if (this == &other) { return *this; } // 자기 대입 검사
        duration = other.duration;
        delete[] title; // 기존 메모리 해제

        // 이후는 복사 생성자와 방식이 같으며, 연속 대입을 위해 *this를 반환한다.
        title = new char[strlen(other.title) + 1];
        strcpy(title, other.title);
        return *this;
    }

    // play() 오버라이딩 (예: "[재생] Title (120초)")
    void play() const override {
        std::cout << "[재생] " << title << " (" << duration << ")초" << std::endl;
    }
};

// 4. 템플릿 보관함: MediaContainer<T>
template <typename T>
class MediaContainer {
private:
    T** data;
    size_t capacity;
    size_t count;

public:
    MediaContainer(size_t cap = 10) : capacity(cap), count(0) {
        data = new T * [capacity];
    }

    ~MediaContainer() {
        for (size_t i = 0; i < count; ++i) {
            delete data[i];
        }
        delete[] data;
    }

    void add(T* item) {
        if (count >= capacity) {
            throw std::overflow_error("보관함 용량이 초과되었습니다.");
        }
        data[count++] = item;
    }

    // 마지막 원소를 delete하고 제거하는 removeLast() 구현
    void removeLast() {
        // 비어 있는 경우 CustomEmptyException throw
        if (count == 0) { throw CustomEmptyException(); }
        delete data[--count]; // 아니면 그냥 없애준다.
    }

    // operator[] 오버로딩 (경계 검사 후 data[index] 반환)
    T* operator[](size_t index) const {
        // index가 count 이상이면 std::out_of_range throw
        if (index >= count) { throw std::out_of_range("입력된 인덱스가 현재 보관된 갯수를 넘어섰습니다."); }
        return data[index];
    }

    size_t size() const { return count; }
};

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. 복사 대입 연산자(operator=) 검증 ===" << std::endl;
    {
        Video v1("Movie A", 7200);
        Video v2("Temp", 100);
        v2 = v1; // 복사 대입 연산자 호출
        v2.play();
    }

    std::cout << "\n=== 2. MediaContainer 다형성 & [] 연산자 테스트 ===" << std::endl;
    try {
        MediaContainer<Media> library(3);

        library.add(new Video("Lecture 01", 3600));
        library.add(new Video("Lecture 02", 4200));

        // operator[]를 통한 순회 및 다형성 호출
        for (size_t i = 0; i < library.size(); ++i) {
            library[i]->play();
        }

        std::cout << "\n--- removeLast 및 CustomEmptyException 테스트 ---" << std::endl;
        library.removeLast(); // Lecture 02 삭제
        library.removeLast(); // Lecture 01 삭제
        library.removeLast(); // 💡 여기서 비어있으므로 CustomEmptyException 발생해야 함

    }
    catch (const CustomEmptyException& e) {
        std::cout << "[사용자 예외 감지] " << e.what() << std::endl;
    }
    catch (const std::out_of_range& e) {
        std::cout << "[인덱스 예외 감지] " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[기타 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}