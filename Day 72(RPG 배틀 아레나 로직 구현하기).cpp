#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <windows.h>

using namespace std;

// 오늘의 실습 과제: 판타지 RPG 배틀 아레나 시스템 (BattleArenaManager)
// 다양한 직업의 챔피언들을 아레나 풀에 등록하고, 스킬 시전과 대미지 연산을 수행하며,
// 생존 챔피언들의 전투력 랭킹을 산출하여 전투 리포트 파일로 저장하는 시스템입니다.
// 이번 과제는 제6강(static 인스턴스 생명주기 관리), 제7강(복합 대입 및 스트림 연산자 오버로딩: +=, <<), 제9·10강(추상 클래스, 순수 가상 함수, 동적 바인딩 다형성),
// 제11강(STL vector, algorithm, 람다식), 제12·13강(파일 스트림 영속화), 제14강(사용자 정의 예외 처리)까지 6개 핵심 단원을 결합했습니다.

// 커스텀 예외 클래스
class ArenaException : public exception {
private: string msg;
public: 
    // 생성자를 통해 에러 메시지 설정, what()으로 해당 문자열 반환.
    ArenaException(string msg) : msg{ msg } {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// 최상위 추상 클래스
class Champion {
protected:
    string name;
    double currentHp;
    double maxHp;
    double atkPower; // 평타
    static int activeChampionCount;
public:
    Champion(string name, double maxHp, double atkPower) : name{ name }, currentHp{ maxHp }, maxHp{ maxHp }, atkPower{ atkPower } { activeChampionCount++; }
    virtual ~Champion() { activeChampionCount--; }
    
    // getters
    string getName() const { return name; }
    double getCurrentHp() const { return currentHp; }
    double getMaxHp() const { return maxHp; }
    double getAtkPower() const { return atkPower; }
    static int getActiveCount() { return activeChampionCount; }

    // 순수 가상 함수
    virtual void castSkill(Champion& target) = 0; // 각자 스킬이 다를 예정.
    virtual void print(ostream& os) const = 0; // 각자 상태 출력

    // 일정 수치만큼의 피해를 입게 되는 함수
    void takeDamage(double dmg) { currentHp = max(0.0, currentHp - dmg); }
    // 생존 여부 판별
    bool isAlive() const { return currentHp > 0.0; }
    
    // 연산자 오버로딩
    // += 연산자 오버로딩, 입력받은 double 값만큼 대상 Champion의 currentHp에 더함. (maxHp를 초과할 순 없음)
    Champion& operator+=(double healAmount) { currentHp = min(currentHp + healAmount, maxHp); return *this; }
    // 비교 연산자(<) 오버로딩, 현재 체력 기준 내림차순 정렬.
    bool operator<(const Champion& other) const {
        // 체력이 같다면 이름 기준 오름차순 정렬을 진행.
        if (currentHp == other.currentHp) { return name < other.name; }
        return currentHp > other.currentHp;
    }
    // 삽입 연산자 오버로딩, 내부에서 print() 함수를 실행.
    friend ostream& operator<<(ostream& os, const Champion& c) { c.print(os); return os; }
};

// 하위 클래스 1, 전사
class Warrior : public Champion {
private: double shield; // 추가 보호막 수치
public:
    Warrior(string name, double maxHp, double atkPower, double shield) : Champion{ name, maxHp, atkPower }, shield{ shield } {}
    // 스킬 오버라이드, 대상에게 atkPower * 1.5 + shield만큼의 피해를 입힘 (진짜 말도안되게 쎄네)
    void castSkill(Champion& target) override { target.takeDamage(atkPower * 1.5 + shield); } 
    void print(ostream& os) const override { // "[전사] name (HP: currentHp/maxHp, 보호막: shield) - 공격력: atkPower" 형식으로 출력
        os << "[전사] " << name << "(HP: " << currentHp << "/" << maxHp << ", 보호막: " << shield << ") - 공격력: " << atkPower;
    }
};

// 하위 클래스 2, 마법사
class Mage : public Champion {
private: 
    double mana; // 마나
    double spellPower; // AP
public:
    Mage(string name, double maxHp, double atkPower, double mana, double spellPower) : Champion{ name, maxHp, atkPower }, mana{ mana }, spellPower{ spellPower } {}
    // 스킬 오버라이드, 마나가 30.0 이상 있다면 atkPower + (spellPower * 2.0)의 피해를 입힘
    void castSkill(Champion& target) override {
        if (mana < 30.0) { throw ArenaException("마나가 부족하여 스킬을 사용할 수 없습니다."); }
        target.takeDamage(atkPower + (spellPower * 2.0));
        mana -= 30.0;
    }
    void print(ostream& os) const override { // "[마법사] name (HP: hp/maxHp, MP: mana) - 주문력: spellPower" 형식으로 출력
        os << "[마법사] " << name << "(HP: " << currentHp << "/" << maxHp << ", MP: " << mana << ") - 주문력: " << spellPower;
    }
};

// 총괄 매니저 클래스
class BattleArenaManager {
private:
    vector<Champion*> champs;
public:
    BattleArenaManager() = default;
    ~BattleArenaManager() { for (Champion* c : champs) { delete c; } }

    // 추가 함수
    void registerChampion(Champion* c) { champs.push_back(c); }
    // < 연산자 오버로딩을 활용한 정렬 함수
    void sortChampions() { sort(champs.begin(), champs.end(), [](const Champion* a, const Champion* b) { return *a < *b; }); }
    // 전체 상태 출력 함수
    void printAll() const { for (Champion* c : champs) { cout << *c << endl; } }
    // 파일 영속화 함수
    void exportBattleLog(const string& filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) { throw runtime_error("파일을 여는 데 실패했습니다."); }
        for (Champion* c : champs) { outFile << *c << endl; }
    }
};

int Champion::activeChampionCount = 0;

int main() {
    SetConsoleOutputCP(65001);

    cout << "=== 1. 초기 아레나 활성 챔피언 수 ===" << endl;
    cout << "활성 챔피언: " << Champion::getActiveCount() << "명\n" << endl;

    {
        BattleArenaManager arena;

        // 어디서 많이 본 이름들이다...
        Warrior* garen = new Warrior("가렌", 600.0, 60.0, 40.0);   // HP: 600, ATK: 60, Shield: 40
        Mage* ryze = new Mage("라이즈", 400.0, 45.0, 50.0, 80.0);    // HP: 400, ATK: 45, MP: 50, SP: 80
        Warrior* darius = new Warrior("다리우스", 580.0, 65.0, 30.0); // HP: 580, ATK: 65, Shield: 30

        arena.registerChampion(garen);
        arena.registerChampion(ryze);
        arena.registerChampion(darius);

        cout << "=== 2. 챔피언 등록 후 초기 상태 ===" << endl;
        arena.printAll();
        cout << "현재 활성 챔피언: " << Champion::getActiveCount() << "명\n" << endl;

        cout << "=== 3. 턴제 전투 액션 (다형성 및 연산자 오버로딩) ===" << endl;
        // 라이즈 -> 가렌 스킬 공격 (대미지: 45 + 80*2 = 205)
        cout << "-> 라이즈가 가렌에게 비전 폭발 시전!" << endl;
        ryze->castSkill(*garen);

        // 가렌 회복 포션 사용 (operator+=)
        cout << "-> 가렌이 회복 포션을 사용하여 체력 50 회복!" << endl;
        *garen += 50.0;

        // 라이즈 마나 부족 예외 테스트 (현재 MP: 20 -> 필요 MP: 30)
        cout << "\n-> 라이즈가 무리하게 연속 비전 폭발 시도..." << endl;
        try {
            ryze->castSkill(*darius);
        }
        catch (const ArenaException& e) {
            cout << "[스킬 실패 감지] " << e.what() << endl;
        }

        cout << "\n=== 4. 체력 기준 랭킹 정렬 (내림차순) ===" << endl;
        arena.sortChampions();
        arena.printAll();

        cout << "\n=== 5. 전투 결과 파일 저장 ===" << endl;
        try {
            arena.exportBattleLog("battle_result.txt");
            cout << "-> 'battle_result.txt' 파일 저장 완료!" << endl;
        }
        catch (const exception& e) {
            cout << "[파일 에러] " << e.what() << endl;
        }

        cout << "\n=== 6. 아레나 종료 직전 ===" << endl;
    } // arena 소멸 -> 모든 Champion 자동 delete

    cout << "\n=== 7. 아레나 종료 후 활성 챔피언 수 ===" << endl;
    cout << "최종 활성 챔피언: " << Champion::getActiveCount() << "명 (0명 확인)" << endl;

    cout << "\n=== 프로그램 정상 종료 ===" << endl;
    return 0;
}