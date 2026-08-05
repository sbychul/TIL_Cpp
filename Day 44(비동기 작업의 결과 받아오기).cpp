#include <iostream>
#include <thread>
#include <future>
#include <memory>
#include <chrono>
#include <windows.h>

// [서론] 비동기 작업의 결과를 받아오는 std::future<T>
// 기존의 Task 클래스는 작업 실행(process()) 후 결과를 반환하지 않았습니다. 하지만 실제 시스템에서는 작업의 반환값이나 예외(Exception)를 메인 스레드에서 수거해야 합니다.
// std::future<T>: 미래의 특정 시점에 설정될 값(T)을 가져올 수 있는 동기화 객체입니다.
// .get() : 결과가 준비될 때까지 대기(Wait)한 후 값을 반환합니다.

// 오늘의 과제: 리턴값이 있는 비동기 작업 처리기 구현하기
// 임의의 리턴값을 가지는 람다 함수를 전달받아 비동기로 실행하고, 결과를 std::future 형태로 반환하는 함수를 직접 구현해 보는 과제입니다.

template <typename F>
std::future<int> submitTask(F func) { // 정수를 반환하는 람다 함수를 인자로 받는 함수.
    // func를 실행할 packaged_task를 가리키는 스마트 포인터 task 지역 변수 생성
    // shared_ptr로 감싸면 람다 캡처가 수월하다, packaged_task는 복사 불가능하고 이동만 가능하기 때문.
    auto task = std::make_shared<std::packaged_task<int()>>(func);

    // task로부터 get_future를 사용하여 future 객체를 추출
    std::future<int> res = task->get_future();

    // 스레드를 생성하여 비동기로 (*task)() 실행 후 detach
    // [task]: 람다 함수의 캡처 록에 task를 넣어 해당 스마트 포인터를 참조.
    // 비동기 자원은 언제 끝날 지 모르기에 메인 스레드에서 함수가 종료되더라도 스레드 내부 람다가 task를 캡처하고 있어 메모리에서 사라지지 않게 된다.
    std::thread([task]() {
        (*task)(); // 해당 스마트 포인터를 역참조하여 전달받은 람다 연산 func를 실제로 진행, 반환값을 획득, 해당 값을 future 객체에 저장.
        }).detach(); // 해당 스레드의 소유권을 방출. 이미 결과를 future 객체가 갖고 있기에, detach하면 결과값을 받아오기 어려운 문제가 해결.

    // 결과를 갖고 있는 future 객체를 반환
    return res;
}

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== std::future 기반 비동기 작업 테스트 ===" << std::endl;

    // 1. 연산을 시뮬레이션하는 람다 함수 전달
    std::future<int> f1 = submitTask([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        return 10 + 20;
        });

    std::future<int> f2 = submitTask([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return 100 * 2;
        });

    std::cout << "메인 스레드는 비동기 작업 완료를 기다리는 중..." << std::endl;

    // .get()을 부르는 시점에 작업이 끝날 때까지 대기(Wait) 후 결과를 받아옴
    std::cout << "작업 1 결과: " << f1.get() << std::endl;
    std::cout << "작업 2 결과: " << f2.get() << std::endl;

    std::cout << "=== 모든 비동기 작업 완료 ===" << std::endl;
    return 0;
}