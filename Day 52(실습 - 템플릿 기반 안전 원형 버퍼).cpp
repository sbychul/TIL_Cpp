#include <iostream>
#include <stdexcept>
#include <string>
#include <windows.h>

// 오늘의 과제: 템플릿 기반 안전 원형 버퍼 (SafeRingBuffer<T, Capacity>) 구현
// 2학기 강의계획서의 제11강(템플릿과 STL 라이브러리)과 제14강(예외 처리, try-catch-throw)의 실전 결합 과제,
// 템플릿의 타입 제약이나 버퍼 제어를 직접 다루는 고정 크기 원형 큐(Circular Queue) 템플릿 클래스 (SafeRingBuffer<T, N>)를 구현해 봅니다.

// 데이터 타입과 정수형 상수를 템플릿 인자로 받음.
template <typename T, size_t Capacity>
class SafeRingBuffer {
private:
    T buffer[Capacity];
    size_t head = 0;
    size_t tail = 0;
    size_t count = 0;

public:
    SafeRingBuffer() = default;

    bool empty() const { return count == 0; }


    bool full() const { return count == Capacity; }
    size_t size() const { return count; }
    size_t capacity() const { return Capacity; }

    // TODO 1: push 구현 (가득 차 있으면 std::overflow_error throw)
    void push(const T& item) {
        // 가득 차 있으면 예외를 던짐
        if (count == Capacity) { throw std::overflow_error("버퍼가 가득 찼습니다.");  }
        buffer[tail] = item; // 아니라면 tail 위치에 item을 넣고
        tail = (tail + 1) % Capacity; // 원형 버퍼이기 때문에 나머지 연산을 이용하여 tail 포인터 변수(라고 해야 하나)를 이동.
        count++; // count를 1 증가
    }

    // TODO 2: pop 구현 (비어 있으면 std::underflow_error throw)
    void pop() {
        // 비어 있으면 예외를 던짐.
        if (count == 0) { throw std::underflow_error("버퍼가 비어 있습니다."); }
        head = (head + 1) % Capacity; // 아니라면 더이상 접근할 수 없도록 head 포인터 변수를 이동.
        count--; // count를 1 줄임.
    }

    // TODO 3: front 구현 (비어 있으면 std::underflow_error throw, 아니라면 const T& 반환)
    const T& front() const {
        // 비어 있으면 예외를 던짐.
        if (count == 0) { throw std::underflow_error("버퍼가 비어 있습니다."); }
        return buffer[head]; // 아니라면 head 포인터 변수가 가리키고 있는 값을 반환.
    }
};

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. SafeRingBuffer 기본 동작 및 int 테스트 ===" << std::endl;
    try {
        SafeRingBuffer<int, 3> rb;
        rb.push(10);
        rb.push(20);
        rb.push(30);

        std::cout << "현재 size: " << rb.size() << " / capacity: " << rb.capacity() << std::endl;
        std::cout << "front 값: " << rb.front() << std::endl; // 10

        rb.pop(); // 10 제거
        rb.push(40); // 원형 순환 삽입 (20, 30, 40)

        while (!rb.empty()) {
            std::cout << "pop: " << rb.front() << std::endl;
            rb.pop();
        }

        // 언더플로우 유발 테스트
        std::cout << "\n--- 언더플로우 예외 테스트 ---" << std::endl;
        rb.pop();

    }
    catch (const std::underflow_error& e) {
        std::cout << "[언더플로우 예외 감지] " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[기타 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 2. std::string 타입 및 오버플로우 테스트 ===" << std::endl;
    try {
        SafeRingBuffer<std::string, 2> strRb;
        strRb.push("First");
        strRb.push("Second");

        std::cout << "오버플로우 직전 strRb.size(): " << strRb.size() << std::endl;

        // 오버플로우 유발
        strRb.push("Third (Overflow!)");

    }
    catch (const std::overflow_error& e) {
        std::cout << "[오버플로우 예외 감지] " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[기타 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}