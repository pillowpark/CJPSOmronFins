// CJPSOmronFins.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//


#include "pch.h"

#include "../Include/CJPSOmronFins.h"
#include "CommonSupport.h"
#include "CDeviceFactory.h"
#include "CFinsDevice.h"


CDeviceFactory	g_DeviceFactory;

// �ܺΰ��� ���ϰ� ����ϴ� �Լ�
UINT32 IsFinsConnected(UINT32 uDeviceID)
{
	UINT32 uResult = 0;
	PCFinsDevice pFinsDevice;

	//m_socketPrimary = NULL;
	pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
	if (pFinsDevice)
	{
		uResult = pFinsDevice->IsFinsConnected();
	}
	else
	{
		uResult = COFR_InvalidDevice;
	}

	return uResult;
}

// ���̺귯�� ����
// ��ü����
// CJPSOFinsOpen(����̽�Ÿ��, IP�ּ�, PORT��ȣ);
UINT32 WINAPI CJPSOFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum, PUINT32 puDeviceID)
{
	//for debug
	DebugPrint(_T("[DebugPring] CJPSOFinsOpen(%d) call..\n"), uDeviceType);

	UINT32 uResult = 0;

	//uResult = g_DeviceFactory.AddFinsDevice(uIPAddress, puDeviceID);
	uResult = g_DeviceFactory.AddFinsDevice(uDeviceType, uIPAddress, (UINT16)uPortNum, puDeviceID);


	if (uResult == COFR_Ok)
	{
		//uResult = OSConnect(*puDeviceID);
		if (uResult != COFR_Ok)
			g_DeviceFactory.DeleteFinsDevice(*puDeviceID);
	}
	else
	{
		g_DeviceFactory.DeleteFinsDevice(*puDeviceID); //2023.03.15 ����
	}

	return uResult;
}

// ���̺긮�� Ŭ����
//��ü����, ���ҽ� ����
UINT32 WINAPI CJPSOFinsClose(UINT32 uDeviceID)
{
	bool _isConnected = false;

	//for debug
	DebugPrint(_T("[DebugPring] CJPSOFinsClose(%d) call..\n"), uDeviceID);
		

	if( (IsFinsConnected(uDeviceID) == COFR_Connected) )
		CJPSOFinsDisconnect(uDeviceID);

	return g_DeviceFactory.DeleteFinsDevice(uDeviceID);
	
}


// PLC�� ���� ��
//����ȣ �޾ƿ��°ͱ��� ����
UINT32 WINAPI CJPSOFinsConnect(UINT32 uDeviceID)
{
	//for debug
	DebugPrint(_T("[DebugPrint] CJPSOFinsConnect(%d) call..\n"), uDeviceID);

	UINT32 uResult = 0;
	PCFinsDevice pFinsDevice;
	INT32 nErrorCode;
	
	pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
	//
	
	if (pFinsDevice)
	{		
		uResult = pFinsDevice->Connect();		
	}
	else
	{
		uResult = COFR_InvalidDevice;
	}

	DebugPrint(_T("[DebugPrint] CJPSOFinsConnect(%d)::Connect() return = %d\n", uDeviceID, uResult));

	if (uResult == COFR_Ok)
	{			

		pFinsDevice->GetNodeAddress(&nErrorCode);

		if (nErrorCode != 0)
		{
			uResult = COFR_NodeDataRequestFail;
			CJPSOFinsDisconnect(uDeviceID);

			//for debug
			DebugPrint(_T("[DebugPring] CJPSOFinsDisconnect(%d) call..\n"), uDeviceID);
		}	

	}
	return uResult;


}

// ���Ӳ���
UINT32 WINAPI CJPSOFinsDisconnect(UINT32 uDeviceID)
{
	//for debug
	DebugPrint(_T("[DebugPring] CJPSOFinsDisconnect(%d) call..\n"), uDeviceID);

	UINT32 uResult = 0;
	PCFinsDevice pFinsDevice;

	pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
	if (pFinsDevice)
	{
		uResult = pFinsDevice->Disconnect();
	}
	else
		uResult = COFR_InvalidDevice;

	return uResult;
}
//�о����
UINT32 WINAPI CJPSOFinsMemRead(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength)
{
	//for debug
	DebugPrint(_T("[DebugPring] CJPSOFinsMemRead(%d) call..\n"), uDeviceID);

	UINT32 uResult = 0;
	PCFinsDevice pFinsDevice;
	UINT32 uFinsErrCode = 0;

	pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
	if (pFinsDevice)
	{
		uResult = pFinsDevice->FinsMemRead(uStart, uWordLength, pReadBuff, &uFinsErrCode);
		//(UINT uAddress, INT nSize, PVOID pValue)
	}
	else
		uResult = COFR_InvalidDevice;

	return uResult;
}

//�޸𸮿� ����
UINT32 WINAPI CJPSOFinsMemWrite(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pWriteBuff, PINT32 pnLength)
{
	//for debug
	DebugPrint(_T("[DebugPring] CJPSOFinsMemWrite(%d) call..\n"), uDeviceID);
	UINT32 uResult = 0;
	PCFinsDevice pFinsDevice;
	UINT32 uFinsErrCode = 0;


	pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
	if (pFinsDevice)
	{
		uResult = pFinsDevice->FinsMemWrite(uStart, uWordLength, pWriteBuff , &uFinsErrCode);
		//(UINT uAddress, INT nSize, PVOID pValue)
	}
	else
		uResult = COFR_InvalidDevice;

	return uResult;
}

//2023.05.09 Add
//Hostlink Header ����
UINT32 WINAPI CJPSOFinsSetNodeInfo(UINT32 uDeviceID, INT32 nBlockArea,
	INT32 nDestNetworkAddr, INT32 nDestNodeNum, INT32 nDestUnitAddr,
	INT32 nSourceNetworkAddr, INT32 nSourceNodeNum, INT32 nSourceUnitAddr)
{
	
		//for debug
		DebugPrint(_T("[DebugPring] CJPSOFinsSetNodeInfo(%d) call..\n"), uDeviceID);

		UINT32 uResult = 0;
		PCFinsDevice pFinsDevice;
		UINT32 uFinsErrCode = 0;

		pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
		if (pFinsDevice)
		{
			uResult = pFinsDevice->SetFinsHeader( nBlockArea,
				 nDestNetworkAddr,  nDestNodeNum,  nDestUnitAddr,
				 nSourceNetworkAddr,  nSourceNodeNum,  nSourceUnitAddr);
			//(UINT uAddress, INT nSize, PVOID pValue)
		}
		else
			uResult = COFR_InvalidDevice;

		return uResult;
	
}

//Hostlink Header Ȯ��
UINT32 WINAPI CJPSOFinsGetNodeInfo(UINT32 uDeviceID, PINT32 nBlockArea,
	PINT32 nDestNetworkAddr, PINT32 nDestNodeNum, PINT32 nDestUnitAddr,
	PINT32 nSourceNetworkAddr, PINT32 nSourceNodeNum, PINT32 nSourceUnitAddr)
{
	//for debug
	DebugPrint(_T("[DebugPring] CJPSOFinsGetNodeInfo(%d) call..\n"), uDeviceID);

	UINT32 uResult = 0;
	PCFinsDevice pFinsDevice;
	UINT32 uFinsErrCode = 0;

	pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
	if (pFinsDevice)
	{
		uResult = pFinsDevice->GetFinsHeader(nBlockArea,
			nDestNetworkAddr, nDestNodeNum, nDestUnitAddr,
			nSourceNetworkAddr, nSourceNodeNum, nSourceUnitAddr);
		//(UINT uAddress, INT nSize, PVOID pValue)
	}
	else
		uResult = COFR_InvalidDevice;

	return uResult;
}

// ���� Ȯ��
//��ü ���� Ȯ��(���ӻ��� ȹ��)
UINT32 WINAPI CJPSOFinsStatus(UINT32 uDeviceID)
{
	CSF_CON_STATUS uResult;

	uResult = COFCS_UnKnown;

	return uResult;
}



//2023.05.15 Add
	// Single Character �������� ����
UINT32 WINAPI CJPSOFinsGetLastErrorA(UINT32 uDeviceID, LPSTR lpErrorString, INT32 nLength)
{
	UINT32 uResult;
	PCFinsDevice pFinsDevice;


	pFinsDevice = g_DeviceFactory.GetFinsDevice(uDeviceID);
	if (pFinsDevice)
	{
		uResult = pFinsDevice->GetLastErrorA(lpErrorString, nLength);
	}
	else
		uResult = COFR_InvalidDevice;

	return uResult;
}