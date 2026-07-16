#include <iostream>
#include <windows.h> 
#include <string>

// extern "C"의 역할
// 이 영역에 선언된 함수들은 C언어 방식으로 빌드되었으니, C 방식으로 링킹하라고 명령 내리는 장치.
// C++은 함수 오버로딩을 지원하기에, 내임 맹글링 메커니즘이 존재. 해당 메커니즘이 C에는 없기 때문에 C 방식 함수를 C++에서 사용할 때 사용한다.

// C++ 컴파일러가 읽을 때만 extern "C"를 켜 준다.
// __cplusplus는 C++ 컴파일러로 빌드할 때만 자동으로 정의되는 내장 매크로.
#ifdef __cplusplus
extern "C" {
#endif
	
	// C 스타일의 전역 함수 정의
	int calculate_checksum(const char* packet, int size) {
		int sum = 0;
		for (int i = 0; i < size; i++) {
			sum += (int)packet[i]; // 문자 하나씩 빼내어 아스키 코드 값을 sum에 더하기
		}
		return sum % 256;
	}

#ifdef __cplusplus
}
#endif
// extern "C" 블록 종료

// 이제 C++의 역할
int main() {
	SetConsoleOutputCP(65001);

	// C++ 스타일의 string 패킷 데이터 준비
	std::string packet = "F1_TELEMETRY_DATA_PACKET";

	// C 스타일 함수에 던져주기 위해 주소(c_str())와 크기(size())를 인자로 전달
	// C++ 컴파일러가 이 함수를 찾을 때, 'extern "C"' 덕분에 
	// 이름이 뒤틀리지 않은 순수한 "calculate_checksum" 심볼을 정상적으로 찾아내어 호출(링킹)에 성공.
	int checksum = calculate_checksum(packet.c_str(), static_cast<int>(packet.size()));

	// 16진수(0x??) 형태로 출력
	std::cout << "최종 계산된 패킷 체크섬 값: 0x" << std::hex << checksum << std::dec << "\n";

	return 0;
}