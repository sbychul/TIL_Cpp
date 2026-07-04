#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 자원 탈취형 메모리 덮어쓰기가 구현된 스마트 버퍼 클래스
// 문자열을 동적으로 관리하는 SmartBuffer 클래스를 설계하세요.
class SmartBuffer {
private:
	// 문자열 포인터
	std::string* buf_ptr;
public:
	// 생성자. 문자열을 받아 힙 영역에 할당.
	SmartBuffer(std::string str_input) { buf_ptr = new std::string{ str_input }; }
	// Getter. 포인터가 가리키는 값을 반환
	std::string get() {
		// 만약 nullptr일 경우 (메모리 해제 이후의 경우)
		if (buf_ptr == nullptr) { return "Released"; }
		return *buf_ptr; // 아니라면 포인터가 가리키는 값을 반환.
	}
	// 이동 대입 연산자.
	// 이미 존재하는 두 객체 사이에서, 한 객체가 가진 자원을 다른 객체로 훔쳐 오며 덮어쓸 때 호출된다.
	// 대입 연산자 오버로딩: 같은 값을 둘 다 가짐
	// 이동 대입 연산자: 뺏어버림. 이 값은 이제 제 겁니다.
	SmartBuffer& operator=(SmartBuffer&& other) noexcept { // 우측값 참조를 쓴다. 이동 대입, 우측 값은 곧 사라질(비워버릴) 값이니까.
		std::cout << "이동 대입 연산자 호출됨\n";

		// 1단계, 자기 대입 검사.
		// (대입 연산자 오버로딩과 같은 이유로) 이게 없다면 자기 자신의 메모리를 지운 후 이미 지워진 메모리를 참조하려 함. (멸망)
		if (this == &other) { return *this; }
		// 2단계, 기존 갖고 있던 값의 메모리 할당 해제.
		delete buf_ptr;
		// 3단계, 원본(other)의 주소를 그대로 가져옴. (Deep Copy 아니다)
		buf_ptr = other.buf_ptr;
		// 4단계, 원본이 소멸할 때 자원이 파괴되지 않도록 끊어버리기
		// 안 하면 원본(other)에서 소멸자 호출하면 주소 그대로 터트린다 ㅋㅋㅋㅋㅋㅋ
		other.buf_ptr = nullptr;

		// 연속 대입이 가능하도록 자기 자신의 참조자를 반환.
		return *this;
	}

	// 소멸자
	~SmartBuffer() {
		if (buf_ptr == nullptr) { return; }
		delete buf_ptr;
	}
};

int main() {
	SetConsoleOutputCP(65001);
	// SmartBuffer 객체 두 개 생성
	SmartBuffer b1{ "T1" };
	SmartBuffer b2{ "Faker" };

	// 이동 대입 연산자 실행.
	b2 = std::move(b1);

	std::cout << "b1의 상태: " << b1.get() << "\n"; // Released
	std::cout << "b2의 상태: " << b2.get() << "\n"; // T1

	// 자기 이동 대입 시도
	b2 = std::move(b2);
	std::cout << "b2의 상태 (자기 대입 후): " << b2.get() << std::endl; // T1
}