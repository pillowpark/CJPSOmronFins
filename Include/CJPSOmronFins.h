
#ifndef __CJPS_OMRONFINS_H__
#define __CJPS_OMRONFINS_H__

#ifndef __ATLBASE_H__
#include <windows.h>
#endif
#include "CJPSOmronFinsDef.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus



	

	

		

	// ���̺귯�� ����
	//UINT32 WINAPI OSEthernetOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	//UINT32 WINAPI OSAnalogIOOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	UINT32 WINAPI CJPSOFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum, PUINT32 puDeviceID);
	// CJPSOFinsOpen(����̽�Ÿ��, IP�ּ�, PORT��ȣ);
	// ��ü����

	// ���̺긮�� Ŭ����
	UINT32 WINAPI CJPSOFinsClose(UINT32 uDeviceID);
	//��ü����, ���ҽ� ����

	// ���� Ȯ��
	//��ü ���� Ȯ��(���ӻ��� ȹ��)
	UINT32 WINAPI CJPSOFinsStatus(UINT32 uDeviceID);
	

	// PLC�� ���� ��
	//����ȣ �޾ƿ��°ͱ��� ����
	UINT32 WINAPI CJPSOFinsConnect(UINT32 uDeviceID);

	// ���Ӳ���
	UINT32 WINAPI CJPSOFinsDisconnect(UINT32 uDeviceID);

	//�о����
	UINT32 WINAPI CJPSOFinsMemRead(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength);

	//�޸𸮿� ����
	UINT32 WINAPI CJPSOFinsMemWrite(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pWriteBuff, PINT32 pnLength);



	//2023.05.09 Add
	//Hostlink Header ����
	UINT32 WINAPI CJPSOFinsSetNodeInfo(UINT32 uDeviceID, INT32 nBlockArea, 
		INT32 nDestNetworkAddr, INT32 nDestNodeNum, INT32 nDestUnitAddr,
		INT32 nSourceNetworkAddr, INT32 nSourceNodeNum, INT32 nSourceUnitAddr );
	

	//Hostlink Header Ȯ��
	UINT32 WINAPI CJPSOFinsGetNodeInfo(UINT32 uDeviceID, PINT32 nBlockArea,
		PINT32 nDestNetworkAddr, PINT32 nDestNodeNum, PINT32 nDestUnitAddr,
		PINT32 nSourceNetworkAddr, PINT32 nSourceNodeNum, PINT32 nSourceUnitAddr);
	

	//2023.05.15 Add
	// Single Character �������� ����
	UINT32 WINAPI CJPSOFinsGetLastErrorA(UINT32 uDeviceID, LPSTR lpErrorString, INT32 nLength);

#ifdef UNICODE
//#define OSGetResponse			OSGetResponseW
//#define OSGetMemory				OSGetMemoryW
#else
//#define OSGetResponse			OSGetResponseA
//#define OSGetMemory				OSGetMemoryA
#endif

#ifdef __cplusplus
}
#endif	//__cplusplus


#endif