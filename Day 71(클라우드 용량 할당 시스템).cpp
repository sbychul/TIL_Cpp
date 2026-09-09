#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <windows.h>

using namespace std;

// 오늘의 실습 과제: 스마트 캠퍼스 클라우드 스토리지 할당 시스템 (CloudStorageManager)
// [문제 설명]
// 오늘의 실습은 2학기 강의계획서의 제5강(깊은 복사를 보장하는 복사 생성자 및 복사 대입 연산자), 제6강(정적 멤버 static의 생명주기 캡슐화), 제7강(복합 연산자 오버로딩: +=, <<),
// 제9·10강(추상 클래스 및 가상 함수 다형성), 제11강(STL vector, algorithm), 제14강(사용자 정의 예외 처리)까지 6개 핵심 단원을 결합했습니다.
// 학생과 연구실에 배정되는 가상 드라이브(스토리지)를 생성하고 사용 용량을 모니터링하는 시스템입니다.
// 각 스토리지는 가상 데이터 블록을 동적 할당하여 관리하며, 전체 서버 풀의 활성 스토리지 수와 총 용량 한도를 예외 처리와 다형성을 통해 안전하게 통제합니다.

// 커스텀 예외 클래스
class StorageException : public exception {
private: string msg;
public:
    StorageException(string msg) : msg{ msg } {}
    const char* what() const noexcept override  { return msg.c_str(); }
};

// 최상위 추상 클래스
class StorageUnit {
protected:
    string ownerId;
    double allocatedGB;
    static int activeStorageCount; // static 멤버, 현재 메모리에 살아 있는 스토리지 객체 수 카운터
public:
    // 생성자와 가상 소멸자에서 activeStorageCount를 관리.
    StorageUnit(string ownerId, double allocatedGB) : ownerId{ ownerId }, allocatedGB{ allocatedGB } { activeStorageCount++; }
    virtual ~StorageUnit() { activeStorageCount--; } 

    // Getters
    string getOwnerId() const { return ownerId; }
    double getAllocatedGB() const { return allocatedGB; }
    // 활성 객체 수 게터
    static int getActiveCount() { return activeStorageCount; }

    // 순수 가상 함수 선언
    virtual double calculateCost() const = 0; // 월 사용료 계산 함수
    virtual void print(ostream& os) const = 0; // 정보 출력 함수

    // 비교 연산자 오버로딩, allocatedGB 기준 내림차순 정렬, 용량이 같다면 ownerId 기준 오름차순 정렬.
    bool operator<(const StorageUnit& other) const {    
        if (allocatedGB == other.allocatedGB) { return ownerId > other.ownerId; }
        return allocatedGB > other.allocatedGB;
    }
    // 삽입 연산자 오버로딩, 내부에서 print(os) 호출
    friend ostream& operator<<(ostream& os, const StorageUnit& unit) { unit.print(os); return os; }
};

// 하위 클래스 1, 학생 개인용 드라이브
class PersonalStorage : public StorageUnit {
private: int maxBackupCopies; // 무료 백업본 수
public:
    PersonalStorage(string ownerId, double allocatedGB, int maxBackupCopies) : StorageUnit{ ownerId, allocatedGB }, maxBackupCopies{ maxBackupCopies } {}
    // 사용료 계산 함수 오버라이딩
    double calculateCost() const override {
        if (allocatedGB <= 50.0) { return 0; } // 50GB 이하 사용은 무료
        return (allocatedGB - 50.0) * 500.0; // 50GB 초과 시 1GB당 500원
    }
    // 출력 함수, "[개인용] [ownerId] 용량: allocatedGB GB (백업: maxBackupCopies개) -> 월 비용: cost원" 형식 출력.
    void print(ostream& os) const override {
        os << "[개인용] [" << ownerId << "] 용량: " << allocatedGB <<
            "GB (백업: " << maxBackupCopies << "개) -> 월 비용: " << calculateCost() << "원" << endl;
    }
};

// 하위 클래스 2, 연구실 공용 드라이브
class LabStorage : public StorageUnit {
private: 
    string labName;
    char* cacheBuffer; // 동적 버퍼
    size_t bufferSize; // 동적 버퍼 크기
public:
    // 생성자에서 동적 버퍼 크기를 받아 동적 할당
    LabStorage(string ownerId, double allocatedGB, string labName, size_t bufferSize) :
        StorageUnit{ ownerId, allocatedGB }, labName{ labName }, bufferSize{ bufferSize } { cacheBuffer = new char[bufferSize]; }
    virtual ~LabStorage() { delete[] cacheBuffer; } // 가상 소멸자에서 동적 배열 할당 해제

    // 복사 생성자, other의 멤버 변수를 그대로 받아와 복사 후 cacheBuffer에 other가 갖고 있던 것들을 그대로 넣음.
    LabStorage(const LabStorage& other) : StorageUnit{ other.ownerId, other.allocatedGB }, labName{ other.labName }, bufferSize { other.bufferSize } {
        cacheBuffer = new char[bufferSize];
        // std::copy를 이용하여 복사 진행. copy(시작점, 끝점, 복사한 내용을 넣을 곳(destination))
        copy(other.cacheBuffer, other.cacheBuffer + bufferSize, cacheBuffer);
    }
    // 복사 대입 연산자 오버로딩
    LabStorage& operator=(const LabStorage& other) {
        if (this == &other) { return *this; } // 자기 대입 검사
        // other의 멤버 변수 값을 그대로 받아옴.
        ownerId = other.ownerId;
        allocatedGB = other.allocatedGB;
        labName = other.labName;
        bufferSize = other.bufferSize;
        delete[] cacheBuffer; // 지금 갖고 있던 건 지움
        // 이후는 복사 생성자와 똑같음.
        cacheBuffer = new char[bufferSize];
        copy(other.cacheBuffer, other.cacheBuffer + bufferSize, cacheBuffer);
        return *this; // 연속 대입을 가능하게 하도록 자기 자신을 반환.
    }

    // 사용료 계산 함수 오버라이딩
    double calculateCost() const override {
        return 20000.0 + (allocatedGB * 300.0); // 연구실엔 얄짤없이 기본 2만원에 GB당 300원
    }
    // 출력 함수, "[연구실] [ownerId] [labName] 용량: allocatedGB GB -> 월 비용: cost원" 형식 출력.
    void print(ostream& os) const override {
        os << "[연구실] [" << ownerId << "] [" << labName << "] 용량: " <<
            allocatedGB << "GB -> 월 비용: " << calculateCost() << "원" << endl;
    }
};

// 매니저 클래스
class CloudStorageManager {
private:
    vector<StorageUnit*> storageList;
    double totalCapLimit;
    double currentUsedCap;
public:
    // 생성자와 소멸자
    CloudStorageManager(double totalCapLimit) : totalCapLimit{ totalCapLimit }, currentUsedCap{ 0 } {}
    ~CloudStorageManager() { for (StorageUnit* s : storageList) { delete s; } }

    // 추가 함수, 용량 초과의 경우 바로 전달받은 객체를 delete하고 예외를 던진다.
    void addStorage(StorageUnit* unit) {
        if (currentUsedCap + unit->getAllocatedGB() > totalCapLimit) {
            delete unit;
            throw StorageException("클라우드 용량 한도를 초과했습니다.");
        }
        // 용량이 충분하다면 누적하고 벡터에 추가.
        currentUsedCap += unit->getAllocatedGB();
        storageList.push_back(unit);
    }
    // 비교 연산자 오버로딩한 것을 활용하여 정렬하는 함수
    void sortStorage() { sort(storageList.begin(), storageList.end(), [](const StorageUnit* a, const StorageUnit* b) { return *a < *b; }); }

    // 등록된 모든 스토리지 목록을 출력
    void printAll() const { for (StorageUnit* s : storageList) { cout << *s; } }
    
    // 전체 스토리지의 월 사용료 합산
    double getTotalMonthlyRevenue() const {
        double result{ 0 };
        for (StorageUnit* s : storageList) { result += s->calculateCost(); }
        return result;
    }
};

// static 멤버 특, 외부에서 초기화해야 함.
int StorageUnit::activeStorageCount = 0;

int main() {
    SetConsoleOutputCP(65001);

    cout << "=== 1. 초기 활성 스토리지 수 ===" << endl;
    cout << "초기 활성 스토리지: " << StorageUnit::getActiveCount() << "개\n" << endl;

    {
        // 총 용량 한도: 500.0 GB
        CloudStorageManager manager(500.0);

        cout << "=== 2. 스토리지 등록 및 용량 한도 예외 검사 ===" << endl;
        try {
            manager.addStorage(new PersonalStorage("STD01", 30.0, 3));                   // 30GB (무료)
            manager.addStorage(new LabStorage("LAB01", 200.0, "AI로봇연구실", 1024));     // 200GB
            manager.addStorage(new PersonalStorage("STD02", 80.0, 5));                   // 80GB (초과분 30GB 비용 발생)
            manager.addStorage(new LabStorage("LAB02", 250.0, "시스템보안연구실", 2048)); // 250GB (총 560GB -> 한도 초과 예상)
        }
        catch (const StorageException& e) {
            cout << "[예외 감지] " << e.what() << endl;
        }

        cout << "\n=== 3. 등록 후 활성 스토리지 개수 확인 ===" << endl;
        // 4번째 등록 실패 객체는 delete되었으므로 3개여야 함
        cout << "현재 활성 스토리지: " << StorageUnit::getActiveCount() << "개" << endl;

        cout << "\n=== 4. 스토리지 용량 기준 내림차순 정렬 ===" << endl;
        manager.sortStorage();
        manager.printAll();

        cout << "\n=== 5. 월 총 예상 수익 계산 ===" << endl;
        cout << "총 예상 월 회비: " << manager.getTotalMonthlyRevenue() << "원" << endl;

        cout << "\n=== 6. 스코프 종료로 인한 소멸자 호출 직전 ===" << endl;
    } // manager 소멸 -> 모든 등록된 StorageUnit 메모리 해제

    cout << "\n=== 7. 매니저 소멸 후 활성 스토리지 수 ===" << endl;
    cout << "최종 활성 스토리지: " << StorageUnit::getActiveCount() << "개 (0개 확인)" << endl;

    cout << "\n=== 프로그램 정상 종료 ===" << endl;
    return 0;
}