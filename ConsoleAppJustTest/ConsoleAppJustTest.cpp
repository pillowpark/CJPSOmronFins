// ConsoleAppJustTest.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>


typedef union convert_int_to_byte4 {
	unsigned int	int_val;
	unsigned short	word_val[2];
	unsigned char	byte_val[4];
}byte4_t;

typedef union convert_short_to_byte2 {
	unsigned short	word_val;
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
int main()
{
    std::cout << "Hello World!\n";

	//for debug
	printf("\n FINS_READ_REQ_PACKET sizeof( ) = %d \n", sizeof(FINS_READ_REQ_PACKET));
	//
	printf("\n FINS_READ_RESPONSE_PACKET sizeof( ) = %d \n", sizeof(FINS_READ_RESPONSE_PACKET));
	//
	printf("\n FINS_WRITE_REQ_PACKET sizeof( ) = %d \n", sizeof(FINS_WRITE_REQ_PACKET));
	//
	printf("\n FINS_WRITE_RESPONSE_PACKET sizeof( ) = %d \n", sizeof(FINS_WRITE_RESPONSE_PACKET));

	printf("\n sizeof ( byte2_t ) = %d \n", sizeof(byte2_t) );
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.




//PLC Errors for Omron FINS Series CS and CJ
//Error Message   Description
//
//0x0000 Normal Completion.
//0x0001 Service Canceled.
//0x0101 Local Error : Local node not in network.
//0x0102 Local Error : Token Timeout.
//0x0103 Local Error : Retries Failed.
//0x0104 Local Error : Too many send frames.
//0x0105 Local Error : Node address range error.
//0x0106 Local Error : Node Address Duplication.
//0x0201 Destination Node Error : Destination Node not in network.
//0x0202 Destination Node Error : Unit Missing.
//0x0203 Destination Node Error : Third Node missing.
//0x0204 Destination Node Error : Destination Node busy.
//0x0205 Destination Node Error : Response Timeout.
//0x0301 Controller Error : Communications Controller Error.
//0x0302 Controller Error : CPU Unit Error.
//0x0303 Controller Error : Controller Error.
//0x0304 Controller Error : Unit number Error.
//0x0401 Service Unsupported : Undefined Command.
//0x0402 Service Unsupported : Not supported by Model / Version.
//0x0501 Routing Table Error : Destination address setting error.
//0x0502 Routing Table Error : No routing tables.
//0x0503 Routing Table Error : Routing table error.
//0x0504 Routing Table Error : Too many delays.
//0x1001 Command Format Error : Command too long.
//0x1002 Command Format Error : Command too short.
//0x1003 Command Format Error : Elements / Data don't match.
//0x1004 Command Format Error : Command format error.
//0x1005 Command Format Error : Header Error.
//0x1101 Parameter Error : Area classification missing.
//0x1102 Parameter Error : Access Size Error.
//0x1103 Parameter Error : Address range error.
//0x1104 Parameter Error : Address range exceeded.
//0x1106 Parameter Error : Program Missing.
//0x1109 Parameter Error : Relational Error.
//0x110A Parameter Error : Duplicate Data Access.
//0x110B Parameter Error : Response too long.
//0x110C Parameter Error : Parameter Error.
//0x2002 Read Not Possible : Protected.
//0x2003 Read Not Possible : Table missing.
//0x2004 Read Not Possible : Data missing.
//0x2005 Read Not Possible : Program missing.
//0x2006 Read Not Possible : File missing.
//0x2007 Read Not Possible : Data mismatch.
//0x2101 Write Not Possible : Read Only.
//0x2102 Write Not Possible : Protected - cannot write data link table.
//0x2103 Write Not Possible : Cannot register.
//0x2105 Write Not Possible : Program missing.
//0x2106 Write Not Possible : File missing.
//0x2107 Write Not Possible : File name already exists.
//0x2108 Write Not Possible : Cannot change.
//0x2201 Not executable in current mode : Not possible during execution.
//0x2202 Not executable in current mode : Not possible while running.
//0x2203 Not executable in current mode : Wrong PLC mode(Program).
//0x2204 Not executable in current mode : Wrong PLC mode(Debug).
//0x2205 Not executable in current mode : Wrong PLC mode(Monitor).
//0x2206 Not executable in current mode : Wrong PLC mode(Run).
//0x2207 Not executable in current mode : Specified node not polling node.
//0x2208 Not executable in current mode : Step cannot be executed.
//0x2301 No such device : File device missing.
//0x2302 No such device : Missing memory.
//0x2303 No such device : Clock missing.
//0x2401 Cannot Start / Stop : Table missing.
//0x2502 Unit Error : Memory Error.
//0x2503 Unit Error : I / O setting Error.
//0x2504 Unit Error : Too many I / O points.
//0x2505 Unit Error : CPU bus error.
//0x2506 Unit Error : I / O Duplication.
//0x2507 Unit Error : I / O bus error.
//0x2509 Unit Error : SYSMAC BUS / 2 error.
//0x250A Unit Error : CPU Bus Unit Error.
//0x250D Unit Error : SYSMAC BUS No.duplication.
//0x250F Unit Error : Memory Error.
//0x2510 Unit Error : SYSMAC BUS terminator missing.
//0x2601 Command Error : No protection.
//0x2602 Command Error : Incorrect password.
//0x2604 Command Error : Protected.
//0x2605 Command Error : Service already executing.
//0x2606 Command Error : Service stopped.
//0x2607 Command Error : No execution right.
//0x2608 Command Error : Settings not complete.
//0x2609 Command Error : Necessary items not set.
//0x260A Command Error : Number already defined.
//0x260B Command Error : Error will not clear.
//0x3001 Access Right Error : No access right.
//0x4001 Abort : Service aborted.