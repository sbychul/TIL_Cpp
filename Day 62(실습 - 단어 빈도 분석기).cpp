#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 단어 빈도 분석기 (WordFrequencyAnalyzer)
// 강의계획서의 제11강(STL 컨테이너: std::map, std::vector), 제8강/제9강(연산자 중복과 입출력 스트림),
// 제14강(사용자 정의 예외 처리)을 결합한 "STL 기반 단어 빈도 분석 및 사전 관리 시스템 (WordFrequencyAnalyzer)"입니다.
// 텍스트 데이터를 정제하여 단어별 출현 빈도를 연관 컨테이너(std::map)로 자동 집계하고, 빈도수 기준 내림차순 정렬을 위한 std::pair 벡터 변환 및
// 커스텀 비교 함수 연동, 존재하지 않는 단어 조회 시 사용자 정의 예외 처리, 그리고 스트림 출력 연산자(operator<<) 중복을 다룹니다.

// 사용자 정의 예외 클래스
class WordNotFoundException : public std::exception {
public:
    const char* what() const noexcept override {
        return "사전에 등록되지 않은 단어입니다.";
    }
};

// 빈도수 기준 내림차순 정렬 함수, pair: <단어(string), 빈도수(int)>
bool compareByFrequency(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    if (a.second == b.second) { return a.first < b.first; } // 빈도수가 같다면 단어 오름차순
    return a.second > b.second; // 기본적으로는 빈도수 기준 내림차순 정렬
} 

// 단어 빈도 분석기 클래스
class WordFrequencyAnalyzer {
private:
    std::map<std::string, int> frequencyMap;

    // 단어 정제 헬퍼 함수 (구두점 제거 및 소문자 변환)
    std::string cleanWord(const std::string& raw) const {
        std::string cleaned = ""; // 반환할 긴 문자열 선언
        for (char ch : raw) { // 기본적으로 받은 문자열의 문자를 하나씩 순회하며
            if (!std::ispunct(static_cast<unsigned char>(ch))) { 
                // 문장 부호인지 확인하는 함수(std::ispunct())를 사용
                // 문장 부호가 아니라면 소문자로 변환되어(std::tolower) cleaned 문자열에 하나씩 들어감.
                cleaned += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
        }
        return cleaned; // 끝
    }
public:
    WordFrequencyAnalyzer() = default; // 기본 생성자 사용

    // text를 stringstream으로 분리하고 cleanWord()를 거친 후 frequencyMap에 누적하는 함수
    // 단, 정제 후 빈 문자열("")인 토큰은 맵에 넣지 않고 건너뜁니다.
    void processText(const std::string& text) {
        std::stringstream textStream(text); // 일단 받은 문자열을 stringstream으로 분리
        std::string word; // 단어를 담을 빈 문자열
        while (textStream >> word) {
            word = cleanWord(word); // 정제 함수를 거친 뒤
            if (word == "") { continue; } // 빈 문자열인 토큰이 들어올 경우 건너뜀
            frequencyMap[word]++; // 이후 frequencyMap에 해당 단어를 누적 (대괄호 연산자 사용 시 기본값이 0으로 map에 추가되는 것을 활용)
        }
    }

    // 특정 단어의 빈도수 조회 함수
    int getFrequency(const std::string& word) const {
        // find()함수가 발견하지 못하면 end()를 반환하는 특성 활용. 없다면 예외를 던진다.
        auto found = frequencyMap.find(word);
        if (found == frequencyMap.end()) { throw WordNotFoundException(); }
        return found->second; // 이후 빈도수 반환
    }

    // 빈도수 상위 N개 추출 함수
    // - frequencyMap의 모든 요소를 vector<pair<string, int>>에 복사
    // - compareByFrequency 함수를 이용하여 std::sort 수행
    // - 상위 n개(또는 전체 크기가 n보다 작으면 전체)를 새 벡터에 담아 반환
    std::vector<std::pair<std::string, int>> getTopN(size_t n) const {
        std::vector<std::pair<std::string, int>> frequencyVec; // 빈 벡터

        // iterator를 사용하여 모든 요소를 벡터에 복사
        for (auto it = frequencyMap.begin(); it != frequencyMap.end(); it++) {
            frequencyVec.push_back(std::make_pair(it->first, it->second));
        }
        // std::sort에 정렬 함수를 넣어 정렬
        std::sort(frequencyVec.begin(), frequencyVec.end(), compareByFrequency);

        std::vector<std::pair<std::string, int>> result; // 결과를 담을 빈 벡터
        size_t resultSize{ n };
        // 만약 입력받은 n보다 크기가 작다면 인덱스를 초과한 접근 방지
        if (frequencyVec.size() < n) { resultSize = frequencyVec.size(); }
        // 순회하며 처음에 입력받은 상위 n개의 요소만을 담아 반환
        for (size_t i = 0; i < resultSize; i++) { result.push_back(frequencyVec[i]); }
        return result;
    }

    // 전체 단어 목록 사전순 출력 함수 (operator<< 오버로딩, "[단어]: X회" 형식으로 전체 목록을 출력)
    friend std::ostream& operator<<(std::ostream& os, const WordFrequencyAnalyzer& analyzer) {
        for (auto it = analyzer.frequencyMap.begin(); it != analyzer.frequencyMap.end(); it++) {
            os << "[" << it->first << "] " << it->second << "회" << std::endl;
        }
        return os;
    }
};

// 테스트 케이스
int main() {
    SetConsoleOutputCP(65001);

    WordFrequencyAnalyzer analyzer;

    std::string sampleText =
        "C++ is a powerful language. C++ templates and STL make C++ programming robust, "
        "flexible, and expressive. Happy coding with C++ and STL!";

    std::cout << "=== 1. 텍스트 분석 진행 ===" << std::endl;
    analyzer.processText(sampleText);
    std::cout << "-> 텍스트 분석 완료!" << std::endl;

    std::cout << "\n=== 2. 전체 단어 빈도 목록 (사전순 출력) ===" << std::endl;
    std::cout << analyzer;

    std::cout << "\n=== 3. 단어 빈도수 개별 조회 및 예외 테스트 ===" << std::endl;
    try {
        std::cout << "'c++' 빈도수: " << analyzer.getFrequency("c++") << "회" << std::endl;
        std::cout << "'stl' 빈도수: " << analyzer.getFrequency("stl") << "회" << std::endl;
        std::cout << "'java' 빈도수 조회 시도..." << std::endl;
        std::cout << analyzer.getFrequency("java") << "회" << std::endl;
    }
    catch (const WordNotFoundException& e) {
        std::cout << "[단어 조회 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 4. 빈도수 Top 3 단어 추출 ===" << std::endl;
    auto topList = analyzer.getTopN(3);
    int rank = 1;
    for (const auto& item : topList) {
        std::cout << rank++ << "위: " << item.first << " (" << item.second << "회)" << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}