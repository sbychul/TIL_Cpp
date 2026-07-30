#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <windows.h>

// 오늘의 통합 과제: 스레드 안전한 스마트 작업 큐 (Thread-Safe Task Queue)
// 여러 작업(Task) 생성자 스레드가 큐에 작업을 추가하고, 여러 작업 처리자 스레드가 큐에서 작업을 꺼내어 처리하는 스레드 안전한 작업 큐(Thread - Safe Task Queue) 시스템을 구현해 보세요.
// 이전에 배운 스마트 포인터(std::unique_ptr), 람다 함수, 예외 처리, 그리고 오늘의 std::mutex& std::lock_guard를 모두 결합해 봅니다.

class Task {
private:
	int id;
	std::string description;
public:
	// 생성자
	Task(int task_id, std::string task_description) : id{ task_id }, description{ task_description } {}
	// 작업 id와 설명을 출력
	void process() const { std::cout << "작업명: " << id << " / 상세: " << description; }
};

class ThreadSafeQueue {
private:
	std::queue<std::unique_ptr<Task>> taskQueue;
	// std::mutex: 임계 영역(Critical Section)을 만들어 한 번에 하나의 스레드만 자원에 접근하도록 잠금(Lock)을 거는 키워드.
	mutable std::mutex mtx; // 읽기 전용 함수에서도 잠금을 사용할 수 있도록 mutable 키워드를 사용
public:
	void push(std::unique_ptr<Task> task) {
		// 스코프 안에서 뮤텍스를 잠그고
		std::lock_guard<std::mutex> lock(mtx);
		// 유니크 포인터기 때문에 std::move로 넣기
		taskQueue.push(std::move(task));
		// 함수가 끝나면 lock 객체가 자동으로 소멸.
	}

	std::unique_ptr<Task> pop() {
		// 잠그고
		std::lock_guard<std::mutex> lock(mtx);
		// 비어 있는지 확인
		if (taskQueue.empty()) { return nullptr; }

		std::unique_ptr<Task> temp = std::move(taskQueue.front()); // move를 활용하여 임시 포인터에 이동
		taskQueue.pop(); // 빠져나간 빈 칸 없애기

		return temp; // 이후 빼낸 데이터를 반환
	}

	// 비어 있는지 확인
	bool empty() const {
		std::lock_guard<std::mutex> lock(mtx);
		return taskQueue.empty();
	}
};

// 테스트 케이스
// 테스트용 작업 1: 작업 생성자 스레드 함수
void producer(ThreadSafeQueue& q, int producerId) {
    for (int i = 1; i <= 3; ++i) {
        int taskId = producerId * 100 + i;
        std::string desc = "Producer_" + std::to_string(producerId) + "의 작업 " + std::to_string(i);

        q.push(std::make_unique<Task>(taskId, desc));
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 스레드 간 교대 유도
    }
}

// 테스트용 작업 2: 작업 처리자 스레드 함수
void consumer(ThreadSafeQueue& q, int consumerId) {
    for (int i = 0; i < 3; ++i) {
        auto task = q.pop();
        if (task) {
            std::cout << "[Consumer " << consumerId << " 처리 중] ";
            task->process();
        }
        else {
            std::cout << "[Consumer " << consumerId << "] 처리할 작업이 없습니다." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
}

// main 함수로 테스트
int main() {
    SetConsoleOutputCP(65001);
    ThreadSafeQueue queue;

    std::cout << "=== 멀티스레드 스마트 작업 큐 테스트 시작 ===" << std::endl;

    // 2개의 Producer와 2개의 Consumer 스레드 생성
    std::thread p1(producer, std::ref(queue), 1);
    std::thread p2(producer, std::ref(queue), 2);
    std::thread c1(consumer, std::ref(queue), 1);
    std::thread c2(consumer, std::ref(queue), 2);

    // 모든 스레드가 종료될 때까지 메인 스레드 대기
    p1.join();
    p2.join();
    c1.join();
    c2.join();

    std::cout << "\n=== 모든 스레드 작업 완료 및 프로그램 종료 ===" << std::endl;
    return 0;
}