#include <iostream>
#include <windows.h> 
#include <string>
#include <stack>

// [문제] 괄호 검사 알고리즘
bool isValidParentheses(const std::string& brackets) {
	std::stack<char> test_stack; // char를 담는 stack 선언

	for (char c : brackets) { // 문자열 순회
		if (c == '[' || c == '{' || c == '(') { test_stack.push(c); } // 여는 괄호라면 push
		else { // 닫는 괄호의 경우
			if (test_stack.empty()) { return false; }
			switch (c) {
				case ')' : {
					if (test_stack.top() != '(') { return false; }  // 맨 위를 확인, 안 맞으면 false.
					test_stack.pop(); // 맞다면 들어가 있던 여는 괄호를 스택에서 빼낸다.
					break;
				}
				case '}': {
					if (test_stack.top() != '{') { return false; } 
					test_stack.pop(); 
					break;
				}
				case ']': {
					if (test_stack.top() != '[') { return false; } 
					test_stack.pop(); 
					break;
				}
				default: { return false; } // 괄호가 아닌 걸 만났다면 바로 false 반환.
			}
		}
	}
	return test_stack.empty(); // 남아있다면 자동으로 false 출력. 아니라면 true.
}

int main() {
	SetConsoleOutputCP(65001);
	// 테스트 케이스
	std::string brackets1 = "([])"; // 결과: true (올바른 괄호 쌍)
	std::string brackets2 = "([)]"; // 결과: false (올바르지 않은 괄호 쌍)
	std::string brackets3 = "(()";  // 결과: false (올바르지 않은 괄호 쌍)

	std::cout << "괄호 검사 결과:\n";
	std::cout << std::boolalpha; // 0과 1이 true, false로 출력
	std::cout << "괄호 쌍 1: " << isValidParentheses(brackets1) << "\n";
	std::cout << "괄호 쌍 2: " << isValidParentheses(brackets2) << "\n";
	std::cout << "괄호 쌍 3: " << isValidParentheses(brackets3) << "\n";
}