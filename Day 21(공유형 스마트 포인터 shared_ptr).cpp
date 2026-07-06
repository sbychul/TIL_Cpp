#include <iostream>
#include <windows.h> 
#include <string>
#include <memory>

// [문제] 길드(Guild)원들의 레이드 방(RaidRoom) 공동 소유 시스템
// 동적으로 관리될 RaidRoom 클래스.
class RaidRoom {
private: std::string room_name;
public:
	// 생성자와 소멸자
	RaidRoom(std::string name) : room_name{ name } { std::cout << "레이드 방 [" << room_name << "]이(가) 개설되었습니다.\n"; }
	~RaidRoom() { std::cout << "레이드 방 [" << room_name << "]의 모든 유저가 퇴장하여 방이 해체됩니다.\n"; }

	// Getter
	std::string getRoomName() { return room_name; }
};

// RaidRoom을 소유하여 공유할 클래스.
class GuildMember {
private:
	std::string member_name;
	// 레이드 방을 가리킬 공유 스마트 포인터.
	std::shared_ptr<RaidRoom> room_ptr;
public:
	// 생성자, 멤버 이름을 받아 초기화
	GuildMember(std::string name) : member_name{ name } {}

	// 방 입장 함수
	// 매개변수로 받은 shared_ptr을 자신의 room_ptr에 대입(복사)하여 소유권을 공유한다.
	void enterRoom(const std::shared_ptr<RaidRoom>& room) { // 매개변수 타입이 &(참조자)가 아니면 불필요한 복사가 진행된다.
		room_ptr = room;
		// use_count() : 참조 계수, 얼마나 많은 포인터가 해당 주소를 참조하고 있는지를 확인할 수 있다.
		std::cout << member_name << "이(가) [" << room->getRoomName() << "]에 입장했습니다. (현재 방 유저 수: [" << room.use_count() << "]명)\n";
	}

	// 퇴장 함수
	void leaveRoom() {
		room_ptr = nullptr; // 또는 room_ptr.reset();, 소유권을 해제한다는 메커니즘은 동일.
		std::cout << member_name << "이(가) 방에서 퇴장했습니다.\n";
	}
};

int main() {
	SetConsoleOutputCP(65001);
	// make_shared를 통하여 공유할 객체를 공유형 스마트 포인터 main_room에 할당한다.
	std::shared_ptr main_room = std::make_shared<RaidRoom>("2026 MSI LPL 레이드");
	
	// 두 개의 길드원(팀명이긴 한데...) 객체를 생성.
	GuildMember first_seed{"Hanwha Life Esports"};
	GuildMember second_seed{ "T1" };
	
	// 입장
	first_seed.enterRoom(main_room);
	second_seed.enterRoom(main_room);

	// main_room 변수를 reset하여 소유권을 해제.
	// 그러나, first_seed와 second_seed가 본래 main_room이 갖고 있던 주소에 대한 소유권이 있기 때문에,
	// 지금 소멸자가 호출되면 뭔가 잘못된 것이다. (그럴 일 없다는 뜻)
	main_room.reset();

	// 퇴장
	first_seed.leaveRoom();
	second_seed.leaveRoom(); // 여기서 소멸자가 자동으로 호출된다. 왜? 해당 주소에 대한 소유권을 가진 객체(변수)가 아무 것도 없기 때문에.

	// 퇴장 시 방 폭파 문구가 먼저 뜨는 이유:
	// 1. 퇴장 함수에서 T1이 소유권을 해제(room_ptr = nullptr) -> 이 시점에 참조 계수가 0.
	// 2. 참조 계수가 0이 되었으므로, RaidRoom의 소멸자가 참조 계수가 0이 된 room_ptr = nullptr 라인에서 즉시 발동. 
	// 3. 따라서 "방이 해체됩니다"가 먼저 화면에 출력.
}