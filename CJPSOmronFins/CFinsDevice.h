

#ifndef __CFINS_DEVICE_H__
#define __CFINS_DEVICE_H__


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

}_csofins_header_t;

//fins node address request data 전송
const char s_buf_fins_noad_address_req[24]
= { 0x46, 0x49, 0x4E, 0x53, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };


typedef union convert_int_to_byte4 {
	unsigned int	uint32_val;
	unsigned short	uint16_val[2];
	unsigned char	byte_val[4];
}byte4_t;

typedef union convert_short_to_byte2 {
	unsigned short	uint16_val;
	unsigned char	byte_val[2];
}byte2_t;

//x64를 생각해서 8바이트 단위로 끊을것
typedef struct tagFinsReadMemRequest
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


}FINS_READ_REQ_PACKET;



typedef struct tagFinsReadMemResponse
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
	unsigned char	data_area_type;		// 29
	unsigned char	read_err_code;		// 30
	unsigned char	bData[1602];		// 31 ~ 1632

}FINS_READ_RESPONSE_PACKET;

typedef struct tagFinsWriteMemRequest
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


}FINS_WRITE_REQ_PACKET;


typedef struct tagFinsWriteMemResponse
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

}FINS_WRITE_RESPONSE_PACKET;


class CFinsDevice
{
public:
	CFinsDevice();
	CFinsDevice(UINT32 uIPAddress, UINT32 uDeviceID);
	virtual ~CFinsDevice();

	virtual UINT32 GetIPAddress();
	virtual UINT32 GetDeviceID();

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


protected:
	UINT32	m_uDeviceID;
	UINT32	m_uIPAddress;
	HANDLE	m_hMutex;

	SOCKET m_socketPrimary;
	UINT32 m_uByteOrder;

	//
	_csofins_header_t			m_fins_header;
	FINS_READ_REQ_PACKET		m_read_req_packet;
	FINS_READ_RESPONSE_PACKET	m_read_res_packet;
	FINS_WRITE_REQ_PACKET		m_write_req_packet;
	FINS_WRITE_RESPONSE_PACKET	m_write_res_packet;
		
};

typedef CFinsDevice* PCFinsDevice;



#endif
//__CFINS_DEVICE_H__