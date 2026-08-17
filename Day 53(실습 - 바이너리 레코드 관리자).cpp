#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <windows.h>

// 오늘의 과제: 바이너리 레코드 관리자 (BinaryRecordManager)
// 강의계획서의 제12~13주차(C++ 입출력 스트림, 파일 입출력, 바이너리 직렬화) 과제입니다.
// 학생 기록 구조체 Student를 바이너리 파일(students.dat)에 저장하고 복원하는 클래스를 구현합니다.

// 파일에 통째로 읽고 쓸 구조체 (고정 크기 버퍼 사용)
struct Student {
    int id;
    // 구조체 안에 std::string이나 포인터(char*) 같은 동적 할당 객체가 들어있으면 포인터 주소만 저장되어 복원 시 댕글링 포인터가 발생하므로,
    // 단순 메모리 덤프 방식에서는 char name[32]와 같은 고정 크기 배열을 사용한다.
    char name[32];
    double gpa;
};

class BinaryRecordManager {
private:
    std::string filename;

public:
    BinaryRecordManager(const std::string& fname) : filename(fname) {}

    // TODO 1: 구조체 데이터를 바이너리 형태로 파일 끝에 추가 (std::ios::binary | std::ios::app)
    // - ofstream 열기 실패 시 std::runtime_error 예외 throw
    // - reinterpret_cast와 write() 사용
    void appendRecord(const Student& s) {
        // std::ios::binary | std::ios::app 옵션을 명시하여 바이너리, 파일 맨 끝에 덧붙이는 형태(append)로 작성
        std::ofstream outFile(filename, std::ios::binary | std::ios::app);
        // 파일 열기 실패 시 런타임 에러를 던짐.
        if (!outFile.is_open()) { throw std::runtime_error("파일을 불러오는 데 실패했습니다."); }

        // 바이트 데이터를 그대로 &s를 char* 타입으로 변환하여 전달.
        // Student 구조체의 크기만큼 메모리를 할당하여 통째로 덤프.
        outFile.write(reinterpret_cast<const char*>(&s), sizeof(Student));
    }

    // TODO 2: 바이너리 파일로부터 모든 Student 구조체 데이터를 읽어 std::vector로 반환
    // - ifstream 열기 실패 시 std::runtime_error 예외 throw
    // - read()를 통해 한 구조체씩 읽으며 vector에 push_back
    // - 파일 끝(EOF) 도달 시 반복 종료
    std::vector<Student> loadAllRecords() {
        // 바이너리 읽기 옵션 명시
        std::ifstream inFile(filename, std::ios::binary);
        // 파일 열기 실패 시 런타임 에러를 던짐.
        if (!inFile.is_open()) { throw std::runtime_error("파일을 불러오는 데 실패했습니다."); }
        std::vector<Student> result; // 구조체를 담을 벡터 선언

        Student temp; // 임시 구조체 변수 선언
        // &temp를 char* 타입으로 변환하여 파일의 바이트를 메모리에 채워 넣는다.
        while (inFile.read(reinterpret_cast<char*>(&temp), sizeof(Student))) {
            result.push_back(temp);
        }

        return result;
    }

    // 파일 초기화 (테스트용)
    void clearFile() {
        std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
    }
};

int main() {
    SetConsoleOutputCP(65001);

    BinaryRecordManager manager("Day53_students.dat");
    manager.clearFile(); // 이전 데이터 초기화

    std::cout << "=== 1. 바이너리 데이터 쓰기 ===" << std::endl;
    try {
        Student s1 = { 20230001, "Alice", 4.15 };
        Student s2 = { 20230002, "Bob", 3.85 };
        Student s3 = { 20230003, "Charlie", 4.30 };

        manager.appendRecord(s1);
        manager.appendRecord(s2);
        manager.appendRecord(s3);
        std::cout << "-> 총 3건의 레코드 바이너리 저장 완료" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[쓰기 에러] " << e.what() << std::endl;
    }

    std::cout << "\n=== 2. 바이너리 데이터 역직렬화(읽기) ===" << std::endl;
    try {
        std::vector<Student> records = manager.loadAllRecords();
        std::cout << "-> 로드된 학생 수: " << records.size() << "명\n" << std::endl;

        for (const auto& s : records) {
            std::cout << "학번: " << s.id
                << " | 이름: " << s.name
                << " | 학점: " << s.gpa << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << "[읽기 에러] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}