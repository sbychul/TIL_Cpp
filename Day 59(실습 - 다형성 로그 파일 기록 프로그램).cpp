#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 다형성 로그 파일 기록기 (LogManager)
// 오늘의 종합 과제는 학부 2학기 강의계획서의 제7강(가상 함수와 다형성), 제9강(연산자 중복과 프렌드 함수),
// 제12강(스트림 입출력과 파일 처리), 제14강(예외 처리)을 결합한 "다형성 로그 파일 직렬화 시스템 (LogManager)"입니다.
// 
// 다양한 형태의 로그(일반 텍스트 로그, 에러 코드 로그)를 추상 기본 클래스로 묶고,
// 출력 스트림 연산자(operator<<)의 다형성 동적 바인딩 기법, 파일 스트림(std::ofstream) 입출력 및 예외 처리,
// 그리고 동적 자원 소멸자 클린업을 다루는 실전 스켈레톤 과제입니다.

// 1. 최상위 추상 클래스: LogEntry
class LogEntry {
protected:
    std::string timestamp;

public:
    LogEntry(const std::string& time) : timestamp(time) {}
    // 가상 소멸자 선언
    virtual ~LogEntry() {}

    // 스트림 출력을 위한 순수 가상 함수 writeTo() 선언
    virtual void writeTo(std::ostream& os) const = 0;

    // operator<< 오버로딩 (entry.writeTo(os)를 호출하여 다형성 출력 지원)
    friend std::ostream& operator<<(std::ostream& os, const LogEntry& entry) {
        entry.writeTo(os);
        return os;
    }
};

// 일반 텍스트 로그: SimpleLog
class SimpleLog : public LogEntry {
private:
    std::string message;

public:
    SimpleLog(const std::string& time, const std::string& msg)
        : LogEntry(time), message(msg) {
    }

    // writeTo() 오버라이딩 "[timestamp] [INFO] message" 형식으로 출력.
    void writeTo(std::ostream& os) const override {
        os << "[" << timestamp << "]" << " [INFO] " << message << std::endl;
    }
};

// 3. 에러 코드 로그: ErrorLog
class ErrorLog : public LogEntry {
private:
    int errorCode;
    std::string detail;

public:
    ErrorLog(const std::string& time, int code, const std::string& det)
        : LogEntry(time), errorCode(code), detail(det) {
    }

    // writeTo() 오버라이딩 "[timestamp] [ERROR: errorCode] detail" 형식으로 출력.
    void writeTo(std::ostream& os) const override {
        os << "[" << timestamp << "]" << " [ERROR: " << errorCode << "]" << detail << std::endl;
    }
};

// 4. 로그 관리자: LogManager
class LogManager {
private:
    std::vector<LogEntry*> logs;

public:
    LogManager() = default;

    // 소멸자 (logs 벡터 내의 모든 LogEntry* 동적 객체 delete 해제)
    ~LogManager() {
        for (LogEntry* log : logs) { delete log; }
    }

    void addLog(LogEntry* log) {
        logs.push_back(log);
    }

    // 콘솔에 모든 로그 출력 
    void printAllLogs() const {
        // operator<< 오버로딩한 것을 활용.
        for (LogEntry* log : logs) { std::cout << *log; }
    }

    // 파일에 모든 로그를 저장하는 함수
    void saveToFile(const std::string& filename) const {
        std::ofstream OutFile(filename);
        // 파일 열기 실패 시 예외를 던짐
        if (!OutFile.is_open()) { throw std::runtime_error("파일 열기 실패"); }
        // logs 순회하며 파일 스트림에 삽입 연산자 오버로딩 활용하여 기록
        for (LogEntry* log : logs) { OutFile << *log; }
    }
};

int main() {
    SetConsoleOutputCP(65001);

    LogManager manager;

    // 다형성 객체 추가
    manager.addLog(new SimpleLog("10:00:01", "서버가 성공적으로 시작되었습니다."));
    manager.addLog(new ErrorLog("10:05:23", 404, "페이지를 찾을 수 없습니다."));
    manager.addLog(new SimpleLog("10:10:00", "사용자 로그인 성공 (ID: user01)"));
    manager.addLog(new ErrorLog("10:12:45", 500, "데이터베이스 연결 타임아웃 발생."));

    std::cout << "=== 1. 콘솔 다형성 로그 출력 테스트 ===" << std::endl;
    manager.printAllLogs();

    std::cout << "\n=== 2. 파일 직렬화 저장 테스트 ===" << std::endl;
    try {
        manager.saveToFile("Day59_server_log.txt");
        std::cout << "-> 'Day59_server_log.txt' 파일에 로그 저장 완료!" << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "[파일 에러] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}