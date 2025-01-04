#include <stdio.h>
#include <WinSock2.h>		//소켓을 사용하기 위한 헤더

//ws2_32 = lib파일(실제 구현 부분을 바이너리화)
// -> WinSock2.h에 선언한 기능들을 사용하겠다고 라이브러리 링크를 걸어주는 것
#pragma comment(lib, "ws2_32")

#define PORT			2822	//임의의 포트번호
#define PACKET_SIZE		1024

int main()
{
	//소켓 초기화 정보를 저장하기 위한 구조체
	WSADATA wsaData;

	//WSAStartup(소켓버전, WSADATA 구조체 주소) -> 생성자와 비슷한 개념
	//MAKEWORD(하위 8비트, 상위 8비트) -> 0x0202
	//소켓을 사용하는 것은 WSAStartup 함수와 WSACleanup 함수 사이에 작성
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hListen;		//핸들
	//PF_INET = IPV4 타입 사용
	//SOCK_STREAM = 연결지향형 소켓
	//IPPROTO_TCP = TCP 사용
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//소켓의 구성요소를 담을 구조체 생성 및 값 할당
	SOCKADDR_IN tListenAddr = {};
	//반드시 AF_INET! 인자 설명 찾아보면 정의되어있음
	tListenAddr.sin_family = AF_INET;
	//htons = host to network short -> 무조건 빅엔디안 방식으로 데이터 변환하여 설정
	tListenAddr.sin_port = htons(PORT);
	//s_addr = IPv4 Internet address
	//INADDR_ANY = 현재 컴퓨터 IP 주소로 설정(로컬로 작동시키기 때문)
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind(소켓, 소켓 구성요소 구조체의 주소, 구조체의 크기)
	//소켓에 주소 정보를 연결
	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	//소켓을 접속 대기 상태로 전환
	listen(hListen, SOMAXCONN);

	//클라이언트 측 소켓 생성, 정보를 담을 구조체 생성 및 값 할당
	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);

	//accept(소켓, 소켓 구성요소 구조체의 주소, 구조체 크기를 담고 있는 변수의 주소)
	//accept 함수를 이용해 접속 요청을 수락 -> 동기화된 방식으로 동작(요청 마무리 전까지 계속 대기상태)
	//접속 요청 승인 시 연결된 소켓이 만들어져서 리턴
	SOCKET hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);

	char cBuffer[PACKET_SIZE] = {};
	//recv(소켓, 수신 정보를 담을 배열 주소, 배열의 크기, flag)
	recv(hClient, cBuffer, PACKET_SIZE, 0);
	printf("%s\n", cBuffer);

	char cMsg[] = "Hello Client!";
	//send(소켓, 수신 정보를 담을 배열 주소, 배열의 크기, flag)
	send(hClient, cMsg, strlen(cMsg), 0);

	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();	//WSAStartup을 하면서 지정된 내용을 지워줌, 소멸자와 비슷한 개념
	return 0;
}