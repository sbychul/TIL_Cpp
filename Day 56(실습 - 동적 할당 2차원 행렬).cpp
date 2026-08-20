#include <iostream>
#include <windows.h> 
#include <stdexcept>
#include <string>

// 오늘의 종합 과제: 제네릭 행렬 클래스 (Matrix<T>) 구현
// 지난 과제에 이어 이번 과제는 학부 2학기 객체지향프로그래밍의 핵심 킬러 주제인 "동적 할당 2차원 행렬 클래스 (Matrix<T>)"입니다.
// 템플릿, 이중 동적 배열 메모리 관리, Rule of Three(복사 생성자, 복사 대입 연산자, 소멸자),
// 함수 호출 연산자(operator()) 및 산술 연산자(operator+) 오버로딩, 그리고 표준 예외 처리를 결합한 종합 과제입니다.

template <typename T>
class Matrix {
private:
    size_t rows; // 행 개수
    size_t cols; // 열 개수
    T** data;

    // 내부 메모리 할당 헬퍼 함수
    void allocateMemory() {
        data = new T * [rows]; // 행 포인터 배열 할당
        for (size_t i = 0; i < rows; ++i) { // 이후 각 행의 열 배열을 할당한다.
            data[i] = new T[cols](); // 기본값(0) 초기화
        }
    }

    // 내부 메모리 해제 헬퍼 함수
    void deallocateMemory() {
        if (data != nullptr) {
            for (size_t i = 0; i < rows; ++i) { // 각 행을 먼저 지우고
                delete[] data[i];
            }
            delete[] data; // 포인터 배열을 지운다.
            data = nullptr;
        }
    }

public:
    // 생성자, 행과 열의 개수를 받아 내부 메모리 할당 헬퍼 함수를 이용하여 동적 할당.
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(nullptr) {
        allocateMemory();
    }

    // 소멸자, 메모리 해제 헬퍼 함수를 이용한다.
    ~Matrix() {
        deallocateMemory();
    }

    // TODO 1: 깊은 복사를 수행하는 복사 생성자 구현
    // rows와 cols는 other의 것을 그대로 받아 온다.
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), data(nullptr) {
        allocateMemory(); // 할당용 헬퍼 함수 소환

        // 할당이 완료된 배열에 other의 data를 복사
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = other.data[i][j];
            }
        }
    }

    // TODO 2: 깊은 복사를 수행하는 복사 대입 연산자(operator=) 구현
    Matrix& operator=(const Matrix& other) {
        if (this == &other) { return *this; } // 자기 대입 검사
        deallocateMemory(); // 원래 가지고 있던 메모리를 해제.

        // 행/열의 개수를 갱신
        rows = other.rows;
        cols = other.cols;

        // 이후 나머지는 복사 생성자와 똑같은 방식으로 진행한다.
        allocateMemory(); // 할당용 헬퍼 함수 소환

        // 할당이 완료된 배열에 other의 data를 복사
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = other.data[i][j];
            }
        }

        return *this; // 연속 대입이 가능하도록 *this를 반환.
    }

    // TODO 3: 원소 접근을 위한 operator() 오버로딩 (비상수 버전)
    // 수정 가능한 참조를 반환, 값을 읽는 것 뿐만 아니라 새로운 값을 대입할 수 있다.
    T& operator()(size_t r, size_t c) {
        // 예외 처리
        if (r >= rows || c >= cols) { throw std::out_of_range("지정한 인덱스가 행렬의 크기를 벗어났습니다."); }
        return data[r][c]; // 아니라면 그대로 반환.
    }

    // TODO 4: 원소 조회를 위한 operator() 오버로딩 (상수 버전)
    // const가 있어 상수 객체에서도 호출할 수 있으나, 참조를 반환하므로 값 대입을 시도하면 컴파일 에러가 발생한다.
    // 생긴 건 비상수 버전과 상수 버전이 똑같으나, 역할이 다르기에 둘 다 만들어줘야 한다.
    const T& operator()(size_t r, size_t c) const {
        // 예외 처리
        if (r >= rows || c >= cols) { throw std::out_of_range("지정한 인덱스가 행렬의 크기를 벗어났습니다."); }
        return data[r][c]; // 아니라면 그대로 반환.
    }

    // TODO 5: 행렬 덧셈 연산자(operator+) 오버로딩
    // - 각 원소를 더한 새 Matrix<T> 객체 반환
    Matrix operator+(const Matrix& other) const {
        // 예외 처리
        if (rows != other.rows || cols != other.cols) { throw std::invalid_argument("행렬의 크기가 달라 연산을 진행할 수 없습니다."); }
        Matrix<T> result(rows, cols); // 각 원소를 더한 값을 담을 새 Matrix 객체.

        // 행렬을 순회하며 더한 값을 result 행렬에 넣는다.
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    void print() const {
        for (size_t i = 0; i < rows; ++i) {
            std::cout << "[ ";
            for (size_t j = 0; j < cols; ++j) {
                std::cout << data[i][j] << " ";
            }
            std::cout << "]\n";
        }
    }
};

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. Matrix 기본 생성 및 원소 접근/출력 ===" << std::endl;
    Matrix<int> m1(2, 2);
    m1(0, 0) = 1; m1(0, 1) = 2;
    m1(1, 0) = 3; m1(1, 1) = 4;
    std::cout << "--- Matrix m1 ---\n";
    m1.print();

    std::cout << "\n=== 2. 깊은 복사 생성자 및 복사 대입 검증 ===" << std::endl;
    {
        Matrix<int> m2 = m1; // 복사 생성자
        Matrix<int> m3(2, 2);
        m3 = m1;             // 복사 대입 연산자

        m1(0, 0) = 99;       // 원본 수정 시 복사본에 영향이 없어야 함 (깊은 복사)

        std::cout << "--- m1 (원본 수정 후) ---\n";
        m1.print();
        std::cout << "--- m2 (복사 생성자 본) ---\n";
        m2.print();
        std::cout << "--- m3 (복사 대입 본) ---\n";
        m3.print();
    }

    std::cout << "\n=== 3. 행렬 덧셈(operator+) 테스트 ===" << std::endl;
    try {
        Matrix<int> a(2, 2);
        a(0, 0) = 10; a(0, 1) = 20;
        a(1, 0) = 30; a(1, 1) = 40;

        Matrix<int> b(2, 2);
        b(0, 0) = 5;  b(0, 1) = 5;
        b(1, 0) = 5;  b(1, 1) = 5;

        Matrix<int> c = a + b;
        std::cout << "--- a + b 결과 ---\n";
        c.print();

        // 크기가 다른 행렬 덧셈 예외 테스트
        std::cout << "\n--- 크기 불일치 덧셈 예외 테스트 ---" << std::endl;
        Matrix<int> diff(3, 3);
        Matrix<int> fail = a + diff;

    }
    catch (const std::invalid_argument& e) {
        std::cout << "[유효성 예외 감지] " << e.what() << std::endl;
    }
    catch (const std::out_of_range& e) {
        std::cout << "[범위 초과 예외 감지] " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[기타 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}