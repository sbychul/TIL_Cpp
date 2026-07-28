#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <windows.h>

// 오늘의 통합 과제: 스마트 포인터 기반 인벤토리 & 캐릭터 시스템
// 게임 캐릭터의 인벤토리와 장비 시스템을 구현합니다.기존의 Raw Pointer(Node*, Item*) 대신 std::unique_ptr과 std::shared_ptr을 사용하여 메모리를 관리하고,
// 프로그램 종료 시 모든 메모리가 delete 호출 없이 안전하게 해제되는지 확인해 보세요.

class Item {
private:
	std::string name; // 이름
	int power; // 아이템 전투력(능력치)
public:
	// 생성자, 소멸자. 동작 확인 용도.
	Item(std::string name_input, int power_input) : name{ name_input }, power{ power_input } {
		std::cout << name << " 아이템이 생성되었습니다." << std::endl;
	}
	~Item() { std::cout << name << " 아이템이 파괴되었습니다." << std::endl; }

	// 아이템 정보 출력 함수
	void printInfo() const {
		std::cout << "아이템명: [" << name << "] / 전투력(능력치): " << power << std::endl;
	}
};

class Inventory {
private:
	// 아이템들의 독점적 소유권을 가지는 벡터
	std::vector<std::unique_ptr<Item>> items;
public:
	// 인벤토리에 아이템 추가, std::move를 통해 소유권 이전
	void addItem(std::unique_ptr<Item> item) { items.push_back(std::move(item)); }

	// 지정한 인덱스의 아이템을 인벤토리에서 제거, 소유권을 외부로 넘기며 반환
	std::unique_ptr<Item> removeItem(int index) {
		// 인덱스 유효성 검사
		if (index < 0 || index >= items.size()) { std::cout << "잘못된 인덱스입니다." << std::endl; return nullptr; }
		// 다른 포인터에 해당 인덱스에 위치한 아이템 소유권 이전
		std::unique_ptr<Item> temp = std::move(items[index]);
		// 벡터에서의 해당 칸 자체를 삭제
		items.erase(items.begin() + index);
		// 아이템 소유권을 가지고 있는 포인터를 반환
		return temp;
	}

	// 모든 아이템의 정보 출력
	void printAll() const {
		for (const auto& item : items) { item->printInfo(); }
	}
};

class Character {
private:
	std::string name; // 캐릭터명
	std::shared_ptr<Item> equippedItem; // 장착된 아이템, 외부에서 공유될 수 있으므로 공유 포인터로 관리
public:
	// 생성자
	Character(std::string name_input) : name{ name_input }, equippedItem{ nullptr } {}

	// 아이템 장착
	void equipItem(std::shared_ptr<Item> item) { equippedItem = item; }

	// 아이템 장착 해제
	void unequipItem() { equippedItem = nullptr; }

	// 캐릭터 이름 및 현재 장착 중인 아이템 정보(없으면 "없음") 출력
	void printStatus() const {
		std::cout << "캐릭터명: [" << name << "]\n장착 중인 아이템: ";
		if (equippedItem == nullptr) { std::cout << "없음" << std::endl; }
		else { equippedItem->printInfo(); }
	}
};

int main() {
	SetConsoleOutputCP(65001);

	std::cout << "=== 1. unique_ptr 기반 Inventory 테스트 ===" << std::endl;
	{
		Inventory inv;

		// make_unique로 아이템 생성 후 Inventory에 소유권 이전
		inv.addItem(std::make_unique<Item>("장검", 50));
		inv.addItem(std::make_unique<Item>("강철 갑옷", 30));

		inv.printAll();

		std::cout << "\n--- 인벤토리에서 아이템 하나 꺼내기 ---" << std::endl;
		std::unique_ptr<Item> poppedItem = inv.removeItem(0);
		if (poppedItem) {
			std::cout << "꺼낸 아이템: ";
			poppedItem->printInfo();
		}

		std::cout << "\n--- 꺼낸 아이템(poppedItem) 스코프 종료 전 ---" << std::endl;
	} // Inventory 및 poppedItem 스코프 종료 -> 모든 Item 객체 자동 소멸!

	std::cout << "\n=== 2. shared_ptr 기반 Character 장비 시스템 테스트 ===" << std::endl;
	{
		Character hero("용사");

		// 아이템 생성 (참조 카운트 = 1)
		std::shared_ptr<Item> sharedSword = std::make_shared<Item>("전설의 검", 100);

		std::cout << "현재 아이템 참조 카운트: " << sharedSword.use_count() << std::endl;

		// 캐릭터에 아이템 장착 (참조 카운트 = 2)
		hero.equipItem(sharedSword);
		std::cout << "장착 후 아이템 참조 카운트: " << sharedSword.use_count() << std::endl;

		hero.printStatus();

		// 캐릭터 장착 해제 (참조 카운트 = 1)
		hero.unequipItem();
		std::cout << "장착 해제 후 아이템 참조 카운트: " << sharedSword.use_count() << std::endl;

	} // hero 및 sharedSword 스코프 종료 -> 전설의 검 자동 소멸!

	std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
}