#include <iostream>
#include <windows.h> 
#include <string>

// 어제(34일차) 만들었던 클래스에서 추가로 구현한다.
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

    // 정보 출력 함수
    void print() const {
        std::cout << "[";
        for (int i = 0; i < count; ++i) {
            std::cout << data[i];
            if (i < count - 1) { std::cout << ", "; } // 마지막 요소가 아니면 쉼표 출력
        }
        std::cout << "] (Count: " << count << ", Capacity: " << capacity << ")\n";
    }

    // 여기서부터 35일차.
    // 1. 인덱스 접근 연산자 오버로딩.
    // 배열처럼 vec[i] 형태로 특정 인덱스의 요소에 접근할 수 있도록 한다.
    std::string& operator[](int index) { return data[index]; } // ...끝이다

    // 2. 결합 연산자 오버로딩
    // 멤버 함수로 써도 되고 전역 함수로 써도 된다.
    StringVector operator+(const StringVector& other) const {
        StringVector temp{}; // 새 StringVector 선언, 얘를 반환할 거다.
        for (int i = 0; i < count; i++) { temp.push_back(data[i]); } // 첫 번째 StringVector의 원소들을 순회하며 삽입.
        for (int i = 0; i < other.count; i++) { temp.push_back(other.data[i]); } // 두 번째(other) StringVector의 원소들을 순회하며 삽입.
        return temp; // 다 합쳐진 temp를 반환.
    }

    // 3. 삽입 연산자 오버로딩
    // friend 키워드를 이용하여 함수를 선언만 해 준다. 구현은 밖에서 한다.
    friend std::ostream& operator<<(std::ostream& os, const StringVector& vec);
};

// 4. 삽입 연산자 오버로딩 함수 구현부, print() 함수의 출력 양식과 똑같이 출력하도록 하였다.
std::ostream& operator<<(std::ostream& os, const StringVector& vec) {
    os << "[";
    for (int i = 0; i < vec.count; ++i) {
        os << vec.data[i];
        if (i < vec.count - 1) { os << ", "; } 
    }
    os << "] (Count: " << vec.count << ", Capacity: " << vec.capacity << ")";
    return os; // 입출력 스트림을 그대로 다시 반환해야 이어 쓸 수 있다.
}

int main() {
    SetConsoleOutputCP(65001);
    StringVector vec1;
    vec1.push_back("C++");
    vec1.push_back("OOP");

    StringVector vec2;
    vec2.push_back("Operator");
    vec2.push_back("Overloading");

    // 1. [] 연산자 테스트 (읽기 및 수정)
    std::cout << "vec1의 0번 요소: " << vec1[0] << std::endl;
    vec1[0] = "Modern C++"; // [] 연산자로 값 수정!

    // 2. << 연산자 테스트 (friend 활용)
    std::cout << "=== vec1 출력 (<< 연산자) ===" << std::endl;
    std::cout << vec1 << std::endl;

    // 3. + 연산자 테스트 (두 vector 결합)
    StringVector vec3 = vec1 + vec2;
    std::cout << "=== vec3 (vec1 + vec2) 출력 ===" << std::endl;
    std::cout << vec3 << std::endl;

    return 0;
}