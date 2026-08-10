#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <type_traits>
#include <string>
#include <chrono>
#include <windows.h>

// 스레드 최종 과제: 범용 ThreadPool 클래스 완성하기
// 스켈레톤 코드를 바탕으로, 그동안 배운 가변 인자 템플릿과 std::packaged_task가 결합된 최종 ThreadPool의 동작을 확인하고 완성해 보세요.

class ThreadPool {
private:
    std::vector<std::thread> workers; // 워커 스레드를 담는 벡터
    // void() 규격 작업 큐, 범용 스레드 풀은 어떤 리턴 값/인자를 가진 함수든 다 받아야 하기 때문에 void() 규격을 사용한다.
    std::queue<std::function<void()>> taskQueue; 
    std::mutex mtx; // 뮤텍스
    std::condition_variable cv; // 조건 변수
    bool isShutdown = false; // 작업 큐 종료 여부 플래그

public:
    // 생성자: numThreads 개수만큼 워커 스레드 생성
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] { // 스레드가 실행할 함수를 this 포인터를 이용하여 전달.
                while (true) {
                    // std::function<void()>는 인자도 없고 리턴값도 없는 함수를 담는 상자. 실행 시 task()하면 끝.
                    // 상자 하나를 만든다.
                    std::function<void()> task; 
                    {
                        std::unique_lock<std::mutex> lock(this->mtx); // 뮤텍스 잠금
                        // 큐가 비어 있지 않거나, 종료 플래그가 true라면(꺼져 있다면) 스레드 작동 개시.
                        this->cv.wait(lock, [this] { 
                            return this->isShutdown || !this->taskQueue.empty();
                            });

                        // 큐가 꺼져 있거나 큐가 비어 있다면 스레드 종료.
                        if (this->isShutdown && this->taskQueue.empty()) {
                            return;
                        }

                        task = std::move(this->taskQueue.front()); // move를 통해 큐의 맨 앞 작업(함수)을 상자에 담음(소유권 이전)
                        this->taskQueue.pop(); // 그리고 큐에서는 없앤다.
                    }
                    task(); // 포장된 void() 람다 실행
                }
                });
        }
    }

    // 오늘 메인 메뉴(TODO): 가변 인자 enqueueTask 구현하기
    template <typename F, typename... Args>
    auto enqueueTask(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using return_type = std::invoke_result_t<F, Args...>;

        // std::bind와 std::packaged_task를 사용해 task 생성
        // std::bind를 사용해 함수 f와 인자 args...를 하나로 묶음.
        auto boundTask = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // 리턴 타입을 추론하여 packaged_task를 생성.
        auto task = std::make_shared<std::packaged_task<return_type()>>(boundTask);

        // task로부터 future 추출, 외부로 돌려줄 future를 미리 추출해 둔다.
        std::future<return_type> res = task->get_future();

        // 뮤텍스 잠금 후 taskQueue에 [task]() { (*task)(); } 형태의 람다 추가
        {
            std::unique_lock<std::mutex> lock(mtx); // 뮤텍스 잠금
            if (isShutdown) { throw std::runtime_error("종료된 ThreadPool에는 작업을 추가할 수 없습니다."); } // 꺼져 있다면 예외를 던져 람다 추가 X
            taskQueue.emplace([task]() { (*task)(); }); // 꺼져 있지 않다면 람다를 추가.
        }

        // cv.notify_one() 호출 후 res 반환
        cv.notify_one();
        return res;
    }

    // 소멸자
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            isShutdown = true;
        }
        cv.notify_all();

        for (std::thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

// 테스트 케이스
int add(int a, int b) { return a + b; }

int main() {
    SetConsoleOutputCP(65001);

    {
        ThreadPool pool(3);

        auto f1 = pool.enqueueTask(add, 10, 20);
        auto f2 = pool.enqueueTask([](std::string name) {
            return "Hello " + name;
            }, "World");

        std::cout << "f1 결과: " << f1.get() << std::endl; // 30
        std::cout << "f2 결과: " << f2.get() << std::endl; // Hello World
    }

    return 0;
}