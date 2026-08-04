#include <iostream>
#include <string>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <windows.h>

// 오늘의 과제: 완성형 ThreadPool 클래스 작성하기
// 이전에 완성했던 Task 및 ThreadSafeQueue를 부품으로 활용하여, Worker 스레드들을 총괄 관리하는 ThreadPool 클래스를 직접 구현하거나 복습해 보세요.

// 이 두 개는 건드리지 않았음.
class Task {
private:
    int id;
    std::string description;
public:
    // 생성자
    Task(int task_id, std::string task_description) : id{ task_id }, description{ task_description } {}
    // 작업 id와 설명을 출력
    void process() const { std::cout << "작업명: " << id << " / 상세: " << description << std::endl; }
};
class ThreadSafeQueue {
private:
    std::queue<std::unique_ptr<Task>> taskQueue;
    mutable std::mutex mtx;

    // 42일차 추가 멤버 변수
    // 조건 변수(condition variable):
    // 스레드가 특정 조건이 만족될 때까지 대기(Wait) 상태로 기다리게 만들거나, 다른 스레드에게 조건이 갖춰졌음을 알리는(Notify) 동기화 도구
    std::condition_variable cv; 
    // 큐 종료 여부 플래그
    bool isShutdown = false; 
public:
    void push(std::unique_ptr<Task> task) {
        // 41일차와 똑같은 원리이다. 뮤텍스 잠그고 move를 통해 유니크 포인터 소유권 이전.
        // 대신 unique_lock을 사용. 조건 변수가 원할 때 명시적으로 unlock() 및 lock()을 수행할 수 있어 제어 능력이 뛰어나다.
        // 조건 변수와 함께 사용할 땐 내부적으로 잠금을 풀고 다시 잡는 게 필요하므로 반드시 unique_lock을 사용해야 한다.
        std::unique_lock<std::mutex> lock(mtx);
        taskQueue.push(std::move(task));

        // 42일차 추가본
        // 작업 추가 후 대기 중인 Consumer 스레드 하나를 깨운다.
        cv.notify_one();
    }

    std::unique_ptr<Task> pop() {
        // 잠그고
        std::unique_lock<std::mutex> lock(mtx);
        
        // cv.wait(lock, predicate): lock - 락 객체 / predicate - 조건 검사 함수(람다 또는 함수 포인터), 해당 조건이 true가 될 때까지 대기 상태를 유지.
        
        // 스레드를 대기 상태로 만들고,
        // wait 함수가 호출되는 순간 뮤텍스의 잠금이 해제,
        // ThreadSafeQueue의 멤버 클래스에 접근하기 위해 this 포인터를 캡처([this]),
        // 큐가 비어 있지 않거나, 종료 플래그가 true라면 스레드 작동 개시.
        cv.wait(lock, [this] { return !taskQueue.empty() || isShutdown; });

        // 스레드가 작동하는데, 큐가 비어 있고 꺼져 있다면 nullptr 반환.
        if (taskQueue.empty() && isShutdown) { return nullptr; }

        // 아니라면 나머지는 41일차에 작성했던 그대로 큐에서 꺼낸다.
        std::unique_ptr<Task> temp = std::move(taskQueue.front());
        taskQueue.pop();
        return temp;
    }

    // 42일차 추가 함수. 큐를 꺼버린다.
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mtx); // 잠그고
            isShutdown = true; // 꺼버리고
        }
        cv.notify_all(); // 대기 중인 모든 컨슈머 스레드를 깨워 종료 수순을 밟도록 함. 
    }

    // 41일차와 똑같음. 비어 있는지 확인
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return taskQueue.empty();
    }
};

// 43일차 메인, ThreadPool 클래스 구현
class ThreadPool {
private:
    ThreadSafeQueue taskQueue; // 스레드 안전 큐 자체
    std::vector<std::thread> workers; // 스레드를 담는 벡터

public:
    // 생성자: numThreads 개수만큼 워커 스레드를 생성하여 바로 실행 상태로 만듦
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            // 스레드가 실행할 함수를 [this]포인터를 이용하여 전달
            workers.emplace_back([this] {
                // 할 일이 없을 때까지 돌아야 하므로 무한 루프
                while (true) {
                    auto task = taskQueue.pop(); // 하나씩 작업을 큐에서 꺼내
                    if (!task) { break; } // 작업이 nullptr이라면 탈출 (!task랑 task == nullptr은 같은 표현)
                    task->process(); // 아니면 진행시켜
                }
            });
        }
    }

    // 작업 추가
    void enqueueTask(std::unique_ptr<Task> task) {
        taskQueue.push(std::move(task)); // 추가, 유니크 포인터이기에 소유권 이전
    }

    // 소멸자: 큐 shutdown 및 모든 스레드 join
    ~ThreadPool() {
        taskQueue.shutdown(); // 끄고
        // 스레드 벡터를 순회하며 각각의 스레드를 join한다.
        for (std::thread& worker : workers) { worker.join(); }
    }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 스레드 풀(ThreadPool) 테스트 시작 ===" << std::endl;

    {
        // 3개의 일꾼 스레드를 가진 스레드 풀 생성
        ThreadPool pool(3);

        // 10개의 작업을 스레드 풀에 던짐
        for (int i = 1; i <= 10; ++i) {
            pool.enqueueTask(std::make_unique<Task>(i, "대량 데이터 처리 작업 " + std::to_string(i)));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "--- main 함수 내 블록 종료 시점 (ThreadPool 소멸자 호출 예정) ---" << std::endl;
    } // <- 여기서 pool 객체가 소멸되며 소멸자가 모든 스레드를 안전하게 정리합니다.

    std::cout << "=== 스레드 풀이 안전하게 파괴되었습니다 ===" << std::endl;
    return 0;
}