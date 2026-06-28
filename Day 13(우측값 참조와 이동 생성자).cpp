#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 자원 탈취가 가능한 스마트 스트링 박스 클래스
// 문자열을 동적으로 관리하는 StringBox 클래스를 설계하세요.
// 멤버 변수(private) : 문자열 포인터(std::string* str_ptr)
// 생성자(public) : std::string 값을 받아 힙 영역에 new std::string{ value }로 동적 할당합니다.
// Getter(public) : std::string get() -> 포인터가 가리키는 문자열을 반환합니다. (포인터가 nullptr이면 "Empty" 반환 - Guard Clause 적용)
// 이동 생성자(public) : 우측값 참조(&&)를 이용해 원본의 포인터 주소를 그대로 가져오고, 원본의 포인터는 nullptr로 초기화하세요. (내부에서 "이동 생성자가 호출되었습니다."를 출력)
// 소멸자(public) : 포인터를 안전하게 delete합니다. (포인터가 nullptr이 아닐 때만 "메모리 해제 완료"를 출력)

class StringBox {
private:
	std::string* str_ptr;
public:
	// 기본 생성자
	StringBox(std::string value) { str_ptr = new std::string{ value }; }
	// Getter
	std::string get() {
		if (str_ptr == nullptr) { return "Empty"; }
		return *str_ptr;
	}

	// 이동 생성자: 원본의 메모리 주소를 가져오고(소유권 이전), 원본 포인터를 nullptr로 끊어버림.
	// 매개변수에 &&를 사용, 곧 사라질 임시 값(끊어버릴 거니까)인 우측 값만을 가져오기 위한 우측값 참조 문법.
	// &&는 (연산 후 곧바로)사라지지 않을 값에는 사용할 수 없음.
	// noexcept 키워드: 절대 예외를 발생시키지 않고, 주소만 옮긴다는 뜻.
	// 이 키워드가 있어야 벡터 등의 컨테이너들이 복사 대신 이동 생성자를 사용, 효율 상승.
	StringBox(StringBox&& other) noexcept {
		str_ptr = other.str_ptr; // 원본의 주소를 그대로 가져옴.
		other.str_ptr = nullptr; // 원본의 포인터를 끊어버림.
		std::cout << "이동 생성자가 호출되었습니다.\n";
	}

	~StringBox() {
		if (str_ptr == nullptr) { return; } // 이미 nullptr이면 delete할 이유가 없음, 바로 return.
		delete str_ptr; // 아니라면 메모리 해제
		std::cout << "메모리 해제 완료." << std::endl;
	}
};

// main 함수에서 box1 객체를 생성합니다.
// std::move(box1) 함수를 사용하여 box1을 강제로 우측값으로 변환한 뒤, 이를 인자로 삼아 box2 객체를 생성하세요.
// 문법: StringBox box2{ std::move(box1) };
// 이동 연산이 끝난 후, box1.get()과 box2.get()을 각각 출력하여 자원이 성공적으로 box2로 넘어가고 box1은 빈 상태가 되었는지 확인하세요.

int main() {
	SetConsoleOutputCP(65001);
	StringBox box1{ "Box this lap" }; 

	// std::move를 통해 box1 객체를 강제로 우측 값으로 변환.
	// 이걸 안 하면 복사가 일어남, 곧 사라질 우측값으로 취급하도록 만드는 함수.
	// "나 이거 안 쓸 거니까 얘 메모리 맘대로 해라" 느낌.
	StringBox box2{ std::move(box1) }; 

	// 이동 생성자로 인해 데이터 이동이 잘 되었는지 확인해 보자.
	std::cout << "box1의 상태: " << box1.get() << "\n";
	std::cout << "box2의 상태: " << box2.get() << "\n";
}