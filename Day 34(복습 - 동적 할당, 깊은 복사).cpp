#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 동적 메모리와 얕은 복사/깊은 복사 문제: StringVector 클래스 구현
// C++의 클래스, 동적 할당(new[] / delete[]), 복사 생성자 개념을 종합적으로 활용하여 가변 크기 문자열 배열 클래스(StringVector) 를 직접 구현해 보세요.
class StringVector {
private:
	std::string* data;	// 동적 할당을 진행할 문자열 배열 포인터
	int capacity;		// 현재 용량 (부족할 때마다 2배로 늘리고 재할당할 예정)
	int count;			// 저장된 요소 개수
public:
	// 기본 생성자, 초기 용량을 입력받아 동적 문자열 배열 할당.
	StringVector(int init_capacity = 2) : capacity{ init_capacity }, count{ 0 } { data = new std::string[capacity]; }

	// 소멸자, 동적 할당된 메모리 해제
	~StringVector() {
		if (data == nullptr) { return; } // Guard Clause
		delete[] data;
	}

    // 복사 생성자
    StringVector(const StringVector& other) : capacity{ other.capacity }, count{ other.count } {
        data = new std::string[capacity]; // 원본과 동일한 크기의 새 메모리를 할당
        for (int i = 0; i < count; i++) { data[i] = other.data[i]; } // 원본(other)의 배열을 순회하며 복사받을 배열에 같은 내용을 저장.
    }

    // 요소 추가 (용량이 가득 차면 용량을 2배로 늘리고 재할당)
    void push_back(const std::string& val) {
        // 용량이 가득 찼을 경우
        if (capacity == count) {
            capacity = capacity * 2; // 용량을 2배로
            std::string* temp = new std::string[capacity]; // 임시 배열 생성 (사실 이게 새 거가 될 거임)
            for (int i = 0; i < count; i++) { temp[i] = data[i]; } // 순회하며 삽입
            delete[] data; // 메모리 누수를 막기 위하여 기존 배열 삭제
            data = temp; // data 포인터가 temp의 주소를 가리키도록 함.
        }
        
        // 삽입 작업 시행
        data[count++] = val;
    }

    // 5. 정보 출력 함수
    void print() const {
        std::cout << "[";
        for (int i = 0; i < count; ++i) {
            std::cout << data[i];
            if (i < count - 1) { std::cout << ", "; } // 마지막 요소가 아니면 쉼표 출력
        }
        std::cout << "] (Count: " << count << ", Capacity: " << capacity << ")\n";
    }
};

// 테스트 코드
int main() {
    SetConsoleOutputCP(65001);

    StringVector vec1;
    vec1.push_back("C++");
    vec1.push_back("OOP");
    vec1.push_back("Memory Management"); // 용량 2 초과 -> 2배 재할당 발생!

    std::cout << "=== vec1 출력 ===" << std::endl;
    vec1.print();

    // 복사 생성자 호출 (깊은 복사 검증)
    StringVector vec2 = vec1;
    vec2.push_back("Deep Copy Success!");

    std::cout << "\n=== vec2 출력 ===" << std::endl;
    vec2.print();

    return 0;
    // main 종료 시 vec1, vec2의 소멸자가 각각 호출됨.
    // 깊은 복사가 안 되어 있다면 여기서 "Double Free" 크래시가 발생함!
}