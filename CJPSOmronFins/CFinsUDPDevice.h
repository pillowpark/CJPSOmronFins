
#ifndef __CFINSUDP_DEVICE_H__
#define __CFINSUDP_DEVICE_H__



#pragma once


#include "CFinsDevice.h"



//---------------------------------------------
//---------  for FINS UDP ���
//x64�� �����ؼ� 8����Ʈ ������ ������
typedef struct tagFinsUDPReadMemRequest
{

	// FINS Header
	//OMRON ICF Field: 0x80, Gateway bit: Use Gateway, Data Type bit: Command, Response setting bit: Response Required
	unsigned char	icf;	// 0 (Information Control Field) - FINS Command�� ��Ʈ��ũ �� �ִ� CPU UNIT�� ������ ��80�� ����
	unsigned char	rsv;	// 1 (Reserved) - ��Ʈ��ũ �� �ִ� CPU Unit�� ������ ������ ��00�� ����
	unsigned char	gct;	// 2 (Gateway Count) - �ۼ����� �̷����� ��� ��Ʈ��ũ �� ��02�� PLC <-> PC
	
	//������(destination)
	unsigned char plc_network_no;	// 3  Destination network address: Local network (0x00)
	unsigned char plc_node_no;		// 4  Destination node number: Unknown (0xdd)
	unsigned char plc_cpu_unit;		// 5  Destination unit address: PC (CPU) (0x00)
	
	//�����(source)
	unsigned char pc_network_no;	// 6  Source network address: Local network (0x00)
	unsigned char pc_node_no;		// 7  Source node number: Unknown (0xa9)
	unsigned char pc_cpu_unit;		// 8  Source unit address: PC (CPU) (0x00)
	//
	unsigned char sequence;				// 9     Service ID: 0x00
	unsigned char fins_cmd_code[2];		// 10 11  Command CODE: Memory Area Read (0x0101)

	// Command Data
	unsigned char data_area_type;		// 12      Memory Area Code: DM : Word contents (0x82)
	unsigned char data_start_offset[2]; // 13 14   Beginning address: 0x0000
	unsigned char bit_address;			// 15       Beginning address bits: 0x00
	unsigned char read_data_length[2];	// 16, 17   Number of items: 
	//
	unsigned char _dummy[6];			// 18,19,20,21,22,23
	//x64�� �����ؼ� 8����Ʈ ������ ������
	//


}FINSUDP_READ_REQ_DATAGRAM;



typedef struct tagFinsUDPReadMemResponse
{

	// FINS Header
	//OMRON ICF Field: 0xc0, Gateway bit: Use Gateway, Data Type bit: Response, Response setting bit: Response Required
	unsigned char	icf;	// 0 (OMRON Information Control Field) -
	unsigned char	rsv;	// 1 (Reserved) - ��Ʈ��ũ �� �ִ� CPU Unit�� ������ ������ ��00�� ����
	unsigned char	gct;	// 2 (Gateway Count) - �ۼ����� �̷����� ��� ��Ʈ��ũ �� ��02�� PLC <-> PC
	
	//������(destination)
	unsigned char pc_network_no;	// 6
	unsigned char pc_node_no;		// 7
	unsigned char pc_cpu_unit;		// 8
	
	//�����(source)
	unsigned char plc_network_no;	// 3
	unsigned char plc_node_no;		// 4
	unsigned char plc_cpu_unit;		// 5
	
	//
	unsigned char	sequence;			// 9  Service ID: 0x00
	unsigned char	fins_cmd_code[2];	// 10 11  Command CODE: Memory Area Read (0x0101)

	// Command Data
	unsigned char	read_err_code[2];		// 12 13  Response code: Normal completion (0x0000)
	unsigned char	bData[1617];		// 15 ~ 1632  Response data: 0000000000000000000500000007ffff

}FINSUDP_READ_RESPONSE_DATAGRAM;

typedef struct tagFinsUDPWriteMemRequest
{
	//
	unsigned char	icf;	// 0 (Information Control Field) - FINS Command�� ��Ʈ��ũ �� �ִ� CPU UNIT�� ������ ��80�� ����
	unsigned char	rsv;	// 1 (Reserved) - ��Ʈ��ũ �� �ִ� CPU Unit�� ������ ������ ��00�� ����
	unsigned char	gct;	// 2 (Gateway Count) - �ۼ����� �̷����� ��� ��Ʈ��ũ �� ��02�� PLC <-> PC
	//
	unsigned char plc_network_no;	// 3
	unsigned char plc_node_no;		// 4
	unsigned char plc_cpu_unit;		// 5
	//
	unsigned char pc_network_no;	// 6
	unsigned char pc_node_no;		// 7
	unsigned char pc_cpu_unit;		// 8
	//
	unsigned char sequence;			// 9
	unsigned char fins_cmd_code[2]; // 10 11

	unsigned char data_area_type;    // 12
	unsigned char data_start_offset[2]; // 13,14
	unsigned char bit_address;			// 15
	unsigned char write_data_length[2]; // 16,17
	unsigned char bData[1622];          // 18 ~ 1640 


}FINSUDP_WRITE_REQ_DATAGRAM;


typedef struct tagFinsUDPWriteMemResponse
{

	//
	unsigned char	icf;	// 0 (Information Control Field) - FINS Command�� ��Ʈ��ũ �� �ִ� CPU UNIT�� ������ ��80�� ����
	unsigned char	rsv;	// 1 (Reserved) - ��Ʈ��ũ �� �ִ� CPU Unit�� ������ ������ ��00�� ����
	unsigned char	gct;	// 2 (Gateway Count) - �ۼ����� �̷����� ��� ��Ʈ��ũ �� ��02�� PLC <-> PC
	//
	unsigned char plc_network_no;	// 3
	unsigned char plc_node_no;		// 4
	unsigned char plc_cpu_unit;		// 5
	//
	unsigned char pc_network_no;	// 6
	unsigned char pc_node_no;		// 7
	unsigned char pc_cpu_unit;		// 8
	//
	unsigned char sequence;			// 9
	unsigned char fins_cmd_code[2]; // 10 11

	unsigned char read_err_code[2]; // 12 13
	//
	unsigned char _dummy[26];  //14,15,~~ 38,39,40
	//x64�� �����ؼ� 8����Ʈ ������ ������

}FINSUDP_WRITE_RESPONSE_DATAGRAM;



class CFinsUDPDevice : public CFinsDevice
{
public:
	CFinsUDPDevice();
	CFinsUDPDevice(UINT32 uIPAddress, UINT16 uPortNumber, UINT32 uDeviceID);
	virtual ~CFinsUDPDevice();

	virtual UINT32 GetIPAddress();
	virtual UINT32 GetDeviceID();

	//
	virtual UINT32 Connect();
	virtual UINT32 Disconnect();
	virtual UINT32 IsFinsConnected();

	
	virtual UINT32 GetMemoryA(LPCSTR lpcCommand, PVOID pValue, INT32 nLength);
	virtual UINT32 GetMemoryW(LPCWSTR lpcwCommand, PVOID pValue, INT32 nLength);

	virtual UINT32 ReadMemory(PVOID pValue, PINT32 pnLength);


	//2023.05.02 Add

	virtual UINT32 GetNodeAddress(PINT32 pnErrorCode);
	virtual UINT32 FinsMemRead(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode);
	virtual UINT32 FinsMemWrite(UINT uAddress, INT nSize, PVOID pValue, UINT32* uFinsErrorCode);
	//
	

		//2023.05.09 Add
	virtual UINT32 SetFinsHeader(INT32 nBlockArea,
			INT32 nDestNetworkAddr, INT32 nDestNodeNum, INT32 nDestUnitAddr,
			INT32 nSourceNetworkAddr, INT32 nSourceNodeNum, INT32 nSourceUnitAddr);
	//Hostlink Header ����

	virtual UINT32 GetFinsHeader(PINT32 nBlockArea,
		PINT32 nDestNetworkAddr, PINT32 nDestNodeNum, PINT32 nDestUnitAddr,
		PINT32 nSourceNetworkAddr, PINT32 nSourceNodeNum, PINT32 nSourceUnitAddr);

protected:
	/*BOOL DeviceDetect(UINT32 uIPAddress = 0);
	
	UINT32 ReadVariableAsciiData(PCHAR pszResponse, INT nLength);*/

	UINT32 ReadVariableBinaryData(PCHAR pszResponse, INT nLength);
	UINT32 ReadDummyData();

	//UINT32 Lock();
	//UINT32 Unlock();

	UINT32 SetNodeAddress(INT32 nDestAddr, INT32 nSourceAddr);
	////2023.03.15 Add
	VOID  HostLinkHeaderSetting();
	//UINT32 GetFinsErrorCode(PCHAR pszResponse, INT nLength);
	//UINT32 GetByteOrder();
	//

private:

	//
	IN_ADDR GetLastDigitOfHostIP();
	UINT32 GetFinsResponseCode(PCHAR pszResponse, INT nLength);
	//
	//UINT32	m_uDeviceID;
	//UINT32	m_uIPAddress;
	//UINT16	m_uPortNumber;

	//
	//for UDP
	SOCKADDR m_plc_addr;

	//HANDLE	m_hMutex;

	//SOCKET m_socketPrimary;
	//UINT32 m_uByteOrder;
	//
	
	//
	FINSUDP_READ_REQ_DATAGRAM		m_read_req_datagram;
	FINSUDP_READ_RESPONSE_DATAGRAM	m_read_res_datagram;
	FINSUDP_WRITE_REQ_DATAGRAM		m_write_req_datagram;
	FINSUDP_WRITE_RESPONSE_DATAGRAM	m_write_res_datagram;

	

};

typedef CFinsUDPDevice* PCFinsUDPDevice;

#endif
//__CFINSUDP_DEVICE_H__