#pragma once
class NetAddress // 접속하는 클라이언트의 네트워크 주소를 편하게 관리하기 위함 
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr;}
	wstring GetIpAddress();
	uint16 GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR Ip2Address(const WCHAR* ip);
private:
	SOCKADDR_IN _sockAddr = {};
};

