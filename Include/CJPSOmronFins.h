
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
	//Hostlink Header 셋팅

	//CJPSOFinsGetNodeInfo(unit DeviceID, class FinsHeader);
	//Hostlink Header 확인
	

		
		

	// 라이브러리 오픈
	//UINT32 WINAPI OSEthernetOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	//UINT32 WINAPI OSAnalogIOOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	UINT32 WINAPI CJPSOFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum, PUINT32 puDeviceID);
	// CJPSOFinsOpen(디바이스타입, IP주소, PORT번호);
	// 객체생성

	// 라이브리리 클로즈
	UINT32 WINAPI CJPSOFinsClose(UINT32 uDeviceID);
	//객체삭제, 리소스 정리

	// PLC에 접속 후
	//노드번호 받아오는것까지 진행
	UINT32 WINAPI CJPSOFinsConnect(UINT32 uDeviceID);

	// 접속끊기
	UINT32 WINAPI CJPSOFinsDisconnect(UINT32 uDeviceID);

	//읽어오기
	UINT32 WINAPI CJPSOFinsMemRead(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength);

	//메모리에 쓰기
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