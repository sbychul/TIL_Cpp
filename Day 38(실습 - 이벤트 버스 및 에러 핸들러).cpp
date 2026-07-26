#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <windows.h>

// 오늘의 통합 과제: 스마트 이벤트 버스 (Event Bus) 및 에러 핸들러
// 다양한 이벤트(알림 메세지, 센서 데이터 등)를 등록받아 처리하고,
// 잘못된 이벤트나 처리 실패 시 C++ 표준 예외(std::exception)를 발생시켜 안전하게 복구하는 비동기형 스타일 이벤트 관리자 클래스를 구현해 보세요.


// std::runtime_error를 상속받는 두 가지 커스텀 예외 클래스
class EventNotFoundException : public std::runtime_error {
public:
	// 생성자를 통해 예외 메시지를 임의로 정하도록 할 수 있으며(안 정하면 기본값), 부모 클래스의 생성자를 호출하여 메시지를 전달한다.
	EventNotFoundException(const std::string& message = "이벤트를 찾을 수 없습니다") : std::runtime_error(message) {}
};

class EventExecutionException : public std::runtime_error {
public:
	EventExecutionException(const std::string& message = "이벤트 실행 중 오류가 발생했습니다.") : std::runtime_error(message) {}
};

class IEvent {
public:
    // 이벤트 실행 가상 함수와 가상 소멸자 선언
    virtual void execute() = 0;
    virtual ~IEvent() = default;
};

// 템플릿 파생 클래스
template <typename T>
class ConcreteEvent : public IEvent {
private:
    T data; // 이벤트에 전달할 데이터
    std::function<void(const T&)> handler; // 이벤트를 처리할 람다/함수 포인터, 반환값은 없고, const T& 타입 인자 하나를 받는 함수를 담음.
public:
    // 생성자, 전달받은 데이터와 람다를 멤버 변수에 저장
    ConcreteEvent(T data_input, std::function<void(const T&)> handler_input) : data{ data_input }, handler{ handler_input } {}
    void execute() override {
        try { handler(data); } // 보관해둔 람다 함수에 데이터를 넣어 실행.
        catch (...) { throw EventExecutionException("이벤트 처리 중 오류 발생"); } // 에러 발생 시 커스텀 예외로 감싸 throw
    }
};

// 이벤트 매니저 클래스
class EventBus {
private:
    std::vector<IEvent*> eventQueue; // 등록된 이벤트 객체들의 포인터를 순서대로 보관하는 큐
public:
    // 외부에서 데이터와 핸들러를 던져 주면, 해당 데이터 타입에 맞는 ConcreteEvent 객체를 생성하여 큐에 추가하는 템플릿 함수
    // 왜 템플릿 함수일까요? ConcreteEvent 객체도 아무거나 들어갈 수 있는 거기 때문이겠죠?
    template <typename T>
    void registerEvent(T data, std::function<void(const T&)> handler) {
        IEvent* newEvent = new ConcreteEvent<T>(data, handler);
        eventQueue.push_back(newEvent);
    }

    // 맨 앞의 이벤트를 실행하고 메모리를 해제하는 함수
    void processNext() {
        // 만약 eventQueue가 비어있다면 이벤트가 없으니 예외를 던집시다.
        if (eventQueue.empty()) { throw EventNotFoundException("실행할 이벤트가 없습니다."); }

        // 맨 앞의 이벤트를 가져와 실행
        IEvent* currentEvent = eventQueue.front();
        currentEvent->execute();

        // 메모리 해제 및 이벤트 큐에서 제거
        delete currentEvent;
        eventQueue.erase(eventQueue.begin()); // vector의 맨 앞에 있는 것을 erase.
    }

    // 소멸자, 남아있는 모든 이벤트 메모리를 해제함.
    ~EventBus() {
        for (IEvent* event : eventQueue) { delete event; } // 이벤트 객체 포인터를 활용하여 벡터를 순회 후 할당 해제.
        eventQueue.clear(); // 벡터까지 클리어해주기.
    }
};


// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);
    EventBus bus;

    // 1. 람다 함수와 템플릿을 활용한 다양한 이벤트 등록
    bus.registerEvent<std::string>("시스템 점검이 10분 뒤 시작됩니다.", [](const std::string& msg) {
        std::cout << "📢 [알림 이벤트]: " << msg << std::endl;
        });

    bus.registerEvent<int>(404, [](const int& errorCode) {
        std::cout << "🚨 [에러 이벤트]: HTTP 에러 코드 " << errorCode << " 발생!" << std::endl;
        });

    // 2. 이벤트 정상 처리 테스트
    std::cout << "=== 이벤트 처리 시작 ===" << std::endl;
    try {
        bus.processNext(); // 첫 번째 이벤트 실행
        bus.processNext(); // 두 번째 이벤트 실행
    }
    catch (const std::exception& e) {
        std::cout << "예외 발생: " << e.what() << std::endl;
    }

    // 3. 빈 큐에서 처리 시도 (예외 발생 테스트)
    std::cout << "\n=== 예외 발생 테스트 ===" << std::endl;
    try {
        bus.processNext(); // 비어있으므로 EventNotFoundException 발생해야 함
    }
    catch (const std::exception& e) {
        std::cout << "💡 성공적으로 예외 포착: " << e.what() << std::endl;
    }

    return 0;
}