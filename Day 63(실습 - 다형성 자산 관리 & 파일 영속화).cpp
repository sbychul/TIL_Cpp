#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 다형성 자산 관리 & 파일 영속화 시스템 (GenericAssetVault)
// 이번 과제는 2학기 강의계획서의 제6강(상속과 객체 포인터), 제7강(가상 함수와 다형성), 제8·9강(연산자 중복 & 프렌드 스트림),
// 제11강(템플릿 클래스 + STL std::vector), 제12강(파일 입출력 직렬화), 제14강(사용자 정의 예외 처리)까지 총 6개 핵심 단원을 하나로 엮었습니다.

// 사용자 정의 예외 클래스
class AssetException : public std::exception {
private:
    std::string msg;
public:
    // 메시지를 임의로 설정할 수 있는 생성자
    AssetException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

// 최상위 추상 클래스: Asset
class Asset {
protected:
    std::string name;
    double baseValue;

public:
    // 생성자와 가상 소멸자 선언
    Asset(const std::string& n, double val) : name(n), baseValue(val) {}
    virtual ~Asset() {}

    // 현재 가치를 계산하는 순수 가상 함수
    virtual double calculateCurrentValue() const = 0;

    // 스트림 직렬화 순수 가상 함수
    virtual void serialize(std::ostream& os) const = 0;

    // operator<< 오버로딩 (serialize를 호출하는 방식)
    friend std::ostream& operator<<(std::ostream& os, const Asset& asset) {
        asset.serialize(os);
        return os;
    }

    // Getter
    std::string getName() const { return name; }
    double getBaseValue() const { return baseValue; }
};

// Asset 파생 클래스 1: RealEstate (부동산)
class RealEstate : public Asset {
private:
    double rentIncome; // 월 임대 수익
    int holdYears;     // 보유 연수

public:
    // 생성자
    RealEstate(const std::string& n, double base, double rent, int years)
        : Asset(n, base), rentIncome(rent), holdYears(years) {
    }

    // 현재 가치 계산 함수 오버라이딩 (baseValue + 월세비 * 12(개월) * 보유 연수)
    double calculateCurrentValue() const override { return baseValue + rentIncome * 12 * holdYears; }

    // 직렬화 함수 오버라이딩 ("[부동산] 이름: (name) | 원가: (baseValue) | 현재 가치: (함수 계산 결과) | 보유: (holdYears)년" 형식)
    void serialize(std::ostream& os) const override {
        os << "[부동산] 이름: " << name << " | 원가: " << baseValue << " | 현재 가치: " << calculateCurrentValue()
            << " | 보유: " << holdYears << "년" << std::endl;
    }
};

// Asset 파생 클래스 2: Crypto (암호화폐)
class Crypto : public Asset {
private:
    double yieldRate; // 변동률 (0.5 = +50%, -0.3 = -30%)

public:
    Crypto(const std::string& n, double base, double yield)
        : Asset(n, base), yieldRate(yield) {
    }

    // 현재 가치 계산 함수 오버라이딩
    double calculateCurrentValue() const override {
        // 예외 처리
        if (yieldRate < -1.0) { throw AssetException("수익률이 -100% 미만일 수 없습니다."); }
        return baseValue * (1.0 + yieldRate); // 원가 * (1.0 + 변동률);
    }

    // 직렬화 함수 오버라이딩 ("[가상자산] 이름: (name) | 원가: (baseValue) | 현재가치: (함수 계산 결과) | 변동률: (yieldRate * 100)%" 형식)
    void serialize(std::ostream& os) const override {
        os << "[가상자산] 이름: " << name << " | 원가: " << baseValue << " | 현재 가치: " << calculateCurrentValue()
            << " | 변동률: " << yieldRate * 100 << "%" << std::endl;
    }
};

// 템플릿 금고 클래스: GenericAssetVault<T>
template <typename T>
class GenericAssetVault {
private:
    std::vector<T*> assets;

public:
    GenericAssetVault() = default;

    // 소멸자, 모든 동적 자산을 벡터 순회하며 delete
    ~GenericAssetVault() { for (T* asset : assets) { delete asset; } }

    // 삽입 함수
    void addAsset(T* asset) { assets.push_back(asset); }

    // 전체 자산 평가총액 계산 함수
    double getTotalPortfolioValue() const {
        double result{ 0 }; // 결과 담을 변수
        for (T* asset : assets) { result += asset->calculateCurrentValue(); } // 함수 호출하여 합산
        return result;
    }

    // 현재 가치가 가장 높은 자산 포인터 반환 함수
    T* findMaxValuedAsset() const {
        // 예외 처리
        if (assets.empty()) { throw AssetException("금고가 비어있습니다."); }
        T* maxCurrentValue = *assets.begin(); // 가장 높은 자산을 갖고 있을 임시 포인터 변수, 첫 번째 것을 일단 갖고 시작.
        for (T* asset : assets) {
            // 벡터 순회하며 현재 가치가 더 큰 게 있다면 maxCurrentValue가 가리키는 포인터를 변경
            if (asset->calculateCurrentValue() > maxCurrentValue->calculateCurrentValue()) { maxCurrentValue = asset; }
        }
        return maxCurrentValue;
    }

    // 파일 직렬화 저장 함수
    void exportToFile(const std::string& filename) const {
        std::ofstream outFile(filename);
        // 파일 열기 실패 시 예외 던지기
        if (!outFile.is_open()) { throw std::runtime_error("파일 열기 실패"); }
        for (T* asset : assets) { outFile << *asset; } // 삽입 연산자 오버로딩한 것을 이용하여 벡터 돌며 다 저장
    }

    // 콘솔 출력
    void printAll() const {
        for (const T* asset : assets) {
            std::cout << *asset << std::endl;
        }
    }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    GenericAssetVault<Asset> myVault;

    // 1. 다형성 자산 등록
    myVault.addAsset(new RealEstate("강남 오피스텔", 300000000.0, 1200000.0, 3)); // 원가 3억, 월세 120만, 3년
    myVault.addAsset(new Crypto("비트코인", 50000000.0, 0.45));                    // 원가 5000만, +45%
    myVault.addAsset(new RealEstate("송도 상가", 500000000.0, 2000000.0, 2));     // 원가 5억, 월세 200만, 2년
    myVault.addAsset(new Crypto("알트코인", 10000000.0, -0.60));                   // 원가 1000만, -60%

    std::cout << "=== 1. 등록된 전체 자산 목록 ===" << std::endl;
    myVault.printAll();

    std::cout << "\n=== 2. 포트폴리오 총 평가액 및 최고 가치 자산 ===" << std::endl;
    try {
        std::cout << "총 포트폴리오 평가액: " << static_cast<long long>(myVault.getTotalPortfolioValue()) << "원" << std::endl;
        Asset* topAsset = myVault.findMaxValuedAsset();
        std::cout << "최고 가치 자산: " << topAsset->getName()
            << " (평가액: " << static_cast<long long>(topAsset->calculateCurrentValue()) << "원)" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[평가 에러] " << e.what() << std::endl;
    }

    std::cout << "\n=== 3. 파일 영속화 저장 테스트 ===" << std::endl;
    try {
        myVault.exportToFile("day62_portfolio.txt");
        std::cout << "-> 'day62_portfolio.txt' 저장 완료!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[파일 에러] " << e.what() << std::endl;
    }

    std::cout << "\n=== 4. 비정상 수익률 예외 처리 테스트 ===" << std::endl;
    try {
        Crypto badCrypto("루나", 1000000.0, -1.5); // -150% 손실 -> 예외 유발
        badCrypto.calculateCurrentValue();
    }
    catch (const AssetException& e) {
        std::cout << "[자산 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}