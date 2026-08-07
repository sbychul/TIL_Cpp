#include <iostream>
#include <vector>
#include <algorithm>
#include <windows.h>

// C++ 람다의 기본 문법
// [캡처] (매개변수) -> 리턴 타입 { 본문 }

// [](대괄호): 캡처 블록, 외부 스코프에 있는 변수를 람다 내부로 어떻게 들여올지 결정하는 영역.
// []: 외부 변수를 전혀 사용하지 않음 (순수 함수)
// [x]: 외부 변수 x를 값 복사(Copy)로 가져옴(람다 내부에서 읽기 전용)
// [&x] : 외부 변수 x를 참조(Reference)로 가져옴(람다 내부에서 수정 시 외부 변수도 변경됨)
// [=] : 바깥의 모든 지역 변수를 값 복사로 가져옴
// [&] : 바깥의 모든 지역 변수를 참조로 가져옴
// [this] : 클래스 멤버 변수 / 메서드 접근을 위해 현재 객체 포인터를 캡처

// 오늘의 과제: 람다 문법 적응하기 (3단계)
int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. 기본 매개변수 람다 ===" << std::endl;
    // TODO 1: 두 정수를 받아 더 큰 값을 반환하는 람다 maxFunc 작성
    // std::max 써버리면 굳이 람다로 감쌀 이유가 싶긴 한데... 람다 식에 대해 익숙해지기 위함이니까...
    auto maxFunc = [](int a, int b) -> int { return max(a, b); };
    std::cout << "Max(10, 20): " << maxFunc(10, 20) << std::endl;


    std::cout << "\n=== 2. 캡처 블록 (복사 vs 참조) ===" << std::endl;
    int factor = 3;
    int sum = 0;

    // TODO 2-1: factor를 복사 캡처([factor])하여 n * factor를 반환하는 multiply 람다 작성
    // [factor]: 외부 변수 factor를 값 복사로 가져옴, 람다 내부에서 읽기 전용.
    auto multiply = [factor](int n) -> int { return factor * n; };
    std::cout << "5 * factor(= 3) = " << multiply(5) << std::endl;

    // TODO 2-2: sum을 참조 캡처([&sum])하여 호출될 때마다 sum에 10을 더하는 addTen 람다 작성
    // [&sum] : 외부 변수 sum를 참조(Reference)로 가져옴, 람다 내부에서 수정 시 외부 변수도 변경.
    auto addTen = [&sum]() -> int { return sum += 10; };
    addTen();
    addTen();
    std::cout << "addTen 2회 호출 후 sum: " << sum << std::endl; // 20 출력되어야 함


    std::cout << "\n=== 3. std::sort와 람다 활용 ===" << std::endl;
    std::vector<int> numbers = { 5, 2, 8, 1, 3 };

    // TODO 3: std::sort의 세 번째 인자로 내림차순 비교 람다 [](int a, int b) { ... } 전달
    // 별도의 비교 함수를 작성하지 않고 정렬 함수 내부에 직관적인 비교 로직을 던져주는 방식.
    std::sort(numbers.begin(), numbers.end(), [](int a, int b) { return a > b; });

    std::cout << "내림차순 정렬 결과: ";
    for (int n : numbers) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    return 0;
}