
#ifndef __CJPS_OMRONFINSDEF_H__
#define __CJPS_OMRONFINSDEF_H__

typedef enum _CJPSOF_DEVICE_TYPE
{
	COFD_BASE_O_PLC = 0x00000000,
	COFD_NJ_PLC = 0x00001000,  
	//NX102-1200 : 리틀엔디안, 통신할때 WORD 내에서는 빅엔디안으로 보내주는중 

	COFD_NX_PLC = 0x00002000,
	COFD_CJ_PLC = 0x00003000,
	COFD_FINS_UDP = 0x00004000,
	COFD_FINS_TCP = 0x00005000


} OS_DEVICE_TYPE;

typedef enum _CJPSOF_RESULT
{
	//omron fins result
	COFR_Ok = 0,
	COFR_NotSupported = 1,
	COFR_Exception = 10,
	COFR_Connected = 11,
	COFR_NotConnected = 12,
	COFR_Failed = 21,
	COFR_BindFailed = 22,
	COFR_InvalidDevice = 31,
	COFR_InvalidCommand = 32,
	COFR_InvalidResponse = 33,
	COFR_DataRemains = 51,
	COFR_CmdLengthExceeds = 53,
	COFR_ResLengthExceeds = 54,
	COFR_RunningDownload = 61,
	//
	COFR_NodeDataRequestFail = 101,
	COFR_PLCErrorOmronFINS = 102
	

} CSF_RESULT;


typedef enum _CJPSOF_CONNECTION_STATUS
{
	COFCS_UnKnown		= 0,
	COFCS_Connected		= 1,
	COFCS_DisConnected	= 2,
	COFCS_DatagramOpen  = 3

} CSF_CON_STATUS;

#endif
//__CJPS_OMRONFINSDEF_H__