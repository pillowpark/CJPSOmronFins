

#include "pch.h"
#include "CFinsDevice.h"
#include "../Include/CJPSOmronFinsDef.h"
#include "CommonSupport.h"

#include <Ipexport.h>
#include <Icmpapi.h>


CFinsDevice::CFinsDevice()
{
	m_uIPAddress = 0;
	m_uDeviceID = 0;
	m_socketPrimary = NULL;
	m_hMutex = NULL;

	//
	GetByteOrder();
	HostLinkHeaderSetting();
}

CFinsDevice::CFinsDevice(UINT32 uIPAddress, UINT32 uDeviceID)
{
	m_uIPAddress = uIPAddress;
	m_uDeviceID = uDeviceID;
	m_socketPrimary = NULL;

	//
	GetByteOrder();
	HostLinkHeaderSetting();
	
	TCHAR tszBuffer[MAX_PATH];
	_stprintf_s(tszBuffer, _countof(tszBuffer), _T("Ethernet-%08X"), uIPAddress);
	m_hMutex = CreateMutex(NULL, FALSE, tszBuffer);
}


CFinsDevice::~CFinsDevice()
{
	if (m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}

VOID CFinsDevice::HostLinkHeaderSetting()
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
	m_fins_header.da2=0x00; //(Destination Node Address) – Host Link 모드에서 목적지가 CPU Unit일 경우 "00"

	//PC Network번호
	m_fins_header.sna= 0x00;//0x01; //(Source Network Address) - PC 네트워크 번호 "01"
	//PC 노드번호(자동할당 받은 번호)
	m_fins_header.sa1=0xFF; //(Source Unit Address) - PC Node 번호 "EF"->자동할당으로 받은 Node 번호
	//PC CPU 유닛 번호(00 고정)
	m_fins_header.sa2 = 0x00; //(Source Unit Address) - CPU Unit에 연결된 경우 "00"

	//Sequence Number( 00 ~ FF 까지 계속 증가 )
	m_fins_header.sid = 0x00; //(Source ID) - 재전송시 카운터로사용됨 "00" (00~FF 사이의 수 입력)


	// FINS Command code (READ = 0101) 지령관련 헤더셋팅
	m_read_req_packet.prefix[0] = 0x46;
	m_read_req_packet.prefix[1] = 0x49;
	m_read_req_packet.prefix[2] = 0x4e;
	m_read_req_packet.prefix[3] = 0x53;

	m_read_req_packet.data_size.uint32_val = htonl(0x001a);
	m_read_req_packet.command_code.uint32_val = htonl(0x0002);
	m_read_req_packet.error_code.uint32_val = htons(0x0000);

	m_read_req_packet.icf = m_fins_header.icf;
	m_read_req_packet.rsv = m_fins_header.rsv;
	m_read_req_packet.gct = m_fins_header.gct;
	//
	m_read_req_packet.plc_network_no	= m_fins_header.dna;
	m_read_req_packet.plc_node_no		= m_fins_header.da1;
	m_read_req_packet.plc_cpu_unit		= m_fins_header.da2;
	//
	m_read_req_packet.pc_network_no		= m_fins_header.sna;
	m_read_req_packet.pc_node_no		= m_fins_header.sa1;
	m_read_req_packet.pc_cpu_unit		= m_fins_header.sa2;
	//
	m_read_req_packet.sequence			= 0x00;
	m_read_req_packet.fins_cmd_code[0]	= 0x01;
	m_read_req_packet.fins_cmd_code[1]	= 0x01;
	m_read_req_packet.data_area_type	= 0x82;  //DM영역
	m_read_req_packet.data_start_offset[0] = 0x00;
	m_read_req_packet.data_start_offset[1] = 0x00;
	m_read_req_packet.bit_address		= 0x00;
	m_read_req_packet.read_data_length[0] = 0x00;
	m_read_req_packet.read_data_length[1] = 0x00;


	// FINS Command code (Write = 0102)
	//	m_write_req_packet
	// (0x46494e53) FINS
	m_write_req_packet.prefix[0] = 0x46;
	m_write_req_packet.prefix[1] = 0x49;
	m_write_req_packet.prefix[2] = 0x4e;
	m_write_req_packet.prefix[3] = 0x53;

	m_write_req_packet.frame_data_size.uint32_val	= 0; //0x001a
	m_write_req_packet.command_code.uint32_val	= htonl(0x0002); //0x0002
	m_write_req_packet.error_code.uint32_val	= htonl(0x0000); //0x0000
	//
	m_write_req_packet.icf = m_fins_header.icf; //(Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 “80” 고정
	m_write_req_packet.rsv = m_fins_header.rsv; //(Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 “00” 고정
	m_write_req_packet.gct = m_fins_header.gct;; //(Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 “02” PLC <-> PC
	//
	m_write_req_packet.plc_network_no	= m_fins_header.dna;
	m_write_req_packet.plc_node_no		= m_fins_header.da1;
	m_write_req_packet.plc_cpu_unit		= m_fins_header.da2;
	//
	m_write_req_packet.pc_network_no	= m_fins_header.sna;
	m_write_req_packet.pc_node_no		= m_fins_header.sa1;
	m_write_req_packet.pc_cpu_unit		= m_fins_header.sa2;
	//
	m_write_req_packet.sequence				= 0x00;			// Sequence(00~FF까지 자동 증가)
	//
	m_write_req_packet.fins_cmd_code[0]		= 0x01; // FINS Command code(Write = 0102)
	m_write_req_packet.fins_cmd_code[1]		= 0x02; // FINS Command code(Write = 0102)
	//
	m_write_req_packet.data_area_type		= 0x82;  //	Data Type(ex.DM의 경우 82)
	//
	m_write_req_packet.data_start_offset[0] = 0x00;
	m_write_req_packet.data_start_offset[1] = 0x00;

	m_write_req_packet.bit_address			= 0x00;

	m_write_req_packet.write_data_length[0] = 0x00;
	m_write_req_packet.write_data_length[1] = 0x00;
}

UINT32 CFinsDevice::GetFinsErrorCode(PCHAR pszResponse, INT nLength)
{
	UINT32 uErrorCode = 0;
	UINT32 uOneByte[04] = { 0,0,0,0 };

	//bit12 ~ 15 : Error Code - 0인지확인
	uOneByte[00] = (pszResponse[15] <<  0) & 0x000000FF;
	uOneByte[01] = (pszResponse[14] <<  8) & 0x0000FF00;
	uOneByte[02] = (pszResponse[13] << 16) & 0x00FF0000;
	uOneByte[03] = (pszResponse[12] << 24) & 0xFF000000;

	uErrorCode = (uOneByte[00] | uOneByte[01] | uOneByte[02] | uOneByte[03]);

	return uErrorCode;
}

UINT32 CFinsDevice::GetNodeAddress(PINT32 pnErrorCode)
{
	UINT32 uResult;
	INT32 nResult, nCommandLength, nResponseLength;
	PCHAR pszCommand, pszResponse;
	UINT32 uErrorCode;
	*pnErrorCode = 0;


	DebugPrint(_T("[DebugPrint][FinsDLL] GetNodeAddress() call \n"));

	if (m_socketPrimary == NULL)
		return COFR_NotConnected;


	Lock();

	uResult = COFR_Ok;
	nCommandLength = 20;  //Fins Node Data Request Frame Size

	nResponseLength = 24; //Fins Node Data Response Frame Size
	pszCommand = new CHAR[nCommandLength];
	pszResponse = new CHAR[nResponseLength];

	memset(pszResponse, 0, nResponseLength);
	//void* memcpy (void* dest, const void* source, size_t num)
	memcpy(pszCommand, s_buf_fins_noad_address_req, nCommandLength);

	DebugPrint(_T("[DebugPrint][FinsDLL] send() %d [byte] call \n"), nCommandLength);
	
	nResult = send(m_socketPrimary, pszCommand, nCommandLength, 0);
	
	DebugPrint(_T("[DebugPrint][FinsDLL] send() %d [byte] return \n"), nResult);

	if (nResult == SOCKET_ERROR)
		uResult = WSAGetLastError();

	if (uResult == COFR_Ok)
	{
		DebugPrint(_T("[DebugPrint][FinsDLL] ReadVariableData() %d [byte] call \n"), nResponseLength);
		uResult = ReadVariableBinaryData(pszResponse, nResponseLength);
		DebugPrint(_T("[DebugPrint][FinsDLL] ReadVariableData() return \n"));

		switch (uResult)
		{
		case COFR_ResLengthExceeds:
			ReadDummyData();

		case COFR_Ok:
			//if (strlen(pszResponse) > static_cast<SIZE_T>(nLength - 1))
			//	uResult = COFR_ResLengthExceeds;
			//strncpy_s(lpResponse, nLength, pszResponse, nLength - 1);
			
			//bit12 ~ 15 : Error Code - 0인지확인
			uErrorCode = GetFinsErrorCode(pszResponse, nResponseLength);
			if (uErrorCode != 0x0000)
			{
				//에러면 통신을 끊어야 될듯
				*pnErrorCode = uErrorCode;
				//for debug
				DebugPrint(_T("[DebugPrint][FinsDLL] GetNodeAddress() Error Occur = %d \n"), uErrorCode);
			}
			else
			{
				//p_fins_type->client_node_no = r_buf[19]; PC Network 번호 0x01 고정??
				//p_fins_type->server_node_no = r_buf[23]; PLC Network 번호

				m_fins_header.da1 = (pszResponse[23] & 0x00FF); //Server Address
				m_fins_header.sa1 = (pszResponse[19] & 0x00FF); //Client Address
				//			
				m_read_req_packet.plc_node_no	= m_fins_header.da1;  
				m_read_req_packet.pc_node_no	= m_fins_header.sa1;
				//
				m_write_req_packet.plc_node_no = m_fins_header.da1;
				m_write_req_packet.pc_node_no = m_fins_header.sa1;

				//for debug
				DebugPrint(_T("[DebugPrint][FinsDLL] GetNodeAddress() PLC Node(da1) = 0x%02x / PC Node(sa1) = 0x%02x   call..\n"), 
					(m_read_req_packet.plc_node_no & 0x00FF), 
					(m_read_req_packet.pc_node_no  & 0x00FF));
				//			

			}
			break;
		}
	}

	delete[] pszCommand;
	delete[] pszResponse;

	Unlock();

	return uResult;
}
UINT32 CFinsDevice::FinsMemRead(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode)
{
	UINT32 uResult = 0;
	UINT32 uTempInt32	= 0;
	UINT32 uTempInt32_2 = 0;
	byte2_t uTemp16;
	
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
	uTempInt32 = m_read_req_packet.sequence++;
	m_read_req_packet.sequence = (unsigned char)(uTempInt32 & 0x00FF);
	//
	uTemp16.uint16_val = htons((unsigned short)nSize);;
	m_read_req_packet.read_data_length[1] = uTemp16.byte_val[1];
	m_read_req_packet.read_data_length[0] = uTemp16.byte_val[0];
	//
	uTemp16.uint16_val = htons((unsigned short)uAddress);
	m_read_req_packet.data_start_offset[1] = uTemp16.byte_val[1];
	m_read_req_packet.data_start_offset[0] = uTemp16.byte_val[0];	
	//

	uResult = COFR_Ok;
	nCommandLength	= 34;  //고정값, Fins Read Memory Data Request Frame Size
	nResponseLength = 30 + (nSize*2); //Fins Read Memory Data Response Frame Size
	// 헤더(30 byte) + 데이타(Word * 2) 

	//printf("\n [DEBUG] FinsMemRead() call - Req[%d]/Resp[%d] ", nCommandLength, nResponseLength);

	uTempInt32		= 8 - (nCommandLength % 8);
	uTempInt32_2	= 8 - (nResponseLength % 8);

	pszCommand	= new CHAR[(nCommandLength  + uTempInt32)];
	pszResponse = new CHAR[(nResponseLength + uTempInt32_2)];

	memset(pszCommand,	0, (nCommandLength + uTempInt32));
	memset(pszResponse, 0, (nResponseLength + uTempInt32_2));

	//void* memcpy (void* dest, const void* source, size_t num)
	memcpy(pszCommand, (char*) & m_read_req_packet, nCommandLength);

	DebugPrint(_T("[DebugPrint][FinsDLL] send() %d [byte] call \n"), nCommandLength);
	nResult = send(m_socketPrimary, pszCommand, nCommandLength, 0);
	DebugPrint(_T("[DebugPrint][FinsDLL] send() %d [byte] return \n"), nResult);

	if (nResult == SOCKET_ERROR)
		uResult = WSAGetLastError();

	if (uResult == COFR_Ok)
	{
		DebugPrint(_T("[DebugPrint][FinsDLL] ReadVariableData() %d [byte] call \n"), nResponseLength);
		uResult = ReadVariableBinaryData((char*) &m_read_res_packet, nResponseLength);
		DebugPrint(_T("[DebugPrint][FinsDLL] ReadVariableData() return \n"));

		switch (uResult)
		{
		case COFR_ResLengthExceeds:
			ReadDummyData();

		case COFR_Ok:
			
			//bit12 ~ 15 : Error Code - 0인지확인
			uErrorCode = GetFinsErrorCode((char*)&m_read_res_packet, nResponseLength);
			if (uErrorCode != 0x0000)
			{
				//에러면 통신을 끊어야 될듯
				*uFinsErrorCode = uErrorCode;
				uResult = COFR_PLCErrorOmronFINS;
			}
			else
			{
				//성공이면 30바이트 빼고 상위로 복사
				uDataLength = (nSize * 2);
				for (int i = 0; i < uDataLength; i+=2)
				{
					//pszResponse[(i)] = m_read_res_packet.bData[(i+1)];
					//pszResponse[(i+1)] = m_read_res_packet.bData[i];

					pszResponse[(i)] = m_read_res_packet.bData[(i + 1)];
					pszResponse[(i + 1)] = m_read_res_packet.bData[i];
				}
				
				//memcpy(pValue, (pszResponse + 30), uDataLength);
				memcpy(pValue, pszResponse, uDataLength);

				//p_fins_type->client_node_no = r_buf[19]; PC Network 번호 0x01 고정??
				//p_fins_type->server_node_no = r_buf[23]; PLC Network 번호

				//m_fins_header.da1 = (pszResponse[23] & 0x00FF);
				//m_fins_header.sa1 = (pszResponse[19] & 0x00FF);
			}
			break;
		}
	}

	delete[] pszCommand;
	delete[] pszResponse;

	Unlock();

	return uResult;
}


UINT32 CFinsDevice::FinsMemWrite(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode)
{
	UINT32 uResult = 0;
	//
	UINT32 uTempInt32	= 0;
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
	//m_write_req_packet.sequence++;
	//m_write_req_packet.write_data_length = (unsigned short)nSize;
	//m_write_req_packet.data_start_offset = (unsigned short)uAddress;

	//memcpy(m_write_req_packet.bData, pValue, nSize);
	//
	// 테스트 해보고 바이트 오더 고려할것
	uTempInt32 = m_write_req_packet.sequence;
	uTempInt32 = uTempInt32 + 1;
	m_write_req_packet.sequence = (unsigned char)(uTempInt32 & 0x00FF);
	//
	uTemp16.uint16_val = htons((unsigned short)(nSize));;
	m_write_req_packet.write_data_length[1] = uTemp16.byte_val[1];
	m_write_req_packet.write_data_length[0] = uTemp16.byte_val[0];
	//
	uTemp16.uint16_val = htons((unsigned short)uAddress);
	m_write_req_packet.data_start_offset[1] = uTemp16.byte_val[1];
	m_write_req_packet.data_start_offset[0] = uTemp16.byte_val[0];
	//
	uTempInt32 = (26 + (nSize * 2));
	m_write_req_packet.frame_data_size.uint32_val = htonl(uTempInt32);
	
	

	//void* memcpy (void* dest, const void* source, size_t num)
	uDataLength = (nSize * 2);
	for (int i = 0; i < uDataLength; i += 2)
	{
		//pszResponse[(i)] = m_read_res_packet.bData[(i+1)];
		//pszResponse[(i+1)] = m_read_res_packet.bData[i];

		m_write_req_packet.bData[(i + 1)]	= ((char*)pValue)[(i)];
		m_write_req_packet.bData[i]			= ((char*)pValue)[(i + 1)];
	}
	//memcpy((char*)m_write_req_packet.bData, (char*)pValue, (size_t)(nSize*2));
	//

	uResult = COFR_Ok;
	nCommandLength = 34 + (nSize * 2);  //Fins Write Memory Data Request Frame Size
	nResponseLength = 30; //Fins Write Memory Data Response Frame Size, 고정값

	printf("\n [DEBUG] FinsMemWrite() call - Req[%d]/Resp[%d] ", nCommandLength, nResponseLength);

	uTempInt32 = 8 - (nCommandLength % 8);
	uTempInt32_2 = 8 - (nResponseLength % 8);

	pszCommand = new CHAR[(nCommandLength + uTempInt32)];
	pszResponse = new CHAR[(nResponseLength + uTempInt32_2)];

	memset(pszCommand, 0, (nCommandLength + uTempInt32));
	memset(pszResponse, 0, (nResponseLength + uTempInt32_2));
	
	//void* memcpy (void* dest, const void* source, size_t num)
	memcpy(pszCommand, (char*) &m_write_req_packet, nCommandLength);

	nResult = send(m_socketPrimary, pszCommand, nCommandLength, 0);
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

			//bit12 ~ 15 : Error Code - 0인지확인
			uErrorCode = GetFinsErrorCode(pszResponse, nResponseLength);
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

UINT32 CFinsDevice::GetByteOrder()
{
	byte4_t _b4v;

	_b4v.uint32_val = 0x12345678;

	if (_b4v.byte_val[0] == 0x12 && _b4v.byte_val[1] == 0x34
		&& _b4v.byte_val[2] == 0x56 && _b4v.byte_val[3] == 0x78)
	{
		m_uByteOrder = BYTE_ORDER_BIG_ENDIAN;
	}
	else if (_b4v.byte_val[3] == 0x12 && _b4v.byte_val[2] == 0x34
		&& _b4v.byte_val[1] == 0x56 && _b4v.byte_val[0] == 0x78)
	{
		m_uByteOrder = BYTE_ORDER_LITTLE_ENDIAN;
	}
	else
	{
		m_uByteOrder = 0;
	}

	return m_uByteOrder;
}

//-------------------------
UINT32 CFinsDevice::IsFinsConnected()
{
	if (m_socketPrimary != NULL)
		return COFR_Connected;
	else
		return COFR_NotConnected;
}

UINT32 CFinsDevice::Connect()
{
	SOCKADDR_IN sin;
	INT32 nResult;
	BOOL bSuccess;
	UINT32 uSendTimeout, uReceiveTimeout;
	DebugPrint(_T("CFinsDevice::Connect()\n"));
	if (m_socketPrimary != NULL)
		return COFR_Connected;

	if (!DeviceDetect())
		return COFR_Failed;

	bSuccess = FALSE;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(m_uIPAddress);
	sin.sin_port = htons(9600);

	m_socketPrimary = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socketPrimary == INVALID_SOCKET)
	{
		m_socketPrimary = NULL;
		return COFR_Failed;
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

UINT32 CFinsDevice::Disconnect()
{
	if (m_socketPrimary != NULL)
	{
		closesocket(m_socketPrimary);
		m_socketPrimary = NULL;
	}

	return COFR_Ok;
}

UINT32 CFinsDevice::GetDeviceID()
{
	return m_uDeviceID;
}

UINT32 CFinsDevice::GetIPAddress()
{
	return m_uIPAddress;
}


UINT32 CFinsDevice::GetResponseA(LPCSTR lpcCommand, LPSTR lpResponse, INT32 nLength)
{
	UINT32 uResult;
	INT32 nResult, nCommandLength, nResponseLength;
	PCHAR pszCommand, pszResponse;

	lpResponse[0] = '\0';

	if (m_socketPrimary == NULL)
		return COFR_NotConnected;

	if (strlen(lpcCommand) >= MAX_COMMAND_LENGTH)
		return COFR_CmdLengthExceeds;

	uResult = COFR_Ok;
	nCommandLength = strlen(lpcCommand) + 1;
	//nResponseLength = nCommandLength + nLength + MAX_READ;
	//nResponseLength = nLength + MAX_READ;
	nResponseLength = nLength;
	pszCommand = new CHAR[nCommandLength];
	pszResponse = new CHAR[nResponseLength];

	memset(pszResponse, 0, nResponseLength);
	sprintf_s(pszCommand, nCommandLength, "%s\n", lpcCommand);

	nResult = send(m_socketPrimary, pszCommand, nCommandLength, 0);
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

			if (strlen(pszResponse) > static_cast<SIZE_T>(nLength - 1))
				uResult = COFR_ResLengthExceeds;
			strncpy_s(lpResponse, nLength, pszResponse, nLength - 1);
			break;
		}
	}

	delete[] pszCommand;
	delete[] pszResponse;

	return uResult;
}



UINT32 CFinsDevice::GetResponseW(LPCWSTR lpcwCommand, LPWSTR lpwResponse, INT32 nLength)
{
	UINT32 uReturn;
	PCHAR pResponse;

	pResponse = new CHAR[nLength];
	uReturn = GetResponseA(CW2A(lpcwCommand), pResponse, nLength);
	wcsncpy_s(lpwResponse, nLength, CA2W(pResponse), nLength);
	delete[] pResponse;

	return uReturn;
}

UINT32 CFinsDevice::GetMemoryA(LPCSTR lpcCommand, PVOID pValue, INT32 nLength)
{
	return COFR_NotSupported;
}

UINT32 CFinsDevice::GetMemoryW(LPCWSTR lpcwCommand, PVOID pValue, INT32 nLength)
{
	UINT32 uReturn;

	uReturn = GetMemoryA(CW2A(lpcwCommand), pValue, nLength);

	return uReturn;
}

UINT32 CFinsDevice::ReadMemory(PVOID pValue, PINT32 pnLength)
{
	return COFR_NotSupported;
}

//=============================================================================
// protected
//=============================================================================
BOOL CFinsDevice::DeviceDetect(UINT32 uIPAddress)
{
#if 1
	HANDLE hIcmpFile;

	if (!uIPAddress)
		uIPAddress = m_uIPAddress;

	hIcmpFile = IcmpCreateFile();
	if (hIcmpFile != INVALID_HANDLE_VALUE)
	{
		INT i;
		DWORD dwRetVal;
		CHAR szSendData[] = "I love kgh!!";
		CHAR szReplyBuffer[100];

		for (i = 0; i < 10; i++)
		{
			dwRetVal = IcmpSendEcho(hIcmpFile, htonl(uIPAddress), szSendData, sizeof(szSendData), NULL, szReplyBuffer, sizeof(szReplyBuffer), 1000);
			if (dwRetVal)
				break;
			Sleep(100);
		}

		IcmpCloseHandle(hIcmpFile);

		if (dwRetVal)
			return TRUE;
	}
	else
	{
		DebugPrint(_T("nRet=%d\n"), GetLastError());
	}

	return FALSE;

#else
	INT i, nRc;
	ULONG lMode;
	SOCKET s;
	sockaddr_in saddr;
	fd_set fdWrite, fdError;
	TIMEVAL Timeout;

	if (!uIPAddress)
		uIPAddress = m_uIPAddress;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(TCP_PORT_NO);
	saddr.sin_addr.s_addr = htonl(uIPAddress);

	Timeout.tv_sec = 0;
	Timeout.tv_usec = 1000000;

	for (i = 0; i < 5; i++)
	{
		s = socket(AF_INET, SOCK_STREAM, 0);
		if (s == INVALID_SOCKET)
			continue;
		lMode = 1;
		nRc = ioctlsocket(s, FIONBIO, &lMode); // put socket in non-blocking state
		nRc = connect(s, (struct sockaddr*)&saddr, sizeof(saddr));
		lMode = 0;
		nRc = ioctlsocket(s, FIONBIO, &lMode); // put socket in non-blocking state

		FD_ZERO(&fdWrite);
		FD_ZERO(&fdError);;
		FD_SET(s, &fdWrite);
		FD_SET(s, &fdError);
		nRc = select(0, NULL, &fdWrite, &fdError, &Timeout);
		if (FD_ISSET(s, &fdWrite))
			nRc = NO_ERROR;
		else
			nRc = ERROR_ACCESS_DENIED;

		closesocket(s);

		if (nRc == NO_ERROR)
			return TRUE;
	}
#endif

	return FALSE;
}


//바이너리 통신 할때 상대방이 응답을 잘라서 보내는 경우를 대비해서 사용
UINT32 CFinsDevice::ReadVariableBinaryData(PCHAR pszResponse, INT nLength)
{
	UINT32 uResult;
	INT32 nResult, nIndex, nMaxRead;
	BOOL bDone;
	PCHAR tempRecv;

	uResult = COFR_Ok;
	bDone = FALSE;
	nIndex = 0;
	nMaxRead = nLength < MAX_READ ? nLength : MAX_READ;
	tempRecv = pszResponse;
	do
	{
		nResult = recv(m_socketPrimary, tempRecv, nMaxRead, 0);
		if (nResult == SOCKET_ERROR)
		{
			uResult = WSAGetLastError();
			break;
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

//ASCII 통신할때 사용하는 함수, 바이너리 통신할때는 약간 안맞을듯
UINT32 CFinsDevice::ReadVariableAsciiData(PCHAR pszResponse, INT nLength)
{
	UINT32 uResult;
	INT32 nResult, nIndex, nMaxRead;
	BOOL bDone;
	PCHAR tempRecv;

	uResult = COFR_Ok;
	bDone = FALSE;
	nIndex = 0;
	nMaxRead = nLength < MAX_READ ? nLength : MAX_READ;
	tempRecv = pszResponse;
	do
	{
		nResult = recv(m_socketPrimary, tempRecv, nMaxRead, 0);
		if (nResult == SOCKET_ERROR)
		{
			uResult = WSAGetLastError();
			break;
		}

		if (nResult > 0)
		{
			tempRecv += nResult;
			nIndex += nResult;
		}

		if (strrchr(pszResponse, CHAR_ACKNOWLEDGE))
		{
			bDone = TRUE;
		}

	} while (!bDone && nIndex < nLength - nMaxRead);

	if (nResult != SOCKET_ERROR && !bDone)
		uResult = COFR_ResLengthExceeds;

	return uResult;
}

UINT32 CFinsDevice::ReadDummyData()
{
	UINT32 uResult;
	INT32 nResult;
	CHAR tempRecv[MAX_READ];
	BOOL bDone;

	uResult = COFR_Ok;
	bDone = FALSE;

	DebugPrint(_T("CFinsDevice::ReadDummyData() call\n"));

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

UINT32 CFinsDevice::Lock()
{
	if (m_hMutex)
		WaitForSingleObject(m_hMutex, INFINITE);

	return COFR_Ok;
}

UINT32 CFinsDevice::Unlock()
{
	if (m_hMutex)
		ReleaseMutex(m_hMutex);

	return COFR_Ok;
}