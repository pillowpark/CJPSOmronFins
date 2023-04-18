
#ifndef __CJPS_OMRONFINS_H__
#define __CJPS_OMRONFINS_H__

#ifndef __ATLBASE_H__
#include <windows.h>
#endif
#include "CJPSOmronFinsDef.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus



	

	

	//CJPSOFinsSetNodeInfo(unit DeviceID, class FinsHeader);
	//Hostlink Header ����

	//CJPSOFinsGetNodeInfo(unit DeviceID, class FinsHeader);
	//Hostlink Header Ȯ��
	

		
		

	// ���̺귯�� ����
	//UINT32 WINAPI OSEthernetOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	//UINT32 WINAPI OSAnalogIOOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	UINT32 WINAPI CJPSOFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum, PUINT32 puDeviceID);
	// CJPSOFinsOpen(����̽�Ÿ��, IP�ּ�, PORT��ȣ);
	// ��ü����

	// ���̺긮�� Ŭ����
	UINT32 WINAPI CJPSOFinsClose(UINT32 uDeviceID);
	//��ü����, ���ҽ� ����

	// PLC�� ���� ��
	//����ȣ �޾ƿ��°ͱ��� ����
	UINT32 WINAPI CJPSOFinsConnect(UINT32 uDeviceID);

	// ���Ӳ���
	UINT32 WINAPI CJPSOFinsDisconnect(UINT32 uDeviceID);

	//�о����
	UINT32 WINAPI CJPSOFinsMemRead(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength);

	//�޸𸮿� ����
	UINT32 WINAPI CJPSOFinsMemWrite(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pWriteBuff, PINT32 pnLength);


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