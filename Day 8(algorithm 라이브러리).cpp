#include <iostream>
#include <windows.h> 
#include <string> 
#include <vector>
#include <algorithm>

// [문제] 성적 우수자 선발 프로그램학생의 정보를 저장할 Student 구조체(struct)를 정의하세요.
// 멤버 변수: 이름(std::string name), 점수(int score)
// 사용자로부터 학생의 수 N을 입력받습니다. 이어서 N명의 이름과 점수를 공백을 두고 입력받아 std::vector<Student>에 차례대로 저장합니다.
// 입력이 끝나면, 학생들을 점수가 높은 순(내림차순)으로 정렬하여 이름과 점수를 한 줄에 하나씩 출력하세요.
// 단, 점수가 같은 학생이 있다면 이름의 사전 순(오름차순)으로 정렬하세요.

struct Student{
public: // 점수에 접근하고 출력하기 좋게 public으로 하는 것이 좋겠다는 판단.
	std::string name;
	int score;

	// 생성자
	Student(std::string name_input, int score_input) : name{ name_input }, score{ score_input } {}
};

// 두 학생 객체를 비교하는 함수, 구조체를 정렬할 때는 bool을 반환하는 비교 함수를 만들어 std::sort 해줘야 함.
bool compareStudents(Student& a, Student& b) {
	if (a.score == b.score) { return a.name < b.name; } // 점수가 같으면 사전 순으로 정렬
	return a.score > b.score; // 기본적으로 점수 기준 내림차순
}

int main() {
	SetConsoleOutputCP(65001);
	int students_count{ 0 }, score_input{ 0 };
	std::string name_input{};
	std::vector<Student> students{}; // Student 객체를 담을 수 있는 빈 벡터 선언.

	std::cout << "학생의 수를 입력하세요: ";
	if (std::cin >> students_count) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); } // 버퍼 비우기
	else { std::cout << "잘못된 입력입니다.\n"; return 1; } // 예외 처리

	for (int i = 0; i < students_count; i++) { // 학생 수만큼 입력받기
		std::cout << i + 1 << "번째 학생의 이름을 입력하세요: ";
		std::getline(std::cin, name_input);
		std::cout << i + 1 << "번째 학생의 점수를 입력하세요: ";
		if (std::cin >> score_input) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); } // 버퍼 비우기
		else { // 예외 처리, 반복 무효화
			std::cout << "잘못된 입력입니다.\n";
			i--;
			score_input = 0;
			std::cin.clear(); // 에러 상태 플래그 초기화
			// 입력 버퍼 비우기, 최대 크기만큼의 문자를 줄바꿈 문자를 만날 때까지 ignore
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			continue;
		}
		// 정상적으로 입력받았다면 벡터에 새 Student 객체 삽입.
		students.push_back(Student{ name_input, score_input });
	}
	// algorithm 헤더의 sort 함수를 이용하여 정렬
	std::sort(students.begin(), students.end(), compareStudents);

	std::cout << "\n[점수 기준 정렬 결과]\n";
	// 순회하며 출력
	for (int i = 0; i < students.size(); i++) {
		std::cout << students[i].name << ": " << students[i].score << "점\n";
	}
}