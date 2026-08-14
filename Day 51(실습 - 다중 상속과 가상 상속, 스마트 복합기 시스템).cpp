#include <iostream>
#include <vector>
#include <windows.h>

// Printer와 Scanner가 각각 Device를 상속받으면, 둘을 동시에 상속받은 MultiFunctionPrinter 내부에는 Device의 멤버 변수와 함수가 2개씩 중복 생성.
// 이 상태에서 mfp.getPowerUsage()나 mfp.id를 호출하면 컴파일러가 "어느 쪽 Device를 가리키는지 모호하다(Ambiguous)"며 컴파일 에러가 발생.

// 해결책: 가상 상속 (virtual public)
// 중간 클래스들이 상속을 받을 때 virtual 키워드를 붙여주면, 컴파일러가 최하위 자식 클래스에서 최상위 부모 객체를 단 하나만 공유하도록 메모리 구조를 재구성한다.

// 오늘의 과제: 스마트 복합기(MultiFunctionPrinter) 시스템

// 1. 최상위 부모 추상 클래스
class Device {
protected:
    int deviceId;
    int powerUsage;
public:
    Device(int id, int power) : deviceId(id), powerUsage(power) {
        std::cout << "[Device 생성] ID: " << deviceId << " (전력: " << powerUsage << "W)" << std::endl;
    }

    // 가상 소멸자. 
    // 부모 클래스 포인터로 자식 객체를 생성한 뒤 delete를 부를 때, 부모 클래스의 소멸자에 virtual이 붙어있지 않으면 자식 클래스의 소멸자가 호출되지 않는다.
    virtual ~Device() {
        std::cout << "[Device 소멸] ID: " << deviceId << std::endl;
    }

    // 순수 가상 함수, 자식 클래스가 오버라이드해야 할 함수.
    virtual void run() = 0;

    int getPowerUsage() const { return powerUsage; }
};

// 2. 중간 클래스 1: Printer (가상 상속 적용)
// Device를 virtual public으로 상속(가상 상속)받는다.
class Printer : virtual public Device {
public:
    Printer(int id, int power) : Device(id, power) {
        std::cout << "[Printer 파트 초기화]" << std::endl;
    }

    // 프린터의 고유 메서드
    void print() {
        std::cout << "-> [인쇄] 문서를 출력합니다." << std::endl;
    }
};

// 3. 중간 클래스 2: Scanner (가상 상속 적용)
// Device를 마찬가지로 virtual public으로 상속(가상 상속)받는다.
class Scanner : virtual public Device {
public:
    Scanner(int id, int power) : Device(id, power) {
        std::cout << "[Scanner 파트 초기화]" << std::endl;
    }

    // 스캐너의 고유 메서드
    void scan() {
        std::cout << "-> [스캔] 문서를 디지털로 스캔합니다." << std::endl;
    }
};

// 4. 최하위 다중 상속 클래스: MultiFunctionPrinter (프린트도 되고 스캔도 되는 복합기)
class MultiFunctionPrinter : public Printer, public Scanner {
public:
    // 가상 상속(virtual public)을 쓰면 가장 하위에 있는 클래스(MultiFunctionPrinter)가 최상위 부모(Device)의 생성자를 직접 호출하는 책임을 가지게 됨.
    // 중간 클래스들의 Device(...) 호출은 무시된다.
    // TODO: Device, Printer, Scanner 생성자를 적절히 초기화 목록에 작성하세요.
    MultiFunctionPrinter(int id, int power)
        // 입력받은 id와 소비 전력(power)를 부모 클래스의 생성자에 집어넣어 생성자를 호출.
        : Device(id, power), Printer(id, power), Scanner(id, power)
    {
        std::cout << "[MultiFunctionPrinter 복합기 완성]" << std::endl;
    }

    // TODO: 순수 가상 함수 run() 오버라이딩 (scan()과 print() 차례로 호출)
    void run() override {
        std::cout << "\n[복합기 작동 시작 - ID: " << deviceId << "]" << std::endl;
        scan();
        print();
    }
};

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. 다중/가상 상속 및 다이아몬드 문제 해결 테스트 ===" << std::endl;
    {
        // 최상위 부모 포인터로 복합기 객체 가리키기 (업캐스팅 & 동적 바인딩)
        // virtual 상속을 붙이지 않으면 여기서 [Device 생성]이 2번 출력되지만, 가상 상속을 적용하면 [Device 생성]이 1번만 출력된다.
        // 가상 상속이 아니라면 둘을 동시에 상속받은 MultiFunctionPrinter 내부에는 Device의 멤버 변수와 함수가 2개씩 중복 생성되기 때문이다.
        Device* myMFP = new MultiFunctionPrinter(101, 250);

        std::cout << "\n--- 다형성(Polymorphism)을 통한 run() 실행 ---" << std::endl;
        myMFP->run(); // virtual에 의해 MultiFunctionPrinter::run() 호출

        // 가상 상속 덕분에 Device 멤버 함수에 모호성 없이 직접 접근 가능
        std::cout << "\n소비 전력 확인: " << myMFP->getPowerUsage() << "W" << std::endl;

        std::cout << "\n--- 자원 해제 ---" << std::endl;
        delete myMFP; // 가상 소멸자로 전체 계층 안전 해제
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}