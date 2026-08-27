#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 후위 표기식 수식 계산기 (ExpressionEvaluator)
// 강의계획서의 제8강(연산자 중복), 제10강(문자열 스트림 입출력), 제11강(템플릿과 STL 컨테이너),
// 제14강(예외 처리)을 결합한 "제네릭 스택 기반 후위 표기식 수식 계산기 (ExpressionEvaluator<T>)"입니다.
// 동적 배열 기반의 템플릿 스택 자료구조, 문자열 스트림(std::stringstream) 기반의 토큰 분리, 연산자 처리 및 0으로 나누기 예외 처리,
// 그리고 동적 메모리 소멸자 클린업을 다룹니다.

// 템플릿 기반 동적 스택: Stack<T>
template <typename T>
class Stack {
private:
    T* data;
    size_t capacity; // 최대 용량
    size_t topIndex; // 현재 저장된 원소 개수

public:
    // 생성자, 크기 받아 초기화
    Stack(size_t cap = 20) : capacity(cap), topIndex(0) {
        data = new T[capacity];
    }

    // 소멸자
    ~Stack() {
        delete[] data;
    }

    // push 메서드
    void push(const T& item) {
        // 용량이 가득 찼다면 예외 처리
        if (topIndex == capacity) { throw std::overflow_error("스택이 가득 찼습니다."); }
        data[topIndex++] = item; // 아니라면 topIndex값의 인덱스 위치에 값을 넣고 topIndex를 1 늘림
    }

    // pop 메서드
    void pop() {
        // 비어 있으면 빼면 안 됩니다, 예외 처리
        if (empty()) { throw std::underflow_error("스택이 비어 있습니다."); }
        topIndex--; // topIndex를 하나 줄여버림 (topIndex는 흐름 상 현재 들어가 있는 요소의 개수를 의미)
    }

    // top 메서드
    T& top() {
        // 비어 있으면 참조할 게 없으니 예외 처리
        if (empty()) { throw std::underflow_error("스택이 비어 있습니다."); }
        return data[topIndex - 1]; // 맨 위에 있는 값을 참조 반환
    }

    // 비어 있는지 확인하는 메서드와 크기 반환 메서드
    bool empty() const { return topIndex == 0; }
    size_t size() const { return topIndex; }
};

// 후위 표기식 수식 계산기: ExpressionEvaluator
class ExpressionEvaluator {
public:
    // 후위 표기식(공백 구분) 계산 메서드
    // - std::stringstream으로 토큰 파싱 (while (ss >> token))
    // - 토큰이 연산자("+", "-", "*", "/")인 경우:
    //     * 스택에서 op2, op1 순서로 pop
    //     * "/" 연산 시 op2 == 0 이면 std::invalid_argument throw
    //     * 연산 결과를 스택에 push
    // - 토큰이 숫자인 경우:
    //     * std::stod(token) 변환 후 스택에 push
    // - 수식 계산 완료 후 최종 결과값(double) 반환
    static double evaluate(const std::string& expression) {
        Stack<double> stack; // 빈 스택 선언
        // stringstream: 공백으로 구분된 문자열을 >> 연산자를 통해 연속적인 토큰(단어) 단위로 추출하는 자료형.
        std::stringstream ss(expression); // 수식을 받아 토큰 파싱할 객체.
        std::string token; // 각 토큰을 담을 빈 문자열
        std::string op1, op2; // 피연산자를 담을 빈 문자열

        while (ss >> token) { // 토큰을 다 빼낼 때까지 반복
            // 토큰이 연산자라면
            if (token == "+" || token == "-" || token == "*" || token == "/") {
                // 스택에서 먼저 뺀 값이 뒤쪽 피연산자이다. 따라서 op2 값을 먼저 뺌.
                double op2 = stack.top(); stack.pop();
                double op1 = stack.top(); stack.pop();

                // 피연산자의 길이가 1임을 활용하여 token[0]을 넘김.
                switch (token[0]) {
                case '+':
                    stack.push(op1 + op2);
                    break;
                case '-':
                    stack.push(op1 - op2);
                    break;
                case '*':
                    stack.push(op1 * op2);
                    break;
                case '/':
                    // 0으로는 나눌 수 없으니 예외를 던짐.
                    if (op2 == 0.0) { throw std::invalid_argument("0으로 나눌 수 없습니다."); }
                    stack.push(op1 / op2);
                    break;
                }
            }
            // 피연산자라면
            else { stack.push(std::stod(token)); }
        }
        return stack.top(); // 스택에 마지막까지 남아 있는 게 연산 결과.
    }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. 정상 후위 표기식 계산 테스트 ===" << std::endl;
    try {
        // (10 + 20) * 3 = 90
        std::string exp1 = "10 20 + 3 *";
        std::cout << exp1 << " = " << ExpressionEvaluator::evaluate(exp1) << " (기대값: 90)" << std::endl;

        // 15 - (4 * 2) = 7
        std::string exp2 = "15 4 2 * -";
        std::cout << exp2 << " = " << ExpressionEvaluator::evaluate(exp2) << " (기대값: 7)" << std::endl;

        // 50 / (5 + 5) = 5
        std::string exp3 = "50 5 5 + /";
        std::cout << exp3 << " = " << ExpressionEvaluator::evaluate(exp3) << " (기대값: 5)" << std::endl;

    }
    catch (const std::exception& e) {
        std::cout << "[계산 에러] " << e.what() << std::endl;
    }

    std::cout << "\n=== 2. 예외 처리 테스트 (0으로 나누기) ===" << std::endl;
    try {
        // 10 / 0
        std::string expZero = "10 0 /";
        std::cout << expZero << " 계산 시도..." << std::endl;
        ExpressionEvaluator::evaluate(expZero);

    }
    catch (const std::invalid_argument& e) {
        std::cout << "[0 나누기 예외 감지] " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[기타 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}