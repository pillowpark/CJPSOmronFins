
#ifndef __CJPS_OMRONFINS_H__
#define __CJPS_OMRONFINS_H__

#ifndef __ATLBASE_H__
#include <windows.h>
#endif
#include "CJPSOmronFinsDef.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus



	

	

		

	// 라이브러리 오픈
	//UINT32 WINAPI OSEthernetOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	//UINT32 WINAPI OSAnalogIOOpen(UINT32 uIPAddress, PUINT32 puDeviceID);
	UINT32 WINAPI CJPSOFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum, PUINT32 puDeviceID);
	// CJPSOFinsOpen(디바이스타입, IP주소, PORT번호);
	// 객체생성

	// 라이브리리 클로즈
	UINT32 WINAPI CJPSOFinsClose(UINT32 uDeviceID);
	//객체삭제, 리소스 정리

	// 상태 확인
	//객체 상태 확인(접속상태 획인)
	UINT32 WINAPI CJPSOFinsStatus(UINT32 uDeviceID);
	

	// PLC에 접속 후
	//노드번호 받아오는것까지 진행
	UINT32 WINAPI CJPSOFinsConnect(UINT32 uDeviceID);

	// 접속끊기
	UINT32 WINAPI CJPSOFinsDisconnect(UINT32 uDeviceID);

	//읽어오기
	UINT32 WINAPI CJPSOFinsMemRead(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength);

	//메모리에 쓰기
	UINT32 WINAPI CJPSOFinsMemWrite(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pWriteBuff, PINT32 pnLength);



	//2023.05.09 Add
	//Hostlink Header 셋팅
	UINT32 WINAPI CJPSOFinsSetNodeInfo(UINT32 uDeviceID, INT32 nBlockArea, 
		INT32 nDestNetworkAddr, INT32 nDestNodeNum, INT32 nDestUnitAddr,
		INT32 nSourceNetworkAddr, INT32 nSourceNodeNum, INT32 nSourceUnitAddr );
	

	//Hostlink Header 확인
	UINT32 WINAPI CJPSOFinsGetNodeInfo(UINT32 uDeviceID, PINT32 nBlockArea,
		PINT32 nDestNetworkAddr, PINT32 nDestNodeNum, PINT32 nDestUnitAddr,
		PINT32 nSourceNetworkAddr, PINT32 nSourceNodeNum, PINT32 nSourceUnitAddr);
	

	//2023.05.15 Add
	// Single Character 형식으로 제어
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