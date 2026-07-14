#include <iostream>
#include <windows.h> 
#include <string>
#include <fstream> // 파일 입출력 스트림 헤더

// std::ofstream (Output File Stream): 파일에 데이터를 쓸 때(출력)
// std::ifstream (Input File Stream): 파일로부터 데이터를 읽을 때(입력)
// std::fstream: 읽기와 쓰기를 동시에 할 때

// [문제] F1 드라이버 데이터 파일 저장 및 로드 시스템
class F1Driver {
private:
	std::string name; // 이름
	std::string team; // 팀
	int number; // 드라이버 번호
	int wins; // 그랑프리 우승 횟수
	friend std::ostream& operator<<(std::ostream& os, const F1Driver& driver);
public:
	// 디폴트 생성자(파일에서 읽어와 채워 넣을 때 사용, 전부 빈 칸(기본값)으로 초기화)
	F1Driver() : name{ "" }, team{ "" }, number{ 0 }, wins{ 0 } {}

	// 사용자 정의 생성자. 이름, 팀명, 번호, 우승 횟수(미입력 시 기본값 0)를 받아 초기화
	F1Driver(std::string driver_name, std::string team_name, int driver_number, int grand_prix_wins = 0) :
		name{ driver_name }, team{ team_name }, number{ driver_number }, wins{ grand_prix_wins } {}

	// Getter
	std::string getName() { return name; }
	std::string getTeam() { return team; }
	int getNumber() { return number; }
	int getWins() { return wins; }

	// Setter, 파일에서 읽어낸 값을 객체에 세팅할 때 사용
	void setDriverProfile(std::string name, std::string team, int number, int wins) {
		this->name = name;
		this->team = team;
		this->number = number;
		this->wins = wins;
	}
};

// 삽입 연산자 오버로딩
std::ostream& operator<<(std::ostream& os, const F1Driver& driver) {
	os << "[" << driver.team << "] No." << driver.number << " " << driver.name << " (우승 횟수: " << driver.wins << "회)";
	return os;
}

int main() {
	SetConsoleOutputCP(65001);
	// 드라이버 객체 2개 생성.
	F1Driver leclerc_save{ "Charles Leclerc", "Scuderia Ferrari HP", 16, 9 };
	F1Driver bearman_save{ "Oliver Bearman", "TGR Haas", 87 }; // 자동으로 wins가 0으로 세팅되는지 확인

	// 파일 쓰기를 위한 스트림 실행.
	std::ofstream outFile("Day28_driver_data.txt");
	if (!outFile.is_open()) { // 파일 개방 실패 시, 검증하는 코드는 필수적이다.
		std::cerr << "파일을 열 수 없습니다!\n";
		return 1;
	}

	// 줄바꿈으로 구분할 예정. 두 객체의 데이터를 파일에 작성.
	outFile << leclerc_save.getName() << "\n" << leclerc_save.getTeam() << "\n" << leclerc_save.getNumber() << "\n" << leclerc_save.getWins() << "\n";
	outFile << bearman_save.getName() << "\n" << bearman_save.getTeam() << "\n" << bearman_save.getNumber() << "\n" << bearman_save.getWins() << "\n";
	
	std::cout << "[시스템] 드라이버 데이터를 파일에 성공적으로 저장했습니다.\n";
	outFile.close(); // 다 썼으면 닫기

	// 데이터를 불러올 객체 배열 생성
	F1Driver loads[2];

	// 파일 읽기를 위한 스트림 실행
	std::ifstream inFile("Day28_driver_data.txt");
	if (!inFile) { // .is_open() 대신 객체 이름 자체를 조건식에 넣어도 가능.
		std::cerr << "파일이 존재하지 않습니다!\n";
		return 1;
	}

	std::string line, name{}, team{};
	int num{ 0 }, wins{ 0 }, i{ 0 };
	// 파일 끝날 때까지 반복
	while (std::getline(inFile, line)) {
		name = line; // 첫 줄은 무조건 이름이니까..
		std::getline(inFile, line); // 한 줄 더 가져오기
		team = line; // 팀
		std::getline(inFile, line); // 더줘
		num = std::stoi(line); // 드라이버 번호(파싱)
		std::getline(inFile, line); // 더줘
		wins = std::stoi(line); // 우승 횟수(파싱)

		// 객체 배열에 세팅
		loads[i].setDriverProfile(name, team, num, wins);
		i++;

		// getline을 해도 오류가 나지 않을 만큼 내용이 더 있을 걸 아니까 해버린 하드코딩.
		// 실제로는 현업에서 쓰는 더 예쁜 방법이 있겠죠...? 
	}
	std::cout << "[시스템] 파일로부터 데이터를 불러와 객체를 복원합니다...\n";
	inFile.close(); // 다 썼으면 닫기

	// 잘 불러왔는지 출력
	std::cout << "\n" << loads[0] << "\n" << loads[1] << std::endl;
}