#include "pch.h"
#include "CFinsUDPDevice.h"







#include "pch.h"

#include "../Include/CJPSOmronFinsDef.h"
#include "CommonSupport.h"

#include <Ipexport.h>
#include <Icmpapi.h>
#include <ws2tcpip.h>

CFinsUDPDevice::CFinsUDPDevice()
{
	m_uIPAddress = 0;
	m_uDeviceID = 0;
	m_socketPrimary = NULL;
	m_hMutex = NULL;
	//
	
	GetByteOrder();
	HostLinkHeaderSetting();
}

CFinsUDPDevice::CFinsUDPDevice(UINT32 uIPAddress, UINT16 uPortNumber, UINT32 uDeviceID)
{
	m_uIPAddress = uIPAddress;
	m_uDeviceID = uDeviceID;
	m_uPortNumber = uPortNumber;
	//
	m_socketPrimary = NULL;
	//
	
	GetByteOrder();
	HostLinkHeaderSetting();

	TCHAR tszBuffer[MAX_PATH];
	_stprintf_s(tszBuffer, _countof(tszBuffer), _T("Ethernet-%08X"), uIPAddress);
	m_hMutex = CreateMutex(NULL, FALSE, tszBuffer);
}


CFinsUDPDevice::~CFinsUDPDevice()
{
	if (m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}

VOID CFinsUDPDevice::HostLinkHeaderSetting()
{

	//0x80 00 02 고정
	m_fins_header.icf = 0x80; //(Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 "80" 고정
	m_fins_header.rsv = 0x00; //(Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 "00" 고정
	m_fins_header.gct = 0x02; //(Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 "02" PLC <-> PC

	//PLC Network 번호
	m_fins_header.dna = 0x00;//0x01; //(Destination Network Address) - PLC Local network 번호 "01"
	//PLC 노드번호(전송받은 번호)
	//Response 에서 Error이 0x00 이 아닐경우 접속을 끊고 재접속하여 다시 노드번호를 받아야함.
	m_fins_header.da1 = 0xFF; //(Destination Node Address) - PLC Node 번호 "0A"
	//PLC CPU Unit 번호(00으로 고정) 
	m_fins_header.da2 = 0x00; //(Destination Node Address) – Host Link 모드에서 목적지가 CPU Unit일 경우 "00"

	//PC Network번호
	m_fins_header.sna = 0x00;//0x01; //(Source Network Address) - PC 네트워크 번호 "01"
	//PC 노드번호(자동할당 받은 번호)
	m_fins_header.sa1 = 0xFF; //(Source Unit Address) - PC Node 번호 "EF"->자동할당으로 받은 Node 번호
	//PC CPU 유닛 번호(00 고정)
	m_fins_header.sa2 = 0x00; //(Source Unit Address) - CPU Unit에 연결된 경우 "00"

	//Sequence Number( 00 ~ FF 까지 계속 증가 )
	m_fins_header.sid = 0x00; //(Source ID) - 재전송시 카운터로사용됨 "00" (00~FF 사이의 수 입력)


	// FINS Command code (READ = 0101) 지령관련 헤더셋팅

	m_read_req_datagram.icf = m_fins_header.icf;
	m_read_req_datagram.rsv = m_fins_header.rsv;
	m_read_req_datagram.gct = m_fins_header.gct;
	//
	m_read_req_datagram.plc_network_no = m_fins_header.dna;
	m_read_req_datagram.plc_node_no = m_fins_header.da1;
	m_read_req_datagram.plc_cpu_unit = m_fins_header.da2;
	//
	m_read_req_datagram.pc_network_no = m_fins_header.sna;
	m_read_req_datagram.pc_node_no = m_fins_header.sa1;
	m_read_req_datagram.pc_cpu_unit = m_fins_header.sa2;
	//
	m_read_req_datagram.sequence = 0x00;
	m_read_req_datagram.fins_cmd_code[0] = 0x01;
	m_read_req_datagram.fins_cmd_code[1] = 0x01;
	m_read_req_datagram.data_area_type = 0x82;  //DM영역
	m_read_req_datagram.data_start_offset[0] = 0x00;
	m_read_req_datagram.data_start_offset[1] = 0x00;
	m_read_req_datagram.bit_address = 0x00;
	m_read_req_datagram.read_data_length[0] = 0x00;
	m_read_req_datagram.read_data_length[1] = 0x00;


	// FINS Command code (Write = 0102)
	//	m_write_req_datagram
	//
	m_write_req_datagram.icf = m_fins_header.icf; //(Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 “80” 고정
	m_write_req_datagram.rsv = m_fins_header.rsv; //(Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 “00” 고정
	m_write_req_datagram.gct = m_fins_header.gct;; //(Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 “02” PLC <-> PC
	//
	m_write_req_datagram.plc_network_no = m_fins_header.dna;
	m_write_req_datagram.plc_node_no = m_fins_header.da1;
	m_write_req_datagram.plc_cpu_unit = m_fins_header.da2;
	//
	m_write_req_datagram.pc_network_no = m_fins_header.sna;
	m_write_req_datagram.pc_node_no = m_fins_header.sa1;
	m_write_req_datagram.pc_cpu_unit = m_fins_header.sa2;
	//
	m_write_req_datagram.sequence = 0x00;			// Sequence(00~FF까지 자동 증가)
	//
	m_write_req_datagram.fins_cmd_code[0] = 0x01; // FINS Command code(Write = 0102)
	m_write_req_datagram.fins_cmd_code[1] = 0x02; // FINS Command code(Write = 0102)
	//
	m_write_req_datagram.data_area_type = 0x82;  //	Data Type(ex.DM의 경우 82)
	//
	m_write_req_datagram.data_start_offset[0] = 0x00;
	m_write_req_datagram.data_start_offset[1] = 0x00;

	m_write_req_datagram.bit_address = 0x00;

	m_write_req_datagram.write_data_length[0] = 0x00;
	m_write_req_datagram.write_data_length[1] = 0x00;
}

//UINT32 CFinsUDPDevice::GetFinsErrorCode(PCHAR pszResponse, INT nLength)
//{
//	UINT32 uErrorCode = 0;
//	UINT32 uOneByte[04] = { 0,0,0,0 };
//
//	//bit12 ~ 15 : Error Code - 0인지확인
//	uOneByte[00] = (pszResponse[15] << 0) & 0x000000FF;
//	uOneByte[01] = (pszResponse[14] << 8) & 0x0000FF00;
//	uOneByte[02] = (pszResponse[13] << 16) & 0x00FF0000;
//	uOneByte[03] = (pszResponse[12] << 24) & 0xFF000000;
//
//	uErrorCode = (uOneByte[00] | uOneByte[01] | uOneByte[02] | uOneByte[03]);
//
//	return uErrorCode;
//}

//수동으로 셋팅하는 부분도 만들자
UINT32 CFinsUDPDevice::SetNodeAddress(INT32 nDestAddr, INT32 nSourceAddr)
{
	m_fins_header.da1 = (nDestAddr & 0x00FF); //Server Address
	m_fins_header.sa1 = (nSourceAddr & 0x00FF); //Client Address
	//
	//			
	m_read_req_datagram.plc_node_no = m_fins_header.da1;
	m_read_req_datagram.pc_node_no = m_fins_header.sa1;
	//
	m_write_req_datagram.plc_node_no = m_fins_header.da1;
	m_write_req_datagram.pc_node_no = m_fins_header.sa1;
	//
	return 0;
}
//IP를 기반으로 자동으로 노드 주소 생성 
UINT32 CFinsUDPDevice::GetNodeAddress(PINT32 pnErrorCode)
{
	UINT32 uResult;
	IN_ADDR inAddr;
	INT32 iMyPCLastDigit;
	INT32 iPLCLastDigit;
	//
	*pnErrorCode = 0;

	DebugPrint(_T("[DebugPrint][FinsDLL] CFinsUDPDevice::GetNodeAddress() call \n"));

	if (m_socketPrimary == NULL)
		return COFR_NotConnected;

	Lock();

	uResult = COFR_Ok;

	//UDP 모드일때는
	//PLC Node번호 = PLC IP 번호 끝자리
	//PC 노트 번호 = PC IP 끝자리

	inAddr = GetLastDigitOfHostIP();
	iMyPCLastDigit = (inAddr.S_un.S_addr & 0x000000FF);
	iPLCLastDigit  = (m_uIPAddress & 0x000000FF);
	//본인의 PC IP는 169.254.x.x 대역의 IP를 제외하고 2번째 검색되는 IP의 끝자리를 사용
	//그것밖에 안나오면 169.254.x.x 대역 사용

	m_fins_header.da1 = (iPLCLastDigit  & 0x00FF); //Server Address
	m_fins_header.sa1 = (iMyPCLastDigit & 0x00FF); //Client Address

	//			
	m_read_req_datagram.plc_node_no = m_fins_header.da1;
	m_read_req_datagram.pc_node_no = m_fins_header.sa1;
	//
	m_write_req_datagram.plc_node_no = m_fins_header.da1;
	m_write_req_datagram.pc_node_no = m_fins_header.sa1;
	//

	Unlock();

	return uResult;

}



IN_ADDR CFinsUDPDevice::GetLastDigitOfHostIP()
{
	char localhostname[MAX_PATH];
	IN_ADDR addr = { 0, };

	addr.S_un.S_addr = 0x000000EF;  //임의의 기본값 설정

	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR)//호스트 이름 얻어오기
	{
		return addr;
	}

	char client_ip[20] = { 0, };
	addrinfo addrHint = { 0, };
	addrHint.ai_socktype = SOCK_STREAM;
	addrHint.ai_protocol = IPPROTO_TCP;
	addrHint.ai_family = AF_INET;
	//
	addrinfo* pAddrInfo = nullptr;
	addrinfo* pAddrInfo_prev = nullptr;
	addrinfo* pAddrInfo_next = nullptr;
	//char nodeName[32] = "brunch.co.kr";
	char serviceName[6] = "http";
	int res = getaddrinfo(localhostname, serviceName, &addrHint, &pAddrInfo);
	CHAR _ip_4_digit[4];
	bool _continue;
	if (res == 0)
	{
		//inet_ntop(AF_INET, &pAddrInfo->ai_addr->sa_data[2], &client_ip[0], 16);
		//printf("%s\n", client_ip);//IPv4 주소를 문자열로 출력
		pAddrInfo_prev = pAddrInfo;

		_ip_4_digit[0] = pAddrInfo->ai_addr->sa_data[2];
		_ip_4_digit[1] = pAddrInfo->ai_addr->sa_data[3];

		//169.254.x.x 
		if (_ip_4_digit[0] == 169 && _ip_4_digit[1] == 254)
		{
			_continue = true;
			while (pAddrInfo_prev->ai_next != nullptr && _continue)
			{
				pAddrInfo_next = pAddrInfo_prev->ai_next;
				//inet_ntop(AF_INET, &pAddrInfo_next->ai_addr->sa_data[2], &client_ip[0], 16);
				//printf("%s\n", client_ip);//IPv4 주소를 문자열로 출력
				pAddrInfo_prev = pAddrInfo_next;

				_ip_4_digit[0] = pAddrInfo_next->ai_addr->sa_data[2];
				_ip_4_digit[1] = pAddrInfo_next->ai_addr->sa_data[3];

				if (_ip_4_digit[0] == 169 && _ip_4_digit[1] == 254)	{
					//nop
				}else
				{
					addr.S_un.S_un_b.s_b1 = pAddrInfo_next->ai_addr->sa_data[2];
					addr.S_un.S_un_b.s_b2 = pAddrInfo_next->ai_addr->sa_data[3];
					addr.S_un.S_un_b.s_b3 = pAddrInfo_next->ai_addr->sa_data[4];
					addr.S_un.S_un_b.s_b4 = pAddrInfo_next->ai_addr->sa_data[5];
					//
					_continue = false;
				}
			}
		}
		else
		{
			addr.S_un.S_un_b.s_b1 = pAddrInfo->ai_addr->sa_data[2];
			addr.S_un.S_un_b.s_b2 = pAddrInfo->ai_addr->sa_data[3];
			addr.S_un.S_un_b.s_b3 = pAddrInfo->ai_addr->sa_data[4];
			addr.S_un.S_un_b.s_b4 = pAddrInfo->ai_addr->sa_data[5];
		}
	}//end if (res == 0)
	else 
	{
		//std::cout << "gethostbyname error:" << gai_strerror(res);
		addr.S_un.S_addr = 0x000000EF;	
	}

	if (pAddrInfo != nullptr)
	{
		freeaddrinfo(pAddrInfo);
	}

	return addr;
}

UINT32 CFinsUDPDevice::GetFinsResponseCode(PCHAR pszResponse, INT nLength)
{
	UINT32 uResponseCode = 0;
	UINT32 uOneByte[04] = { 0,0,0,0 };

	//bit12 ~ 13 : Response Code - 0인지확인
	uOneByte[00] = (pszResponse[12] << 0) & 0x000000FF;
	uOneByte[01] = (pszResponse[13] << 8) & 0x0000FF00;
	

	uResponseCode = (uOneByte[00] | uOneByte[01] | uOneByte[02] | uOneByte[03]);

	return uResponseCode;
}

UINT32 CFinsUDPDevice::FinsMemRead(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode)
{
	UINT32 uResult = 0;
	UINT32 uTempInt32 = 0;
	UINT32 uTempInt32_2 = 0;
	byte2_t uTemp16;
	INT32   _idx;

	//UINT32 uResult;
	INT32 nResult, nCommandLength, nResponseLength;
	PCHAR pszCommand, pszResponse;
	UINT32 uErrorCode;
	UINT32 uDataLength;

	*uFinsErrorCode = 0;

	if (m_socketPrimary == NULL)
		return COFR_NotConnected;

	Lock();

	//
	// 테스트 해보고 바이트 오더 고려할것
	uTempInt32 = m_read_req_datagram.sequence++;
	m_read_req_datagram.sequence = (unsigned char)(uTempInt32 & 0x00FF);
	//
	uTemp16.uint16_val = htons((unsigned short)nSize);;
	m_read_req_datagram.read_data_length[1] = uTemp16.byte_val[1];
	m_read_req_datagram.read_data_length[0] = uTemp16.byte_val[0];
	//
	uTemp16.uint16_val = htons((unsigned short)uAddress);
	m_read_req_datagram.data_start_offset[1] = uTemp16.byte_val[1];
	m_read_req_datagram.data_start_offset[0] = uTemp16.byte_val[0];
	//

	uResult = COFR_Ok;
	
	//UDP 통신이면 앞에 16바이트 빠진다.
	nCommandLength  = 18;  //고정값, Fins Read Memory Data Request Frame Size
	nResponseLength = 14 + (nSize * 2); //Fins Read Memory Data Response Frame Size

	uTempInt32 = 8 - (nCommandLength % 8);
	uTempInt32_2 = 8 - (nResponseLength % 8);

	pszCommand = new CHAR[(nCommandLength + uTempInt32)];
	pszResponse = new CHAR[(nResponseLength + uTempInt32_2)];

	memset(pszCommand, 0, (nCommandLength + uTempInt32));
	memset(pszResponse, 0, (nResponseLength + uTempInt32_2));

	//void* memcpy (void* dest, const void* source, size_t num)
	
	//UDP면 앞에 16바이트가 빠진다.
	memcpy(pszCommand, (char*)(&m_read_req_datagram) , nCommandLength);
	nResult = sendto(m_socketPrimary, pszCommand, nCommandLength, 0,
		(SOCKADDR*)&m_plc_addr, sizeof(m_plc_addr));

	
	if (nResult == SOCKET_ERROR)
		uResult = WSAGetLastError();

	if (uResult == COFR_Ok)
	{
		//DebugPrint(_T("[DebugPrint][FinsDLL] ReadVariableData() %d [byte] call \n"), nResponseLength);
		uResult = ReadVariableBinaryData((char*)&m_read_res_datagram, nResponseLength);
		//DebugPrint(_T("[DebugPrint][FinsDLL] ReadVariableData() return \n"));

		switch (uResult)
		{
		case COFR_ResLengthExceeds:
			ReadDummyData();

		case COFR_Ok:

			// bit12 ~ 13 : Response code: Normal completion (0x0000)
			// 응답코드가 0 인지 확인
			// Response code: Data cannot be changed (0x2108) 이런거 나올수도 있음.
			uErrorCode = GetFinsResponseCode((char*)&m_read_res_datagram, nResponseLength);
			if (uErrorCode != 0x0000)
			{
				//에러면 통신을 끊어야 될듯
				*uFinsErrorCode = uErrorCode;
				uResult = COFR_PLCErrorOmronFINS;
			}
			else
			{
				//성공이면 14바이트 빼고 상위로 복사
				uDataLength = (nSize * 2);
				for (_idx = 0; _idx < uDataLength; _idx += 2)
				{
					pszResponse[_idx]		= m_read_res_datagram.bData[(_idx + 1)];
					pszResponse[(_idx + 1)]= m_read_res_datagram.bData[_idx];
				}

				memcpy(pValue, (void *)&pszResponse[0], uDataLength);
				uResult = COFR_Ok;
			}
			break;
		}
	}

	delete[] pszCommand;
	delete[] pszResponse;

	Unlock();

	return uResult;
}


UINT32 CFinsUDPDevice::FinsMemWrite(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode)
{
	UINT32 uResult = 0;
	//
	UINT32 uTempInt32 = 0;
	UINT32 uTempInt32_2 = 0;
	UINT32 uDataLength = 0;
	byte2_t uTemp16;

	//UINT32 uResult;
	INT32 nResult, nCommandLength, nResponseLength;
	PCHAR pszCommand, pszResponse;
	UINT32 uErrorCode;
	*uFinsErrorCode = 0;

	if (m_socketPrimary == NULL)
		return COFR_NotConnected;


	Lock();

	//
	// 테스트 해보고 바이트 오더 고려할것
	// htonl((uint32_t)stData);
	//m_write_req_datagram.sequence++;
	//m_write_req_datagram.write_data_length = (unsigned short)nSize;
	//m_write_req_datagram.data_start_offset = (unsigned short)uAddress;

	//memcpy(m_write_req_packet.bData, pValue, nSize);
	//
	// 테스트 해보고 바이트 오더 고려할것
	uTempInt32 = m_write_req_datagram.sequence;
	uTempInt32 = uTempInt32 + 1;
	m_write_req_datagram.sequence = (unsigned char)(uTempInt32 & 0x00FF);
	//
	uTemp16.uint16_val = htons((unsigned short)(nSize));;
	m_write_req_datagram.write_data_length[1] = uTemp16.byte_val[1];
	m_write_req_datagram.write_data_length[0] = uTemp16.byte_val[0];
	//
	uTemp16.uint16_val = htons((unsigned short)uAddress);
	m_write_req_datagram.data_start_offset[1] = uTemp16.byte_val[1];
	m_write_req_datagram.data_start_offset[0] = uTemp16.byte_val[0];
	//
	
	//void* memcpy (void* dest, const void* source, size_t num)
	uDataLength = (nSize * 2);
	for (int i = 0; i < uDataLength; i += 2)
	{
		//pszResponse[(i)] = m_read_res_datagram.bData[(i+1)];
		//pszResponse[(i+1)] = m_read_res_datagram.bData[i];

		m_write_req_datagram.bData[(i + 1)] = ((char*)pValue)[(i)];
		m_write_req_datagram.bData[i] = ((char*)pValue)[(i + 1)];
	}
	//memcpy((char*)m_write_req_datagram.bData, (char*)pValue, (size_t)(nSize*2));
	//

	uResult = COFR_Ok;
	
	//UDP 통신이면 앞에 16바이트 빠진다.
	nCommandLength = 18 + (nSize * 2);  //Fins Write Memory Data Request Frame Size
	nResponseLength = 14; //Fins Write Memory Data Response Frame Size, 고정값
	
	//printf("\n [DEBUG] FinsMemWrite() call - Req[%d]/Resp[%d] ", nCommandLength, nResponseLength);

	uTempInt32 = 8 - (nCommandLength % 8);
	uTempInt32_2 = 8 - (nResponseLength % 8);

	pszCommand = new CHAR[(nCommandLength + uTempInt32)];
	pszResponse = new CHAR[(nResponseLength + uTempInt32_2)];

	memset(pszCommand, 0, (nCommandLength + uTempInt32));
	memset(pszResponse, 0, (nResponseLength + uTempInt32_2));


	//void* memcpy (void* dest, const void* source, size_t num)
	memcpy(pszCommand, (char*)(&m_write_req_datagram) , nCommandLength);
	nResult = sendto(m_socketPrimary, pszCommand, nCommandLength, 0, (SOCKADDR*)&m_plc_addr, sizeof(m_plc_addr));
	
	
	if (nResult == SOCKET_ERROR)
		uResult = WSAGetLastError();

	if (uResult == COFR_Ok)
	{
		uResult = ReadVariableBinaryData(pszResponse, nResponseLength);

		switch (uResult)
		{
		case COFR_ResLengthExceeds:
			ReadDummyData();

		case COFR_Ok:

			//bit12 ~ 13 : Response Code - 0인지확인
			uErrorCode = GetFinsResponseCode(pszResponse, nResponseLength);
			if (uErrorCode != 0x0000)
			{
				//에러면 통신을 끊어야 될듯
				*uFinsErrorCode = uErrorCode;
				uResult = COFR_PLCErrorOmronFINS;
			}
			else
			{
				*uFinsErrorCode = 0;
				uResult = COFR_Ok;
			}
			break;
		}
	}

	delete[] pszCommand;
	delete[] pszResponse;

	Unlock();

	return uResult;
}


//-------------------------
UINT32 CFinsUDPDevice::IsFinsConnected()
{
	if (m_socketPrimary != NULL)
		return COFR_Connected;
	else
		return COFR_NotConnected;
}





UINT32 CFinsUDPDevice::Connect()
{
	SOCKADDR_IN sin;
	INT32 nResult;
	BOOL bSuccess;
	UINT32 uSendTimeout, uReceiveTimeout;
	DebugPrint(_T("CFinsUDPDevice::Connect()\n"));
	if (m_socketPrimary != NULL)
		return COFR_Connected;

	if (!DeviceDetect())
		return COFR_Failed;

	bSuccess = FALSE;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(m_uIPAddress);
	sin.sin_port = htons(m_uPortNumber);


	//void* memcpy (void* dest, const void* source, size_t num)
	memcpy(&m_plc_addr, &sin, sizeof(sin));
		
	m_socketPrimary = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_socketPrimary == INVALID_SOCKET)
	{
		m_socketPrimary = NULL;
		return COFR_Failed;
	}


	return COFR_Ok;
}

UINT32 CFinsUDPDevice::Disconnect()
{
	if (m_socketPrimary != NULL)
	{
		closesocket(m_socketPrimary);
		m_socketPrimary = NULL;
	}

	return COFR_Ok;
}

UINT32 CFinsUDPDevice::GetDeviceID()
{
	return m_uDeviceID;
}

UINT32 CFinsUDPDevice::GetIPAddress()
{
	return m_uIPAddress;
}







UINT32 CFinsUDPDevice::GetMemoryA(LPCSTR lpcCommand, PVOID pValue, INT32 nLength)
{
	return COFR_NotSupported;
}

UINT32 CFinsUDPDevice::GetMemoryW(LPCWSTR lpcwCommand, PVOID pValue, INT32 nLength)
{
	UINT32 uReturn;

	uReturn = GetMemoryA(CW2A(lpcwCommand), pValue, nLength);

	return uReturn;
}

UINT32 CFinsUDPDevice::ReadMemory(PVOID pValue, PINT32 pnLength)
{
	return COFR_NotSupported;
}

//=============================================================================
// protected
//=============================================================================


//바이너리 통신 할때 상대방이 응답을 잘라서 보내는 경우를 대비해서 사용
UINT32 CFinsUDPDevice::ReadVariableBinaryData(PCHAR pszResponse, INT nLength)
{
	UINT32 uResult;
	INT32 nResult, nIndex, nMaxRead;
	BOOL bDone;
	PCHAR tempRecv;
	//
	int		 nFromLen;
	SOCKADDR saFromAddress;
	//
	uResult = COFR_Ok;
	bDone = FALSE;
	nIndex = 0;
	nMaxRead = nLength < MAX_READ ? nLength : MAX_READ;	
	tempRecv = (pszResponse);	
	
	do
	{
		//nFromLen = sizeof(m_plc_addr);
		nFromLen = sizeof(saFromAddress);

		// recvfrom 리턴값
		// > 0 : 실제로 수신한 갯수(바이트 수)
		// -1 : 오류가 발생
		// 0 : TCP) 연결이 정상적으로 닫힌 경우,  UDP) 수신된 패킷에 데이터가 없는경우(비어 있음)

		/*int WSAAPI recvfrom(
			[in]                SOCKET		s,
			[out]               char*		buf,
			[in]                int			len,
			[in]                int			flags,
			[out]               sockaddr*	from,
			[in, out, optional] int*		fromlen
		);*/

		nResult = recvfrom(m_socketPrimary, tempRecv, nMaxRead, 0, (SOCKADDR*)&saFromAddress, &nFromLen);
		
		if (nResult == SOCKET_ERROR)
		{
			uResult = WSAGetLastError();
			break;
		}
		else if (nResult == 0)
		{
			// 수신된 패킷에 데이터가 없는경우(비어 있음)
			if (nIndex >= 14)
			{
				//최소 패킷 크기(FINS헤더(12) + Response Code(2))
				//만큼 받으면 종료
				bDone = TRUE;
			}			
		}

		if (nResult > 0)
		{
			tempRecv += nResult;
			nIndex += nResult;
		}

		if (nIndex >= nLength)
		{
			bDone = TRUE;
		}

	} while (!bDone && nIndex < nLength - nMaxRead);

	if (nResult != SOCKET_ERROR && !bDone)
		uResult = COFR_ResLengthExceeds;

	return uResult;
}


UINT32 CFinsUDPDevice::ReadDummyData()
{
	UINT32 uResult;
	INT32 nResult;
	CHAR tempRecv[MAX_READ];
	BOOL bDone;

	uResult = COFR_Ok;
	bDone = FALSE;

	DebugPrint(_T("CFinsUDPDevice::ReadDummyData() call\n"));

	do
	{
		nResult = recv(m_socketPrimary, tempRecv, MAX_READ, 0);
		if (nResult == SOCKET_ERROR)
		{
			uResult = WSAGetLastError();
			break;
		}

		if (strrchr(tempRecv, CHAR_ACKNOWLEDGE))
		{
			bDone = TRUE;
		}

	} while (!bDone);

	return uResult;
}

//UINT32 CFinsUDPDevice::Lock()
//{
//	if (m_hMutex)
//		WaitForSingleObject(m_hMutex, INFINITE);
//
//	return COFR_Ok;
//}
//
//UINT32 CFinsUDPDevice::Unlock()
//{
//	if (m_hMutex)
//		ReleaseMutex(m_hMutex);
//
//	return COFR_Ok;
//}

//2023.05.09 Add
//Hostlink Header 셋팅
UINT32 CFinsUDPDevice::SetFinsHeader(INT32 nBlockArea,
	INT32 nDestNetworkAddr, INT32 nDestNodeNum, INT32 nDestUnitAddr,
	INT32 nSourceNetworkAddr, INT32 nSourceNodeNum, INT32 nSourceUnitAddr)
{
	UINT32 uResult;

	uResult = COFR_Ok;

	//PLC Network 번호
	m_fins_header.dna = nDestNetworkAddr;//0x01; //(Destination Network Address) - PLC Local network 번호 "01"
	//PLC 노드번호(전송받은 번호)
	//Response 에서 Error이 0x00 이 아닐경우 접속을 끊고 재접속하여 다시 노드번호를 받아야함.
	m_fins_header.da1 = nDestNodeNum; //(Destination Node Address) - PLC Node 번호 "0A"
	//PLC CPU Unit 번호(00으로 고정) 
	m_fins_header.da2 = nDestUnitAddr; //(Destination Node Address) – Host Link 모드에서 목적지가 CPU Unit일 경우 "00"

	//PC Network번호
	m_fins_header.sna = nSourceNetworkAddr;//0x01; //(Source Network Address) - PC 네트워크 번호 "01"
	//PC 노드번호(자동할당 받은 번호)
	m_fins_header.sa1 = nSourceNodeNum; //(Source Unit Address) - PC Node 번호 "EF"->자동할당으로 받은 Node 번호
	//PC CPU 유닛 번호(00 고정)
	m_fins_header.sa2 = nSourceUnitAddr; //(Source Unit Address) - CPU Unit에 연결된 경우 "00"

	m_fins_header.memoryAreaCode = nBlockArea;

	//

	//
	m_read_req_datagram.plc_network_no = m_fins_header.dna;
	m_read_req_datagram.plc_node_no = m_fins_header.da1;
	m_read_req_datagram.plc_cpu_unit = m_fins_header.da2;
	//
	m_read_req_datagram.pc_network_no = m_fins_header.sna;
	m_read_req_datagram.pc_node_no = m_fins_header.sa1;
	m_read_req_datagram.pc_cpu_unit = m_fins_header.sa2;
	//
	m_read_req_datagram.data_area_type = m_fins_header.memoryAreaCode; 
	//

	//
	m_write_req_datagram.plc_network_no = m_fins_header.dna;
	m_write_req_datagram.plc_node_no = m_fins_header.da1;
	m_write_req_datagram.plc_cpu_unit = m_fins_header.da2;
	//
	m_write_req_datagram.pc_network_no = m_fins_header.sna;
	m_write_req_datagram.pc_node_no = m_fins_header.sa1;
	m_write_req_datagram.pc_cpu_unit = m_fins_header.sa2;
	//
	m_write_req_datagram.data_area_type = nBlockArea;


	return uResult;
}


UINT32 CFinsUDPDevice::GetFinsHeader(PINT32 nBlockArea,
	PINT32 nDestNetworkAddr, PINT32 nDestNodeNum, PINT32 nDestUnitAddr,
	PINT32 nSourceNetworkAddr, PINT32 nSourceNodeNum, PINT32 nSourceUnitAddr)
{
	UINT32 uResult;

	uResult = COFR_Ok;
	//
	*(nDestNetworkAddr) = m_fins_header.dna;
	*(nDestNodeNum) = m_fins_header.da1;
	*(nDestUnitAddr) = m_fins_header.da2;
	//
	*(nSourceNetworkAddr) = m_fins_header.sna;
	*(nSourceNodeNum) = m_fins_header.sa1;
	*(nSourceUnitAddr) = m_fins_header.sa2;
	//
	*(nBlockArea) = m_write_req_datagram.data_area_type;


	return uResult;
}
