#include <iostream>
#include <windows.h> 
#include <string>

// [문제] 부모 클래스 Shape를 정의하고,
// 이를 상속받는 Rectangle과 Circle 클래스를 구현한 뒤,
// 부모 포인터 배열(동적 할당)을 통해 서로 다른 도형들을 한 번에 관리하는 시스템을 작성해 보세요.
class Shape {
public:
    virtual double getArea() const = 0; // 가상 함수 선언, Shape는 이제 추상 클래스.
    virtual void draw() const {}; // 도형을 그리는 것을 출력하는 가상 함수.

    // 가상 소멸자. 부모 클래스 포인터로 자식 객체를 생성한 뒤 지울 때,
    // 부모 소멸자에 virtual이 없으면 자식 클래스의 소멸자가 호출되지 않는다.
    // 부모 클래스에 가상 소멸자가 있다면 자식 클래스에서 따로 동적 할당을 진행한 게 아닌 이상 소멸자를 작성해줄 필요가 없다.
    virtual ~Shape() {};
};

// 자식 클래스 1, 직사각형
class Rect : public Shape {
private:
    double width;
    double height;
public:
    // 생성자에서 가로 세로 길이를 받아 초기화
    Rect(double width_input, double height_input) : width{ width_input }, height{ height_input } {}
    // 넓이 구하는 가상 함수 오버라이딩.
    double getArea() const override { return width * height; } 
    // 출력 함수 오버라이딩
    void draw() const override { std::cout << "[직사각형] 가로: " << width << ", 세로: " << height << ", 넓이: " << getArea() << std::endl; }
};

// 자식 클래스 2, 원
class Circle : public Shape {
private: double radius;
public:
    // 생성자에서 반지름을 받아 초기화
    Circle(double radius_input) : radius{ radius_input } {}
    // 넓이 구하는 가상 함수 오버라이딩.
    double getArea() const override { return radius * radius * 3.14; }
    // 출력 함수 오버라이딩
    void draw() const override { std::cout << "[원] 반지름: " << radius << ", 넓이: " << getArea() << std::endl; }
};

// 도형들을 담는 동적 관리자 클래스
class Canvas {
private:
    Shape** shapes; // 부모 클래스 Shape의 포인터 배열
    int count; // 현재 몇 개의 도형이 저장되어 있는지 저장할 멤버 변수
    int cap; // 최대 용량이 얼마인지 저장할 멤버 변수
public:
    // 생성자, 최대 몇 개의 도형을 담을지 입력받아 초기화
    Canvas(int capacity) : count{ 0 }, cap { capacity } { shapes = new Shape * [capacity]; }

    // 도형 포인터를 추가하는 함수
    void addShape(Shape* shape) {
        if (count == cap) { std::cout << "캔버스가 가득 찼습니다.\n"; return; }
        shapes[count++] = shape;
    }

    // 저장된 모든 도형의 draw를 호출하는 클래스
    void renderAll() const { for (int i = 0; i < count; i++) { shapes[i]->draw(); } }

    // 소멸자, 저장된 모든 Shape 객체들을 delete로 해제, 포인터 배열 메모리도 해제한다.
    ~Canvas() {
        for (int i = 0; i < count; i++) { delete shapes[i]; }
        delete[] shapes;
    }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);
    Canvas canvas(4); // 최대 4개 도형을 담는 캔버스

    canvas.addShape(new Rect(4.0, 5.0));
    canvas.addShape(new Circle(3.0));
    canvas.addShape(new Rect(2.0, 3.5));

    std::cout << "=== 캔버스 전체 출력 ===" << std::endl;
    canvas.renderAll();

    return 0;
    // main 종료 시 canvas 소멸자가 호출되며
    // 내부의 모든 도형(Rectangle, Circle) 객체들이 가상 소멸자를 통해 안전하게 delete 되어야 함!
}