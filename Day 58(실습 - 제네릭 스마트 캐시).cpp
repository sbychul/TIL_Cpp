#include <iostream>
#include <string>
#include <stdexcept>
#include <windows.h>

// 오늘의 종합 과제: 제네릭 스마트 캐시 (SmartCache<K, V>) 구현
// 오늘의 종합 과제는 학부 2학기 강의계획서의 제7강(가상 함수와 추상 클래스), 제8강(연산자 오버로딩),
// 제11강(템플릿 클래스), 제14강(예외 처리 및 자원 관리)을 아우르는 "제네릭 스마트 캐시 시스템 (SmartCache<Key, Value>)"입니다.
// 템플릿 기반 키-값 매핑 구조체, 고정 용량 관리, 원소 접근 및 갱신을 위한 operator[] 연산자 오버로딩,
// 캐시 미스/만료 시 사용자 정의 예외 처리, 그리고 동적 메모리 소멸자 클린업을 결합한 종합 과제입니다.

// 1. 사용자 정의 예외 클래스
// std::exception을 상속받아 what()을 오버라이딩
class CacheNotFoundException : public std::exception {
public:
    const char* what() const noexcept override {
        return "캐시를 찾을 수 없습니다.";
    }
};

// 2. Key-Value 엔트리 구조체
template <typename K, typename V>
struct CacheEntry {
    K key;
    V value;
    CacheEntry(const K& k, const V& v) : key(k), value(v) {}
};

// 3. 템플릿 기반 스마트 캐시 클래스
template <typename K, typename V>
class SmartCache {
private:
    CacheEntry<K, V>** entries; // 캐시 엔트리 포인터 배열
    size_t capacity;
    size_t count;

public:
    // 생성자, 용량을 받아 초기화
    SmartCache(size_t cap = 3) : capacity(cap), count(0) {
        entries = new CacheEntry<K, V>* [capacity];
    }

    // 소멸자, 모든 동적 할당 entry delete 및 entries 배열 delete[]
    ~SmartCache() {
        // 모든 할당된 entry를 delete 후
        for (size_t i = 0; i < count; i++) { delete entries[i]; }
        delete[] entries; // 배열 자체도 제거
    }

    // put 메서드
    void put(const K& key, const V& value) {
        // 이미 존재하는 key라면
        for (size_t i = 0; i < count; i++) {
            if (entries[i]->key == key) { // 존재한다면
                entries[i]->value = value; // value를 새로운 값으로 갱신
                return; // 함수 끝내기  
            }
        }

        // 배열에 여유가 있다면 맨 끝에 새 객체를 할당
        if (count != capacity) { entries[count++] = new CacheEntry<K, V>(key, value); return; }

        // 배열이 꽉 찼다면 FIFO 형식으로 맨 앞의 것을 밀어낸 뒤 맨 뒤에 새 객체를 삽입
        delete entries[0];
        for (size_t i = 0; i < capacity - 1; ++i) { entries[i] = entries[i + 1]; }
        entries[capacity - 1] = new CacheEntry<K, V>(key, value);
        }

    // operator[] 오버로딩 (키로 값 조회)
    V &operator[](const K& key) {
        // 이미 존재하는 key라면 해당 entry의 value 참조를 반환.
        for (size_t i = 0; i < count; i++) {
            if (entries[i]->key == key) { // 존재한다면
                return entries[i]->value;
            }
        }
        // 존재하지 않는다면 커스텀 예외를 던짐.
        throw CacheNotFoundException();
    }

    size_t size() const { return count; }
};

int main() {
    SetConsoleOutputCP(65001);

    std::cout << "=== 1. SmartCache 기본 삽입 및 [] 연산자 조회 ===" << std::endl;
    SmartCache<std::string, int> scoreCache(2); // 최대 2개 저장

    scoreCache.put("Alice", 95);
    scoreCache.put("Bob", 88);

    try {
        std::cout << "Alice의 점수: " << scoreCache["Alice"] << "점" << std::endl;
        std::cout << "Bob의 점수: " << scoreCache["Bob"] << "점" << std::endl;

        // 값 수정 테스트
        scoreCache["Alice"] = 100;
        std::cout << "Alice의 수정된 점수: " << scoreCache["Alice"] << "점" << std::endl;

    }
    catch (const std::exception& e) {
        std::cout << "[예외 발생] " << e.what() << std::endl;
    }

    std::cout << "\n=== 2. FIFO 교체 정책 및 캐시 미스 예외 테스트 ===" << std::endl;
    try {
        // 용량 초과로 인해 가장 오래된 "Alice"가 제거되고 "Charlie"가 들어감
        std::cout << "-> 신규 데이터 'Charlie' 삽입 (용량 초과로 가장 오래된 Alice 교체)" << std::endl;
        scoreCache.put("Charlie", 92);

        std::cout << "Charlie의 점수: " << scoreCache["Charlie"] << "점" << std::endl;

        // Alice는 교체되어 삭제되었으므로 CacheNotFoundException 발생해야 함
        std::cout << "Alice 조회 시도..." << std::endl;
        std::cout << scoreCache["Alice"] << std::endl;

    }
    catch (const CacheNotFoundException& e) {
        std::cout << "[캐시 미스 감지] " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "[기타 예외 감지] " << e.what() << std::endl;
    }

    std::cout << "\n=== 프로그램 정상 종료 ===" << std::endl;
    return 0;
}