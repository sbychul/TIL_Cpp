#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <windows.h>

// 정규 커리큘럼 과제: STL 기반 할 일 관리기 (TaskManager)
// 정규 커리큘럼(제11강 STL 컨테이너/반복자/함수 객체 + 제14강 예외 처리)에 맞춘 표준 C++ 과제입니다.

// Task 구조체
struct Task {
    int id;
    std::string description;
    int priority; // 1(높음) ~ 3(낮음)
    bool isCompleted;

    // 생성자
    Task(int i, const std::string& desc, int p)
        : id(i), description(desc), priority(p), isCompleted(false) {
    }
};

// std::sort에 전달할 일반 비교 함수
// 우선순위를 기준으로 오름차순 정렬하기 위해 사용될 함수이다.
bool compareTaskPriority(const Task* a, const Task* b) {
    return a->priority < b->priority;
}

// 할 일 관리자 클래스
class TaskManager {
private:
    std::vector<Task*> tasks;
    int nextId;

public:
    TaskManager() : nextId(1) {}

    // 소멸자 구현
    ~TaskManager() {
        // 벡터 내의 동적 할당된 모든 task 객체를 지움.
        for (Task* task : tasks) { delete task; }
    }

    // Task를 벡터에 넣는 함수.
    void addTask(const std::string& desc, int priority) {
        tasks.push_back(new Task(nextId++, desc, priority));
    }

    // ID로 벡터에서 할 일 찾아 완료 처리하는 함수.
    void completeTask(int id) {
        for (Task* task : tasks) { // 일단 순회를 해
            if (task->id == id) { task->isCompleted = true; return; } // 완료시키고 함수 끝냄.
        }
        // 반복문 다 돌았는데 못 찾았다면 해당 ID에 대응되는 할 일이 없다는 뜻.
        throw std::runtime_error("해당 id에 해당하는 작업이 없습니다.");
    }

    // 벡터 내 모든 할 일을 정렬 후 출력하는 함수
    void printSortedByPriority() {
        // sort 함수에 만든 비교 함수를 집어넣어 우선순위 순으로 정렬.
        std::sort(tasks.begin(), tasks.end(), compareTaskPriority); 

        // 이후 순회하며 출력
        std::cout << "\n[할 일 목록 (우선순위 순)]\n";
        for (const Task* task : tasks) {
            std::cout << "ID: " << task->id
                << " | [" << (task->isCompleted ? "O" : "X") << "] "
                << task->description
                << " (우선순위: " << task->priority << ")\n";
        }
    }

    // 이터레이터를 순회하며 완료된 할 일 없애는 함수.
    void removeCompleted() {
        for (auto it = tasks.begin(); it != tasks.end(); ) {
            if ((*it)->isCompleted) {
                delete* it; // 동적 메모리 해제
                it = tasks.erase(it); // 벡터에서 제거 후 다음 유효 반복자 반환
            }
            else {
                ++it;
            }
        }
    }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    TaskManager manager;

    std::cout << "=== 1. 할 일 추가 ===" << std::endl;
    manager.addTask("C++ STL 복습하기", 2);
    manager.addTask("2학기 수강신청 확인", 1);
    manager.addTask("책상 정리하기", 3);
    manager.addTask("알고리즘 예습", 2);

    manager.printSortedByPriority();

    std::cout << "\n=== 2. 할 일 완료 처리 ===" << std::endl;
    try {
        manager.completeTask(1); // C++ 복습 완료
        manager.completeTask(3); // 책상 정리 완료
        manager.completeTask(99); // 없는 ID -> 예외 발생
    }
    catch (const std::runtime_error& e) {
        std::cout << "[예외 감지] " << e.what() << std::endl;
    }

    manager.printSortedByPriority();

    std::cout << "\n=== 3. 완료된 항목 일괄 삭제 ===" << std::endl;
    manager.removeCompleted();

    manager.printSortedByPriority();

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}