// FindHostIP.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
//#include <SFML/Network.hpp>
#include <string>
//#include <Ipexport.h>
#include "CommonSupport.h"
#include <ws2tcpip.h>


#ifdef _WIN32_WCE
#pragma comment(lib,"ws2.lib")
#else
#pragma comment (lib, "ws2_32.lib")
#endif

#pragma comment(lib, "iphlpapi.lib")



int m_socketPrimary;
int m_uIPAddress;

#define MAX_TIMEOUT 1000

#define COFR_Ok 0

unsigned int Disconnect();
unsigned int  Connect();
bool port_is_open(const std::string& address, int port);


int main()
{

	INT32 nResult;
	WSADATA wsaData;

	nResult = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nResult)
	{
		DebugPrint(_T("WSAStartup failed with error: %d\n"), nResult);
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		::WSACleanup();
		return -1;
	}

	//-------------------------------
	bool bis_connect = false;
	std::string str_address = "127.0.0.1";
	int port = 11000;

	//for TEST
	//unsigned int uIPAddress = 0x7f000001;
	//long ulHtoLResult = htonl(uIPAddress);
	//printf("ulHtoLResult = %08X \n", ulHtoLResult);

	bis_connect = port_is_open(str_address, port);
	if (bis_connect)
	{
		printf("[port open] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}
	else
	{
		printf("[port closed] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}

	str_address = "192.168.0.221";
	port = 9600;
	bis_connect = port_is_open(str_address, port);
	if (bis_connect)
	{
		printf("[port open] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}
	else
	{
		printf("[port closed] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}

	str_address = "192.168.0.232";
	port = 9600;
	bis_connect = port_is_open(str_address, port);
	if (bis_connect)
	{
		printf("[port open] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}
	else
	{
		printf("[port closed] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}

	str_address = "192.168.0.221";
	port = 9600;
	bis_connect = port_is_open(str_address, port);
	if (bis_connect)
	{
		printf("[port open] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}
	else
	{
		printf("[port closed] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}


	str_address = "192.168.0.232";
	port = 9600;
	bis_connect = port_is_open(str_address, port);
	if (bis_connect)
	{
		printf("[port open] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}
	else
	{
		printf("[port closed] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}

	str_address = "127.0.0.1";
	port = 11000;
	bis_connect = port_is_open(str_address, port);
	if (bis_connect)
	{
		printf("[port open] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}
	else
	{
		printf("[port closed] ip addr = %s / port = %d \n ", str_address.c_str(), port);
	}
	//--------------------------------
	::WSACleanup();

	return 0;
}




bool port_is_open(const std::string& address, int port)
{
	bool is_open = false;
   /* sf::TcpSocket socket;
    bool open = (socket.connect(sf::IpAddress(address), port) == sf::Socket::Done);
    socket.disconnect();*/

	INT nRetVal;
	SOCKADDR_IN sin;
	INT32 nResult;
	BOOL bSuccess;
	UINT32 uSendTimeout, uReceiveTimeout;
	//DebugPrint(_T("FindHostIPCheck::Port Check()\n"));
	unsigned long _ipaddr_integer;
	in_addr _in_addr_ip_address;

	char cstr[16];
	memset(cstr, 0, sizeof(cstr));
	strcpy_s(cstr, address.c_str());
	//printf("[port is open] Address = %s \n", cstr);

	//in_addr_t inet_addr(const char* cp);
	nRetVal = inet_pton(AF_INET, cstr, &_in_addr_ip_address);
	if (nRetVal != 1) 
	{
		nResult = WSAGetLastError();
		//
		is_open = false;
		return is_open;
	}
	_ipaddr_integer = _in_addr_ip_address.S_un.S_addr;
	//printf("[port is open] InetPton::success.. \n IP = %s ==> 0x%08X \n", cstr, _ipaddr_integer);
	
	if (m_socketPrimary != NULL)
	{
		//
		is_open = false;
		return is_open;
	}

	bSuccess = FALSE;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = _ipaddr_integer;//htonl(m_uIPAddress);
	sin.sin_port = htons(port);

	m_socketPrimary = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socketPrimary == INVALID_SOCKET)
	{
		m_socketPrimary = NULL;
		//
		is_open = false;
		return is_open;
	}

	uSendTimeout = uReceiveTimeout = MAX_TIMEOUT;
	nResult = setsockopt(m_socketPrimary, SOL_SOCKET, SO_SNDTIMEO, (const char*)&uSendTimeout, sizeof(UINT));
	nResult = setsockopt(m_socketPrimary, SOL_SOCKET, SO_RCVTIMEO, (const char*)&uReceiveTimeout, sizeof(UINT));
	nResult = connect(m_socketPrimary, (struct sockaddr*)&sin, sizeof(sin));
	//성공: 0 반환
	//실패 : -1 (Linux), SOCKET_ERROR(Windows)

	if (nResult == SOCKET_ERROR)
	{
		nResult = WSAGetLastError();
		Disconnect();
		//
		is_open = false;
		return is_open;
	}

	if (m_socketPrimary != NULL)
	{
		closesocket(m_socketPrimary);
		m_socketPrimary = NULL;
		//
		is_open = true;
		return is_open;
	}
	else
	{ 
		//
		is_open = false;
	}


	return is_open;
}



unsigned int  Connect()
{
	SOCKADDR_IN sin;
	INT32 nResult;
	BOOL bSuccess;
	UINT32 uSendTimeout, uReceiveTimeout;
	DebugPrint(_T("CFinsDevice::Connect()\n"));


	if (m_socketPrimary != NULL)
		return 1;


	bSuccess = FALSE;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(m_uIPAddress);
	sin.sin_port = htons(9600);

	m_socketPrimary = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socketPrimary == INVALID_SOCKET)
	{
		m_socketPrimary = NULL;
		return 1;
	}

	uSendTimeout = uReceiveTimeout = MAX_TIMEOUT;
	nResult = setsockopt(m_socketPrimary, SOL_SOCKET, SO_SNDTIMEO, (const char*)&uSendTimeout, sizeof(UINT));
	nResult = setsockopt(m_socketPrimary, SOL_SOCKET, SO_RCVTIMEO, (const char*)&uReceiveTimeout, sizeof(UINT));
	nResult = connect(m_socketPrimary, (struct sockaddr*)&sin, sizeof(sin));
	
	if (nResult == SOCKET_ERROR)
	{
		nResult = WSAGetLastError();
		Disconnect();
		return nResult;
	}

	

	return COFR_Ok;
}


unsigned int  Disconnect()
{
	if (m_socketPrimary != NULL)
	{
		closesocket(m_socketPrimary);
		m_socketPrimary = NULL;
	}

	return COFR_Ok;
}

//#include <netdb.h>
//#include <ifaddrs.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//
//
//int main(int argc, char* argv[])
//{
//
//    int s;
//
//    char host[NI_MAXHOST];
//    struct ifaddrs* ifaddr, * ifa;
//
//    if (getifaddrs(&ifaddr) < 0)
//    {
//        printf("ERROR :: %s() : getifaddrs()\n", __FUNCTION__);
//        return -1;
//    }
//
//
//    memset(host, 0x00, sizeof(host));
//    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
//    {
//        if (ifa->ifa_addr == NULL)
//            continue;
//
//        s = getnameinfo(ifa->ifa_adddr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0,
//            NI_NUMERICHOST);
//
//        if (ifa->ifa_addr->sa_family == AF_INET)
//        {
//            if (scrcmp(ifa->ifa_name, "lxcbr0") == 0 || strcmp(ifa->ifa_name, "mnet0") == 0
//                || strcmp(ifa->ifa_name, "anet0") == 0)
//            {
//                if (s != 0)
//                {
//                    printf("getnameinfo() faied: %s\n", gai_strerror(s));
//                    freeifaddrs(ifaddr);
//                    return -1;
//                }
//
//                printf("\tInterface : <%s>\n", ifa->ifa_name);
//                printf("\tAddress   : <%s>\n", host);
//            }
//        }
//    }
//    freeifaddrs(ifaddr);
//}


//
//AStringVector cNetwork::EnumLocalIPAddresses(void)
//{
//	AStringVector res;
//
//#ifdef _WIN32
//
//	// Query the OS for all adapters' addresses:
//	char buffer[64 KiB];  // A buffer backing the address list
//	PIP_ADAPTER_ADDRESSES pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(&buffer);
//	ULONG outBufLen = sizeof(buffer);
//	DWORD dwRetVal = GetAdaptersAddresses(
//		AF_UNSPEC,
//		GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME, nullptr,
//		pAddresses, &outBufLen
//	);
//	if (dwRetVal != ERROR_SUCCESS)
//	{
//		LOG("GetAdaptersAddresses() failed: %u", dwRetVal);
//		return res;
//	}
//
//	// Enumerate all active adapters
//	for (auto pCurrAddresses = pAddresses; pCurrAddresses != nullptr; pCurrAddresses = pCurrAddresses->Next)
//	{
//		if (pCurrAddresses->OperStatus != IfOperStatusUp)
//		{
//			// Adapter not active, skip it:
//			continue;
//		}
//
//		// Collect all IP addresses on this adapter:
//		for (auto pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != nullptr; pUnicast = pUnicast->Next)
//		{
//			auto Address = PrintAddress(pUnicast->Address);
//			if (!Address.empty())
//			{
//				res.push_back(Address);
//			}
//		}  // for pUnicast
//	}  // for pCurrAddresses
//
//#else  // _WIN32
//
//	struct ifaddrs* ifAddrStruct = nullptr;
//	getifaddrs(&ifAddrStruct);
//
//	for (auto ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
//	{
//		if (ifa->ifa_addr == nullptr)
//		{
//			continue;
//		}
//
//		auto Address = PrintAddress(ifa);
//		if (!Address.empty())
//		{
//			res.emplace_back(Address);
//		}
//	}
//
//	if (ifAddrStruct != nullptr)
//	{
//		freeifaddrs(ifAddrStruct);
//	}
//
//#endif  // else _WIN32
//
//	return res;
//}