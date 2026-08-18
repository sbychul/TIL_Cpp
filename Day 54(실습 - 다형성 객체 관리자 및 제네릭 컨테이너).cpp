#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 다형성 객체 관리자 및 제네릭 보관함 (Item, Book, Inventory<T>)
// 강의계획서의 제1~14주차 전체 핵심(클래스, 동적 메모리, 복사 생성자, 연산자 중복, 상속, 가상 함수/소멸자, 템플릿, 예외 처리)을
// 하나의 통합 시나리오로 엮은 학부 C++ 기말 프로젝트/종합 실기 스타일 과제입니다.

// 1. 최상위 추상 클래스: Item
class Item {
public:
    // 가상 소멸자 선언
    virtual ~Item() {}

    // 순수 가상 함수 show() 선언, 하위 클래스가 오버라이드해야 함.
    virtual void show() const = 0;

    // <<(삽입) 연산자 오버로딩 (출력 스트림에서 show()가 호출되도록 구현)
    friend std::ostream& operator<<(std::ostream& os, const Item& item) {
        item.show();
        return os;
    }
};

// 2. 파생 클래스: Book (동적 메모리 + 깊은 복사 생성자)
class Book : public Item {
private:
    char* title;
    int price;

public:
    Book(const char* t, int p) : price(p) {
        title = new char[strlen(t) + 1];
        strcpy(title, t);
    }

    // 소멸자
    ~Book() override {
        delete[] title;
    }

    // 깊은 복사를 수행하는 복사 생성자 구현
    Book(const Book& other) : price(other.price) { // 가격은 바로 받아옴.
        title = new char[strlen(other.title) + 1]; // 새롭게 동적 할당
        for (int i = 0; i <= strlen(other.title); i++) { // 내용물 복사 진행
            title[i] = other.title[i];
        }
    }

    // show() 가상 함수 오버라이딩 (예: [도서] 제목 | 가격: X원)
    void show() const override {
        std::cout << "[도서] " << title << " | 가격: " << price << "원";
    }
};

// 3. 템플릿 인벤토리: Inventory<T>
template <typename T>
class Inventory {
private:
    T** items;          // 포인터 배열 동적 할당
    size_t capacity;
    size_t count;

public:
    Inventory(size_t cap = 5) : capacity(cap), count(0) {
        items = new T * [capacity];
    }

    // TODO 6: 소멸자 구현 (보관 중인 모든 객체 delete 및 배열 delete[] 해제)
    ~Inventory() {
        for (int i = 0; i < count; i++) {
            delete items[i]; // 배열을 순회하며 객체 배열의 내용물을 delete
        }
        delete[] items; // 이후 배열 자체도 delete
    }

    // TODO 7: push 구현 (가득 찼으면 std::overflow_error throw)
    void push(T* item) {
        if (count == capacity) { throw std::overflow_error("인벤토리가 가득 찼습니다."); }
        items[count++] = item; // 현재 count가 위치하고 있는 곳에 item을 삽입.
    }

    // TODO 8: at 구현 (범위 초과 시 std::out_of_range throw, T* 반환)
    T* at(size_t index) const {
        if (index >= count) { throw std::out_of_range("지정한 인덱스가 현재 저장된 갯수를 초과하였습니다."); }
        return items[index];
    }

    size_t size() const { return count; }
};

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. 다형성 + 템플릿 인벤토리 종합 테스트 ===" << std::endl;
    try {
        Inventory<Item> itemStorage(2); // 최대 2개 보관

        // 동적 객체 생성 및 다형성 보관
        itemStorage.push(new Book("C++ Primer", 45000));
        itemStorage.push(new Book("Effective C++", 38000));

        std::cout << "--- 보관 아이템 출력 (연산자 중복 활용) ---" << std::endl;
        for (size_t i = 0; i < itemStorage.size(); ++i) {
            std::cout << *itemStorage.at(i) << std::endl;
        }

        // 예외 테스트: 용량 초과 삽입
        std::cout << "\n--- 용량 초과 예외 테스트 ---" << std::endl;
        itemStorage.push(new Book("Overflow Book", 10000));

    }
    catch (const std::overflow_error& e) {
        std::cout << "[오버플로우 예외 발생] " << e.what() << std::endl;
    }
    catch (const std::out_of_range& e) {
        std::cout << "[범위 초과 예외 발생] " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[기타 예외 발생] " << e.what() << std::endl;
    }

    std::cout << "\n=== 2. 깊은 복사 생성자 검증 ===" << std::endl;
    {
        Book b1("Modern C++", 30000);
        Book b2 = b1; // 깊은 복사 호출 -> 메모리 충돌 없이 안전해야 함
        std::cout << "b1: " << b1 << std::endl;
        std::cout << "b2: " << b2 << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}