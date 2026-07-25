#include <iostream>
#include <windows.h> 
#include <string>

// 오늘의 통합 과제: 스마트 미디어 라이브러리 관리 시스템
// 다양한 미디어(책, 음반 등)를 통합 관리하는 시스템을 구현합니다.
// 미디어 객체 생성 시 고유 ID를 static으로 자동 부여하고, friend 연산자로 출력하며,
// 추상 클래스를 이용해 한 동적 배열 캔버스에 담아 관리하세요.

// 최상위 부모 추상 클래스
class Media {
protected:
    int id; // 고유 ID
    std::string title; // 제목
    static int nextId; // static 멤버, 1부터 시작하여 객체가 생성될 때마다 1씩 증가, 모든 Media 객체는 이를 공유한다.
public:
    // 생성자, title을 입력받아 초기화, id에 nextId++를 할당(할당 후 nextId의 값이 1 증가).
    Media(std::string title_input) : id{ nextId++ }, title{ title_input } {}

    virtual void printInfo() const = 0; // 순수 가상 함수, Media 클래스는 이제 추상 클래스.
    virtual ~Media() {}; // 가상 소멸자
    virtual Media* clone() const = 0; // 복제 가상 함수
    friend std::ostream& operator<<(std::ostream& os, const Media& media); // 삽입 연산자 오버로딩 함수 friend 선언
};

// Media 하위 클래스 1, 책
class Book : public Media {
private:
    std::string author; // 추가 멤버 변수 (저자)
public:
    // 생성자, Media 생성자에 title을 넘기고 author만 따로 처리.
    Book(std::string title_input, std::string author_input) : Media{ title_input }, author{ author_input } {}

    // 정보 출력 함수 오버라이딩
    void printInfo() const override {
        std::cout << "[책 ID: " << id << "] 제목: " << title << " | 저자: " << author;
    }

    // 복제 가상 함수 오버라이딩, 자기 자신을 똑같이 생성해서 반환한다.
    Media* clone() const override { return new Book(*this); }
};

// Media 하위 클래스 2, 음악(음반)
class Music : public Media {
private:
    int duration; // 추가 멤버 변수 (재생 시간(초 단위))
public:
    // 생성자, Media 생성자에 title을 넘기고 duration만 따로 처리.
    Music(std::string title_input, int duration_input) : Media{ title_input }, duration{ duration_input } {}

    // 정보 출력 함수 오버라이딩
    void printInfo() const override {
        std::cout << "[음악 ID: " << id << "] 제목: " << title << " | 재생 시간: " << duration << "초";
    }

    // 복제 가상 함수 오버라이딩, 자기 자신을 똑같이 생성해서 반환한다.
    Media* clone() const override { return new Music(*this); }
};

// Media 객체(클래스)의 삽입 연산자 오버로딩 구현부
std::ostream& operator<<(std::ostream& os, const Media& media) {
    media.printInfo(); // 내부에서 media 객체의 printInfo를 호출하도록 함.
    return os;
}

class LibraryManager {
private:
    Media** items; // Media 객체들을 보관할 동적 포인터 배열
    int count; // 현재 개수
    int capacity; // 최대 용량
public:
    // 생성자, 용량을 입력받아 동적 할당
    LibraryManager(int cap) : count{ 0 }, capacity{ cap } { items = new Media * [capacity]; }

    // 소멸자, 동적 포인터 배열을 순회하며 각각의 객체를 delete 후 배열도 delete.
    ~LibraryManager() {
        for (int i = 0; i < count; i++) { delete items[i]; }
        delete[] items;
    }

    // 복사 생성자, other의 count와 capacity를 받아 온다.
    LibraryManager(const LibraryManager& other) : count{ other.count }, capacity{ other.capacity } {
        // 이후 capacity만큼의 동적 포인터 배열 형성
        items = new Media * [capacity];
        // 복제 함수를 이용하여 깊은 복사를 수행한다.
        for (int i = 0; i < count; i++) { items[i] = other.items[i]->clone(); }
    }

    // 추가 함수
    void addMedia(Media* media) {
        if (count == capacity) { std::cout << "용량이 가득 찼습니다.\n"; return; }
        items[count++] = media;
    }

    // 저장된 모든 미디어 출력 함수, 역참조하여 삽입 연산자 오버로딩이 호출되도록 함.
    void showAll() const { for (int i = 0; i < count; i++) { std::cout << *items[i] << std::endl; } }
};

// static 변수 초기화. 전역 공간에서 초기화.
int Media::nextId = 1;

// 테스트 케이스
int main() {
	SetConsoleOutputCP(65001);
    LibraryManager lib(5);

    // static counter에 의해 ID가 1, 2, 3으로 자동 부여되어야 함
    lib.addMedia(new Book("C++ Primer", "Lippman"));
    lib.addMedia(new Music("Hotel California", 390));
    lib.addMedia(new Book("Effective C++", "Meyers"));

    std::cout << "=== Original Library List ===" << std::endl;
    lib.showAll();

    std::cout << "\n=== Deep Copy Test (Deep Copy Manager) ===" << std::endl;
    LibraryManager copiedLib = lib; // 복사 생성자 호출
    copiedLib.showAll();

    return 0;
}