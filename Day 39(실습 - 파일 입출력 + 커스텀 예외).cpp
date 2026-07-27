#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <windows.h>

// 오늘의 통합 과제: 스마트 파일 기반 로그 저장소 (File-based Log Storage)
// 시스템에서 발생하는 로그 메시지를 파일(logs.txt)에 기록(Save)하고, 프로그램 재시작 시 파일에서 다시 읽어와(Load) 객체화하는 시스템을 구현해 보세요.
// 파일이 없거나 읽기 오류 발생 시 지난번 배운 커스텀 예외(std::runtime_error 상속)를 던지도록 설계합니다.

// 커스텀 예외 클래스
class FileIOException : public std::runtime_error {
public: FileIOException(const std::string& msg = "파일 작업 중 오류가 발생하였습니다.") : std::runtime_error{ msg } {}
};

class LogEntry {
private:
	std::string timestamp; // 시간
	std::string level; // INFO / ERROR 등
	std::string message; // 내용
public:
	// 기본 생성자, 전부 string의 초기값으로 지정
	LogEntry() : timestamp{}, level{}, message{} {}

	// 3개 멤버 초기화 생성자
	LogEntry(std::string time, std::string lev, std::string msg) : timestamp{ time }, level{ lev }, message{ msg } {}

	// 삽입/추출 연산자 오버로딩을 위하여 friend 선언, 구현은 외부에서.
	friend std::ostream& operator<<(std::ostream& os, const LogEntry& entry);
	friend std::istream& operator>>(std::istream& is, LogEntry& entry);
};

// 파일/콘솔에 timestamp level message 형식으로 출력/저장.
std::ostream& operator<<(std::ostream& os, const LogEntry& entry) {
	os << entry.timestamp << " " << entry.level << " " << entry.message << std::endl;
	return os;
}

// 스트림에서 세 값을 읽어와 entry에 저장.
std::istream& operator>>(std::istream& is, LogEntry& entry) {
	// 스트림에서 순서대로 변수를 읽음, 띄어쓰기(공백)나 줄바꿈을 기준으로 단어를 구별하기 때문에,
	// 삽입 연산자 오버로딩 시 공백을 추가해 두었으므로 자동으로 읽어온 세 값이 timestamp, level, message에 저장됨.
	is >> entry.timestamp >> entry.level >> entry.message;
	return is;
}

class LogManager {
private: std::vector<LogEntry> logs; // 로그들을 저장할 벡터
public:
	// 벡터에 로그 추가
	void addLog(const LogEntry& entry) { logs.push_back(entry); }

	// 로그를 파일로 저장
	void saveToFile(const std::string& filename) {
		std::ofstream outFile(filename); // 파일에 쓰기 위해 스트림 실행
		if (!outFile.is_open()) { throw FileIOException("파일 저장 실패"); } // 실패 시 예외 던지기

		// 벡터를 순회하며 로그를 파일에 작성
		// 삽입 연산자 오버로딩을 했기 때문에 오버로딩한 양식대로 그대로 저장된다.
		for (const auto& log : logs) { outFile << log; }
		outFile.close(); // 다 썼으면 닫기
	}

	// 파일에서 로그 불러오기
	void loadFromFile(const std::string& filename) {
		std::ifstream inFile(filename); // 불러오기 위한 스트림 실행
		if (!inFile.is_open()) { throw FileIOException("파일 읽기 실패"); } // 실패 시 예외 던지기
		logs.clear(); // 파일에서 읽어오기 전 기존 vector 비우기(계속 쌓이는 것 방지)
		LogEntry entry{}; // 빈 entry 객체 생성

		// 파일을 끝까지 순회하며 벡터에 로그를 저장
		while (inFile >> entry) { addLog(entry); }
		inFile.close(); // 다 썼으면 닫기
	}

	// 벡터에 저장된 모든 로그 출력
	void printAll() const { for (const auto& log : logs) { std::cout << log; } }
};

int main() {
	SetConsoleOutputCP(65001);
	const std::string filename = "Day39_logs.txt";

	// 1. 로그 생성 및 파일 저장
	{
		LogManager manager;
		manager.addLog(LogEntry("2026-07-27_07:00", "INFO", "System_Initialized"));
		manager.addLog(LogEntry("2026-07-27_07:05", "ERROR", "Connection_Timeout"));

		std::cout << "=== 1. 파일에 로그 저장 ===" << std::endl;
		try {
			manager.saveToFile(filename);
			std::cout << "로그가 성공적으로 저장되었습니다: " << filename << std::endl;
		}
		catch (const std::exception& e) {
			std::cout << "예외 발생: " << e.what() << std::endl;
		}
	}

	// 2. 새로운 관리자가 파일로부터 로그 불러오기 (영속성 검증)
	{
		LogManager newManager;
		std::cout << "\n=== 2. 파일로부터 로그 불러오기 ===" << std::endl;
		try {
			newManager.loadFromFile(filename);
			newManager.printAll();
		}
		catch (const std::exception& e) {
			std::cout << "예외 발생: " << e.what() << std::endl;
		}
	}

	// 3. 존재하지 않는 파일 읽기 시도 (예외 발생 테스트)
	{
		LogManager errManager;
		std::cout << "\n=== 3. 예외 테스트 (없는 파일 읽기) ===" << std::endl;
		try {
			errManager.loadFromFile("invalid_file_path.txt");
		}
		catch (const std::exception& e) {
			std::cout << "💡 성공적으로 예외 포착: " << e.what() << std::endl;
		}
	}
}