#include <iostream>
#include <windows.h> 
#include <string> // string 객체 사용을 위한 헤더 
#include <cctype> // toupper 함수를 사용하기 위한 헤더

// [문제] 모음 제거 및 대문자 변환기
// 사용자로부터 공백이 없는 하나의 영어 단어(문자열)를 입력받습니다.
// 입력받은 단어에서** 모음(a, e, i, o, u, A, E, I, O, U)** 을 모두 제거합니다.
// 모음이 제거되고 남은 알파벳 자음들은 모두 대문자로 변환하여 출력하세요.

int main() {
	SetConsoleOutputCP(65001);
	std::string sentence{}, new_sentence{};

	std::cout << "문자열을 입력하세요: ";
	std::getline(std::cin, sentence); // 한 문장을 입력받음.

	std::cout << "변환 결과: ";
	for (char c : sentence) {
		c = std::toupper(c);
		if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') { continue; }
		else { std::cout << c; }
	}
	
	std::cout << std::endl;
}