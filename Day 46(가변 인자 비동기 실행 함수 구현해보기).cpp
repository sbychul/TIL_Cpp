#include <iostream>
#include <thread>
#include <future>
#include <memory>
#include <functional>
#include <string>
#include <chrono>
#include <windows.h>

// 오늘의 과제: 가변 인자 함수 포장기 구현해 보기
// 오늘은 스레드 풀에 적용하기 전, 임의의 함수와 N개의 인자를 받아서 인자 없는 람다 / 함수로 만들어 비동기 실행하는 포장 함수를 직접 작성해 보는 과제입니다.

// 일반 함수 예시
int add(int a, int b) {
    return a + b;
}

void printMessage(const std::string& msg, int count) {
    for (int i = 0; i < count; ++i) {
        std::cout << "[Print] " << msg << std::endl;
    }
}

// 가변 인자 템플릿 (typename... Args)
// 인자의 개수와 타입이 정해지지 않은 템플릿. ...(파라미터 팩)을 사용하여 N개의 인자를 한 번에 받아냄.
template <typename F, typename... Args>
auto submitGenericTask(F&& f, Args&&... args) {
    // std::bind를 사용해 함수 f와 인자 args...를 하나로 묶음
    // f와 거기에 들어갈 인자 args...를 하나의 호출 가능한(Callable) 틀로 묶어주는 역할.
    auto boundTask = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    // std::forward<Args>(args)... :
    // 전달받은 인자의 타입을 손실 없이 다음 함수에 전달하는 기법.
    // 이동 전용 객체(유니크 포인터 등)나 거대한 객체를 효율적으로 전달할 때 필수이다.

    // 리턴 타입을 추론하여 packaged_task 생성 (std::invoke_result_t 활용)
    // std::invoke_result_t: 어떤 함수 또는 람다에 특정 인자들을 넘겨 실행했을 때, 반환되는 최종 리턴 타입이 무엇인지를 컴파일 시점에 알아내는 도구.
    using return_type = std::invoke_result_t<F, Args...>; // 스레드 풀의 함수에서 뭘 리턴할지 모르니까 std::invoke_result_t를 활용한다.
    auto task = std::make_shared<std::packaged_task<return_type()>>(boundTask);

    // future 객체를 추출
    std::future<return_type> res = task->get_future();

    // 스레드 생성 후 비동기 실행 및 detach
    std::thread([task]() { 
        (*task)();
        }).detach();

    // future 반환
    return res;
}

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 범용 가변 인자 task 제출 테스트 ===" << std::endl;

    // 1. 인자 2개를 받는 일반 함수 add 전달
    std::future<int> f1 = submitGenericTask(add, 10, 20);

    // 2. 인자 2개를 받는 일반 함수 printMessage 전달 (리턴타입 void)
    std::future<void> f2 = submitGenericTask(printMessage, "C++ Generic Thread Pool", 2);

    // 3. 인자 3개를 받는 람다 함수 전달
    std::future<int> f3 = submitGenericTask([](int a, int b, int c) {
        return a * b * c;
        }, 2, 3, 4);

    f2.get(); // void 작업 완료 대기
    std::cout << "add(10, 20) 결과: " << f1.get() << std::endl;
    std::cout << "람다 곱셈 결과: " << f3.get() << std::endl;

    return 0;
}