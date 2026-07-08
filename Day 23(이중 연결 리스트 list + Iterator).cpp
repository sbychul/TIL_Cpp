#include <iostream>
#include <windows.h> 
#include <string>
#include <list> // 리스트 사용을 위한 헤더

// Iterator (이터레이터, 반복자)란?
// 리스트는 메모리가 흩어져 있어 포인터 연산이나 인덱스 사용이 불가.
// 그러나 이를 일반 포인터처럼 주소를 가리키고 이동할 수 있도록 하는 도구.
// l.begin() : 리스트의 첫 요소를 가리킴 / l.end() : 리스트의 맨 마지막 요소 다음 빈 공간을 가리킴 (끝을 판정하는 벽 역할)
// ++it, --it : 앞뒤로 한 칸씩 이동 / *it : 역참조(가리키는 값 추출)

// [문제] 텍스트 에디터 커서 시뮬레이터
// std::list와 이터레이터를 활용해 텍스트 에디터의 커서(Cursor) 움직임을 모방하고
// 원하는 위치에 요소를 중간 삽입/삭제하는 커서 시뮬레이션 알고리즘 문제
int main() {
	SetConsoleOutputCP(65001);
	std::list<char> editor; // 문자를 저장하는 리스트 생성

	// 두 글자를 차례대로 삽입
	editor.push_back('T');
	editor.push_back('1');

	// auto 키워드를 사용하여 std::list<char>::iterator 라는 타입명을 생략.
	auto cursor = editor.end(); // 커서가 맨 뒤에 있음을 의미

	// 시나리오 흐름대로 코드가 차례대로 실행되도록 작성한 부분
	// 1. 커서를 왼쪽으로 이동 (커서가 1을 가리키도록)
	if (cursor != editor.begin()) { --cursor; }

	// 2. 커서 위치에 문자 'X' 삽입
	editor.insert(cursor, 'X');

	// 3. 커서를 왼쪽으로 또 한번 이동
	if (cursor != editor.begin()) { --cursor; }

	// 4. 커서의 오른쪽에 위치한 문자 삭제
	// erase 함수는 삭제된 요소의 다음 iterator를 반환하기에,
	// 반드시 이전 iterator 변수에 다시 대입해 주어야 한다. (댕글링 포인터 방지)
	if (cursor != editor.end()) { cursor = editor.erase(cursor); }

	// 5. 커서 위치에 문자 'O' 삽입
	editor.insert(cursor, 'O');

	// 최종 결과 출력부
	std::cout << "최종 문자열 편집 결과: ";
	for (cursor = editor.begin(); cursor != editor.end(); cursor++) {
		std::cout << *cursor; // 역참조를 통하여 현재 가리키고 있는 값을 출력한다.
	}
}