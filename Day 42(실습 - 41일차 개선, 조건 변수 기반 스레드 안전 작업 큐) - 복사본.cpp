#include <iostream>
#include <string>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <windows.h>

// 오늘의 통합 과제: 조건 변수 기반 스레드 안전 작업 큐
// 지난 시간에 작성한 ThreadSafeQueue를 개선하여, 작업이 없을 때 Consumer 스레드가 대기 상태에 들어가고 Producer가 작업을 넣었을 때 깨어나도록 구현해 보세요.
// 또한 모든 작업이 완료된 후 프로그램이 안전하게 종료될 수 있도록 큐 종료 메커니즘(shutdown)도 추가해 봅니다.

// 41일차와 똑같다. 볼 필요 없음.
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

// 테스트 케이스
// Producer 스레드: 100ms마다 작업 추가
void producer(ThreadSafeQueue& q, int producerId) {
    for (int i = 1; i <= 3; ++i) {
        int taskId = producerId * 100 + i;
        std::string desc = "Producer_" + std::to_string(producerId) + "의 작업 " + std::to_string(i);

        q.push(std::make_unique<Task>(taskId, desc));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Consumer 스레드: 작업이 들어오면 대기 해제 후 처리
void consumer(ThreadSafeQueue& q, int consumerId) {
    while (true) {
        auto task = q.pop();
        if (!task) {
            // shutdown 신호를 받고 큐가 비어있는 경우 반복 종료
            std::cout << "[Consumer " << consumerId << "] 종료 신호 수신. 쓰레드를 종료합니다." << std::endl;
            break;
        }
        std::cout << "[Consumer " << consumerId << " 처리 완료] ";
        task->process();
    }
}

int main() {
    SetConsoleOutputCP(65001);
    ThreadSafeQueue queue;

    std::cout << "=== 조건 변수 기반 생산자-소비자 큐 테스트 시작 ===" << std::endl;

    // Consumer 스레드를 먼저 실행 (작업이 없으므로 대기 상태로 들어감)
    std::thread c1(consumer, std::ref(queue), 1);
    std::thread c2(consumer, std::ref(queue), 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Producer 스레드 실행
    std::thread p1(producer, std::ref(queue), 1);
    std::thread p2(producer, std::ref(queue), 2);

    // Producer 스레드 종료 대기
    p1.join();
    p2.join();

    // 생산 완료 후 큐 종료 처리 및 대기 스레드 전체 깨우기
    std::cout << "\n--- 모든 생산 작업 완료. 큐 종료(shutdown) 요청 ---" << std::endl;
    queue.shutdown();

    // Consumer 스레드 종료 대기
    c1.join();
    c2.join();

    std::cout << "=== 모든 스레드가 안전하게 종료되었습니다 ===" << std::endl;
    return 0;
}