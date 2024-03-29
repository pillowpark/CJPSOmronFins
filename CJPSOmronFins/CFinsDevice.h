﻿

#ifndef __CFINS_DEVICE_H__
#define __CFINS_DEVICE_H__

#include <string.h>
#include <vector>
#include <string>


#pragma once

#define WM_IPC				WM_USER + 1010  //Windows Message 를 이용하기 위한 이벤트 번호

#define TCP_PORT_NO				9600
#define MAX_READ				16384//65536	//8192
#define MAX_BUFFER				4096
#define MAX_TIMEOUT				5000

#define MAX_COMMAND_LENGTH		4095

#define CHAR_ACKNOWLEDGE		0x06
#define CHAR_LINE_FEED			0x0A
#define CHAR_CARRAGE_RETURN		0x0D
#define CHAR_ESCAPE				0x1B

//
#define BYTE_ORDER_BIG_ENDIAN		0x1234
#define BYTE_ORDER_LITTLE_ENDIAN	0x3412



typedef struct _CJPSOF_FINS_HOST_LINK_HEADER
{
	//0x80 00 02 고정
	int icf; //(Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 “80” 고정
	int rsv; //(Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 “00” 고정
	int gct; //(Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 “02” PLC <-> PC

	//PLC Network 번호
	int dna; //(Destination Network Address) - PLC Local network 번호 “01”
	//PLC 노드번호(전송받은 번호)
	//Response 에서 Error이 0x00 이 아닐경우 접속을 끊고 재접속하여 다시 노드번호를 받아야함.
	int da1; //(Destination Node Address) - PLC Node 번호 “0A“
	//PLC CPU Unit 번호(00으로 고정) 
	int da2; //(Destination Node Address) – Host Link 모드에서 목적지가 CPU Unit일 경우 “00”

	//PC Network번호
	int sna; //(Source Network Address) - PC 네트워크 번호 “01”
	//PC 노드번호(자동할당 받은 번호)
	int sa1; //(Source Unit Address) - PC Node 번호 “EF”->자동할당으로 받은 Node 번호
	//PC CPU 유닛 번호(00 고정)
	int sa2; //(Source Unit Address) - CPU Unit에 연결된 경우 “00”
	//Sequence Number( 00 ~ FF 까지 계속 증가 )
	int sid; //(Source ID) - 재전송시 카운터로사용됨 “00”(00~FF 사이의 수 입력)

	//
	int memoryAreaCode; //Command Data 영역에 들어가는 Block 코드

}_csofins_header_t;

//fins node address request data 전송
const char s_buf_fins_noad_address_req[24]
= { 0x46, 0x49, 0x4E, 0x53, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

//---------------------------------------------

#define OMRON_FINS_ERROR_COUNT		100
#define OMRON_FINS_ERROR_MAX_STRING 128

const unsigned int u_fins_error_list[OMRON_FINS_ERROR_COUNT]
= {
//PLC Errors for Omron FINS Series CS and CJ
//Error Message   Description

0x0000 ,//Normal Completion.
0x0001 ,//Service Canceled.
0x0101 ,//Local Error : Local node not in network.
0x0102 ,//Local Error : Token Timeout.
0x0103 ,//Local Error : Retries Failed.
0x0104 ,//Local Error : Too many send frames.
0x0105 ,//Local Error : Node address range error.
0x0106 ,//Local Error : Node Address Duplication.
	   //
0x0201 ,//Destination Node Error : Destination Node not in network.
0x0202 ,//Destination Node Error : Unit Missing.
0x0203 ,//Destination Node Error : Third Node missing.
0x0204 ,//Destination Node Error : Destination Node busy.
0x0205 ,//Destination Node Error : Response Timeout.
	   //
0x0301 ,//Controller Error : Communications Controller Error.
0x0302 ,//Controller Error : CPU Unit Error.
0x0303 ,//Controller Error : Controller Error.
0x0304 ,//Controller Error : Unit number Error.
	   //
0x0401 ,//Service Unsupported : Undefined Command.
0x0402 ,//Service Unsupported : Not supported by Model / Version.
0x0501 ,//Routing Table Error : Destination address setting error.
0x0502 ,//Routing Table Error : No routing tables.
0x0503 ,//Routing Table Error : Routing table error.
0x0504 ,//Routing Table Error : Too many delays.
0x1001 ,//Command Format Error : Command too long.
0x1002 ,//Command Format Error : Command too short.
0x1003 ,//Command Format Error : Elements / Data don't match.
0x1004 ,//Command Format Error : Command format error.
0x1005 ,//Command Format Error : Header Error.
0x1101 ,//Parameter Error : Area classification missing.
0x1102 ,//Parameter Error : Access Size Error.
0x1103 ,//Parameter Error : Address range error.
0x1104 ,//Parameter Error : Address range exceeded.
0x1106 ,//Parameter Error : Program Missing.
0x1109 ,//Parameter Error : Relational Error.
0x110A ,//Parameter Error : Duplicate Data Access.
0x110B ,//Parameter Error : Response too long.
0x110C ,//Parameter Error : Parameter Error.
0x2002 ,//Read Not Possible : Protected.
0x2003 ,//Read Not Possible : Table missing.
0x2004 ,//Read Not Possible : Data missing.
0x2005 ,//Read Not Possible : Program missing.
0x2006 ,//Read Not Possible : File missing.
0x2007 ,//Read Not Possible : Data mismatch.
0x2101 ,//Write Not Possible : Read Only.
0x2102 ,//Write Not Possible : Protected - cannot write data link table.
0x2103 ,//Write Not Possible : Cannot register.
0x2105 ,//Write Not Possible : Program missing.
0x2106 ,//Write Not Possible : File missing.
0x2107 ,//Write Not Possible : File name already exists.
0x2108 ,//Write Not Possible : Cannot change.
0x2201 ,//Not executable in current mode : Not possible during execution.
0x2202 ,//Not executable in current mode : Not possible while running.
0x2203 ,//Not executable in current mode : Wrong PLC mode(Program).
0x2204 ,//Not executable in current mode : Wrong PLC mode(Debug).
0x2205 ,//Not executable in current mode : Wrong PLC mode(Monitor).
0x2206 ,//Not executable in current mode : Wrong PLC mode(Run).
0x2207 ,//Not executable in current mode : Specified node not polling node.
0x2208 ,//Not executable in current mode : Step cannot be executed.
0x2301 ,//No such device : File device missing.
0x2302 ,//No such device : Missing memory.
0x2303 ,//No such device : Clock missing.
0x2401 ,//Cannot Start / Stop : Table missing.
0x2502 ,//Unit Error : Memory Error.
0x2503 ,//Unit Error : I / O setting Error.
0x2504 ,//Unit Error : Too many I / O points.
0x2505 ,//Unit Error : CPU bus error.
0x2506 ,//Unit Error : I / O Duplication.
0x2507 ,//Unit Error : I / O bus error.
0x2509 ,//Unit Error : SYSMAC BUS / 2 error.
0x250A ,//Unit Error : CPU Bus Unit Error.
0x250D ,//Unit Error : SYSMAC BUS No.duplication.
0x250F ,//Unit Error : Memory Error.
0x2510 ,//Unit Error : SYSMAC BUS terminator missing.
0x2601 ,//Command Error : No protection.
0x2602 ,//Command Error : Incorrect password.
0x2604 ,//Command Error : Protected.
0x2605 ,//Command Error : Service already executing.
0x2606 ,//Command Error : Service stopped.
0x2607 ,//Command Error : No execution right.
0x2608 ,//Command Error : Settings not complete.
0x2609 ,//Command Error : Necessary items not set.
0x260A ,//Command Error : Number already defined.
0x260B ,//Command Error : Error will not clear.
0x3001 ,//Access Right Error : No access right.
0x4001 ,//Abort : Service aborted. //85번째
0x0000,};


//vector<string> str_arr1 =
const char ch_fins_error_list[OMRON_FINS_ERROR_COUNT][OMRON_FINS_ERROR_MAX_STRING] =
//const string ch_fins_error_list[86] =
{
	//PLC Errors for Omron FINS Series CS and CJ
	//Error Message   Description

"0x0000, Normal Completion.",
"0x0001, Service Canceled.",
"0x0101, Local Error : Local node not in network.",
"0x0102, Local Error : Token Timeout.",
"0x0103, Local Error : Retries Failed.",
"0x0104, Local Error : Too many send frames.",
"0x0105, Local Error : Node address range error.",
"0x0106, Local Error : Node Address Duplication.",
//		 
"0x0201, Destination Node Error : Destination Node not in network.",
"0x0202, Destination Node Error : Unit Missing.",
"0x0203, Destination Node Error : Third Node missing.",
"0x0204, Destination Node Error : Destination Node busy.",
"0x0205, Destination Node Error : Response Timeout.",
//		 
"0x0301, Controller Error : Communications Controller Error.",
"0x0302, Controller Error : CPU Unit Error.",
"0x0303, Controller Error : Controller Error.",
"0x0304, Controller Error : Unit number Error.",
//		 
"0x0401, Service Unsupported : Undefined Command.",
"0x0402, Service Unsupported : Not supported by Model / Version.",
"0x0501, Routing Table Error : Destination address setting error.",
"0x0502, Routing Table Error : No routing tables.",
"0x0503, Routing Table Error : Routing table error.",
"0x0504, Routing Table Error : Too many delays.",
"0x1001, Command Format Error : Command too long.",
"0x1002, Command Format Error : Command too short.",
"0x1003, Command Format Error : Elements / Data don't match.",
"0x1004, Command Format Error : Command format error.",
"0x1005, Command Format Error : Header Error.",
"0x1101, Parameter Error : Area classification missing.",
"0x1102, Parameter Error : Access Size Error.",
"0x1103, Parameter Error : Address range error.",
"0x1104, Parameter Error : Address range exceeded.",
"0x1106, Parameter Error : Program Missing.",
"0x1109, Parameter Error : Relational Error.",
"0x110A, Parameter Error : Duplicate Data Access.",
"0x110B, Parameter Error : Response too long.",
"0x110C, Parameter Error : Parameter Error.",
"0x2002, Read Not Possible : Protected.",
"0x2003, Read Not Possible : Table missing.",
"0x2004, Read Not Possible : Data missing.",
"0x2005, Read Not Possible : Program missing.",
"0x2006, Read Not Possible : File missing.",
"0x2007, Read Not Possible : Data mismatch.",
"0x2101, Write Not Possible : Read Only.",
"0x2102, Write Not Possible : Protected - cannot write data link table.",
"0x2103, Write Not Possible : Cannot register.",
"0x2105, Write Not Possible : Program missing.",
"0x2106, Write Not Possible : File missing.",
"0x2107, Write Not Possible : File name already exists.",
"0x2108, Write Not Possible : Cannot change.",
"0x2201, Not executable in current mode : Not possible during execution.",
"0x2202, Not executable in current mode : Not possible while running.",
"0x2203, Not executable in current mode : Wrong PLC mode(Program).",
"0x2204, Not executable in current mode : Wrong PLC mode(Debug).",
"0x2205, Not executable in current mode : Wrong PLC mode(Monitor).",
"0x2206, Not executable in current mode : Wrong PLC mode(Run).",
"0x2207, Not executable in current mode : Specified node not polling node.",
"0x2208, Not executable in current mode : Step cannot be executed.",
"0x2301, No such device : File device missing.",
"0x2302, No such device : Missing memory.",
"0x2303, No such device : Clock missing.",
"0x2401, Cannot Start / Stop : Table missing.",
"0x2502, Unit Error : Memory Error.",
"0x2503, Unit Error : I / O setting Error.",
"0x2504, Unit Error : Too many I / O points.",
"0x2505, Unit Error : CPU bus error.",
"0x2506, Unit Error : I / O Duplication.",
"0x2507, Unit Error : I / O bus error.",
"0x2509, Unit Error : SYSMAC BUS / 2 error.",
"0x250A, Unit Error : CPU Bus Unit Error.",
"0x250D, Unit Error : SYSMAC BUS No.duplication.",
"0x250F, Unit Error : Memory Error.",
"0x2510, Unit Error : SYSMAC BUS terminator missing.",
"0x2601, Command Error : No protection.",
"0x2602, Command Error : Incorrect password.",
"0x2604, Command Error : Protected.",
"0x2605, Command Error : Service already executing.",
"0x2606, Command Error : Service stopped.",
"0x2607, Command Error : No execution right.",
"0x2608, Command Error : Settings not complete.",
"0x2609, Command Error : Necessary items not set.",
"0x260A, Command Error : Number already defined.",
"0x260B, Command Error : Error will not clear.",
"0x3001, Access Right Error : No access right.",
"0x4001, Abort : Service aborted.",
"0x0000_", };



//-----------------------------------------------

typedef union convert_int_to_byte4 {
	unsigned int	uint32_val;
	unsigned short	uint16_val[2];
	unsigned char	byte_val[4];
}byte4_t;

typedef union convert_short_to_byte2 {
	unsigned short	uint16_val;
	unsigned char	byte_val[2];
}byte2_t;


//---------------------------------------------
//---------  for FINS TCP 통신
//x64를 생각해서 8바이트 단위로 끊을것
typedef struct tagFinsTCPReadMemRequest
{
	unsigned char	prefix[4];		//FINS    4
	byte4_t			data_size;		//0x001a  8
	byte4_t			command_code;	//0x0002  12
	byte4_t			error_code;		//0x0000  16
	//
	unsigned char	icf;	// 17 (Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 “80” 고정
	unsigned char	rsv;	// 18 (Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 “00” 고정
	unsigned char	gct;	// 19 (Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 “02” PLC <-> PC
	//
	unsigned char plc_network_no;	// 20
	unsigned char plc_node_no;		// 21
	unsigned char plc_cpu_unit;		// 22
	//
	unsigned char pc_network_no;	// 23
	unsigned char pc_node_no;		// 24
	unsigned char pc_cpu_unit;		// 25
	//
	unsigned char sequence;				// 26
	unsigned char fins_cmd_code[2];		// 27 28
	unsigned char data_area_type;		// 29
	unsigned char data_start_offset[2]; // 30 31
	unsigned char bit_address;			// 32
	unsigned char read_data_length[2];	// 33, 34
	//
	unsigned char _dummy[6];			// 35,36,37,38,39,40
	//x64를 생각해서 8바이트 단위로 끊을것
	//


}FINSTCP_READ_REQ_PACKET;



typedef struct tagFinsTCPReadMemResponse
{
	unsigned char	prefix[4];		//FINS    4
	byte4_t			data_size;		//0x001a  8
	byte4_t			command_code;	//0x0002  12
	byte4_t			error_code;		//0x0000  16
	//
	unsigned char	icf;	// 17 (Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 “80” 고정
	unsigned char	rsv;	// 18 (Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 “00” 고정
	unsigned char	gct;	// 19 (Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 “02” PLC <-> PC
	//
	unsigned char plc_network_no;	// 20
	unsigned char plc_node_no;		// 21
	unsigned char plc_cpu_unit;		// 22
	//
	unsigned char pc_network_no;	// 23
	unsigned char pc_node_no;		// 24
	unsigned char pc_cpu_unit;		// 25
	//
	unsigned char	sequence;			// 26
	unsigned char	fins_cmd_code[2];	// 27 28
	//unsigned char	data_area_type;		// 
	unsigned char	read_err_code[2];		//29 30
	unsigned char	bData[1602];		// 31 ~ 1632

}FINSTCP_READ_RESPONSE_PACKET;

typedef struct tagFinsTCPWriteMemRequest
{
	unsigned char	prefix[4];		//FINS    4
	byte4_t			frame_data_size;		//0x001a  8
	byte4_t			command_code;	//0x0002  12
	byte4_t			error_code;		//0x0000  16
	//
	unsigned char	icf;	// 17 (Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 “80” 고정
	unsigned char	rsv;	// 18 (Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 “00” 고정
	unsigned char	gct;	// 19 (Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 “02” PLC <-> PC
	//
	unsigned char plc_network_no;	// 20
	unsigned char plc_node_no;		// 21
	unsigned char plc_cpu_unit;		// 22
	//
	unsigned char pc_network_no;	// 23
	unsigned char pc_node_no;		// 24
	unsigned char pc_cpu_unit;		// 25
	//
	unsigned char sequence;			//26
	unsigned char fins_cmd_code[2]; //27 28

	unsigned char data_area_type;    //29
	unsigned char data_start_offset[2]; //30,31
	unsigned char bit_address;			//32
	unsigned char write_data_length[2]; //33,34
	unsigned char bData[1606];          //35 ~ 1640 


}FINSTCP_WRITE_REQ_PACKET;


typedef struct tagFinsTCPWriteMemResponse
{
	unsigned char	prefix[4];		//FINS    4
	byte4_t			data_size;		//0x001a  8
	byte4_t			command_code;	//0x0002  12
	byte4_t			error_code;		//0x0000  16
	//
	unsigned char	icf;	// 17 (Information Control Field) - FINS Command를 네트워크 상에 있는 CPU UNIT에 보낼때 “80” 고정
	unsigned char	rsv;	// 18 (Reserved) - 네트워크 상에 있는 CPU Unit에 보내는 것으로 “00” 고정
	unsigned char	gct;	// 19 (Gateway Count) - 송수신이 이뤄지는 대상 네트워크 수 “02” PLC <-> PC
	//
	unsigned char plc_network_no;	// 20
	unsigned char plc_node_no;		// 21
	unsigned char plc_cpu_unit;		// 22
	//
	unsigned char pc_network_no;	// 23
	unsigned char pc_node_no;		// 24
	unsigned char pc_cpu_unit;		// 25
	//
	unsigned char sequence;			//26
	unsigned char fins_cmd_code[2]; //27 28

	unsigned char read_err_code[2]; //29 30
	//
	unsigned char _dummy[10];  //31,32,~~ 38,39,40
	//x64를 생각해서 8바이트 단위로 끊을것

}FINSTCP_WRITE_RESPONSE_PACKET;



class CFinsDevice
{
public:
	CFinsDevice();
	CFinsDevice(UINT32 uIPAddress, UINT16 uPortNumber, UINT32 uDeviceID);
	virtual ~CFinsDevice();

	virtual UINT32 GetIPAddress();
	virtual UINT32 GetDeviceID();

	//virtual UINT32 GetDeviceProtocol();
	//
	virtual UINT32 Connect();
	virtual UINT32 Disconnect();
	virtual UINT32 IsFinsConnected();

	virtual UINT32 GetResponseA(LPCSTR lpcCommand, LPSTR lpResponse, INT32 nLength);
	virtual UINT32 GetResponseW(LPCWSTR lpcwCommand, LPWSTR lpwResponse, INT32 nLength);

	virtual UINT32 GetMemoryA(LPCSTR lpcCommand, PVOID pValue, INT32 nLength);
	virtual UINT32 GetMemoryW(LPCWSTR lpcwCommand, PVOID pValue, INT32 nLength);

	virtual UINT32 ReadMemory(PVOID pValue, PINT32 pnLength);
	
	
	//2023.03.15 Add
	virtual UINT32 GetNodeAddress(PINT32 pnErrorCode);	
	virtual UINT32 FinsMemRead(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode);
	virtual UINT32 FinsMemWrite(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode);
	//
			//2023.05.09 Add
	virtual UINT32 SetFinsHeader(INT32 nBlockArea,
		INT32 nDestNetworkAddr, INT32 nDestNodeNum, INT32 nDestUnitAddr,
		INT32 nSourceNetworkAddr, INT32 nSourceNodeNum, INT32 nSourceUnitAddr);
	//Hostlink Header 셋팅

	virtual UINT32 GetFinsHeader(PINT32 nBlockArea,
		PINT32 nDestNetworkAddr, PINT32 nDestNodeNum, PINT32 nDestUnitAddr,
		PINT32 nSourceNetworkAddr, PINT32 nSourceNodeNum, PINT32 nSourceUnitAddr);

	virtual UINT32 GetLastErrorA(LPSTR lpErrorStr, INT32 nLength);

protected:
	BOOL DeviceDetect(UINT32 uIPAddress = 0);
	UINT32 ReadVariableBinaryData(PCHAR pszResponse, INT nLength);
	UINT32 ReadVariableAsciiData(PCHAR pszResponse, INT nLength);
	UINT32 ReadDummyData();

	UINT32 Lock();
	UINT32 Unlock();

	//2023.03.15 Add
	VOID  HostLinkHeaderSetting();
	UINT32 GetFinsErrorCode(PCHAR pszResponse, INT nLength);
	UINT32 GetByteOrder();
	//
	_csofins_header_t			m_fins_header;

	UINT32	m_uDeviceID;
	UINT32	m_uIPAddress;
	UINT16	m_uPortNumber;
	//
	HANDLE	m_hMutex;

	SOCKET m_socketPrimary;
	UINT32 m_uByteOrder;

private:
	
	//
	//UINT32 m_uDeviceType;
	//
	
	FINSTCP_READ_REQ_PACKET		m_read_req_packet;
	FINSTCP_READ_RESPONSE_PACKET	m_read_res_packet;
	FINSTCP_WRITE_REQ_PACKET		m_write_req_packet;
	FINSTCP_WRITE_RESPONSE_PACKET	m_write_res_packet;
		
	//
	//for UDP
	SOCKADDR m_plc_addr;
};

typedef CFinsDevice* PCFinsDevice;



#endif
//__CFINS_DEVICE_H__