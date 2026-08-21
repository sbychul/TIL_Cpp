#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 다형성 결제 관리자 (PaymentManager) 구현
// 오늘의 종합 과제는 학부 2학기 강의계획서의 제6강(상속과 객체 포인터), 제7강(가상 함수와 다형성),
// 제11강(템플릿과 STL), 제14강(예외 처리)을 아우르는 "다형성 기반 스마트 결제/거래 관리 시스템 (PaymentSystem)"입니다.
// 기본 클래스 상속과 순수 가상 함수, std::vector를 활용한 업캐스팅 객체 포인터 관리, 동적 형변환 검사(dynamic_cast),
// 그리고 잔액 부족 / 한도 초과 시 발생하는 예외 처리를 다루는 과제입니다.

// 1. 최상위 추상 클래스: Payment
class Payment {
protected:
    std::string name;

public:
    Payment(const std::string& n) : name(n) {}
    // 가상 소멸자 선언
    virtual ~Payment() {}

    // 순수 가상 함수 pay(), printReceipt() 선언
    virtual void pay(int amount) = 0;
    virtual void printReceipt() const = 0;

    std::string getName() const { return name; }
};

// 2. 자식 클래스: CreditCard (신용카드)
class CreditCard : public Payment {
private:
    int cardLimit;          // 한도
    int accumulatedAmount;  // 현재까지 사용한 누적 금액

public:
    // 생성자
    CreditCard(const std::string& n, int limit)
        : Payment(n), cardLimit(limit), accumulatedAmount(0) {
    }

    // pay() 오버라이딩
    void pay(int amount) override {
        // 한도 초과 시 예외 던지기
        if (accumulatedAmount + amount > cardLimit) { throw std::runtime_error("카드 한도가 초과되었습니다."); }
        accumulatedAmount += amount; // 정상 결제 시 금액 누적
    }

    // printReceipt() 오버라이딩
    // "[신용카드: (카드명)] 누적 승인: XX원 / 잔여 한도: XX원" 형태로 출력.
    void printReceipt() const override {
        std::cout << "[신용카드: " << name << "] 누적 승인: " << accumulatedAmount <<
            "원 / 잔여 한도: " << cardLimit - accumulatedAmount << "원" << std::endl;
    }
};

// 3. 자식 클래스 2: Cash (현금)
class Cash : public Payment {
private:
    int balance; // 보유 잔액

public:
    // 생성자
    Cash(const std::string& n, int initialBalance)
        : Payment(n), balance(initialBalance) {
    }

    // pay() 오버라이딩
    void pay(int amount) override {
        // 돈 모자랄 시 예외 처리
        if (amount > balance) { throw std::runtime_error("현금 잔액이 부족합니다."); }
        balance -= amount; // 정상 결제 시 잔액에서 차감
    }

    // printReceipt() 오버라이딩
    // "[현금: (이름)] 잔액: X원" 형태로 출력.
    void printReceipt() const override {
        std::cout << "[현금: " << name << "] 잔액: " << balance << "원" << std::endl;
    }
};

// 4. 결제 관리자 클래스: PaymentManager
class PaymentManager {
private:
    std::vector<Payment*> methods;

public:
    PaymentManager() = default;

    // 소멸자
    ~PaymentManager() {
        for (Payment* method : methods) { delete method; } // 순회하며 삭제
    }

    void addMethod(Payment* p) {
        methods.push_back(p);
    }

    // 모든 등록 수단으로 동일 금액(amount) 결제 시도
    void processAll(int amount) {
        for (Payment* method : methods) {
            // 벡터를 순회하며 결제 호출
            try {
                method->pay(amount);
                method->printReceipt(); // 성공 시 영수증
            }
            // 실패 시 catch
            catch (const std::runtime_error& e) {
                std::cout << "결제 과정에서 문제가 발생하였습니다: " << e.what() << std::endl;
            }
        }
    }
};

int main() {
    SetConsoleOutputCP(65001);

    PaymentManager manager;

    // 결제 수단 등록 (다형성 업캐스팅)
    manager.addMethod(new CreditCard("신한카드", 50000));
    manager.addMethod(new Cash("지갑 현금", 30000));

    std::cout << "=== 1차 결제 진행 (20,000원 결제) ===" << std::endl;
    try {
        manager.processAll(20000);
    }
    catch (const std::runtime_error& e) {
        std::cout << "[결제 실패] " << e.what() << std::endl;
    }

    std::cout << "\n=== 2차 결제 진행 (15,000원 추가 결제 -> 현금 잔액 부족 유발) ===" << std::endl;
    try {
        manager.processAll(15000);
    }
    catch (const std::runtime_error& e) {
        std::cout << "[결제 실패] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}