#include "pch.h"
#include "CDeviceFactory.h"
#include "CommonSupport.h"

#include "../Include/CJPSOmronFinsDef.h"

#ifdef _WIN32_WCE
#pragma comment(lib,"ws2.lib")
#else
#pragma comment (lib, "ws2_32.lib")
#endif

#pragma comment(lib, "iphlpapi.lib")

CDeviceFactory::CDeviceFactory()
{
	INT32 nResult;
	WSADATA wsaData;

	nResult = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nResult)
	{
		DebugPrint(_T("WSAStartup failed with error: %d\n"), nResult);
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		::WSACleanup();
		return;
	}

	//OpenMessageThread();
}

CDeviceFactory::~CDeviceFactory()
{
	DebugPrint(_T("~CDeviceFactory()\n"));

	//CloseMessageThread();

	while (m_aFinsDevice.size())
	{
		if (m_aFinsDevice[0])
			delete m_aFinsDevice[0].release();
		m_aFinsDevice.erase(m_aFinsDevice.begin() + 0);
		DebugPrint(_T("Release\n"));
	}

	m_aFinsDevice.clear();

	::WSACleanup();
}
/*
UINT32 CDeviceFactory::AddFinsDevice(UINT32 uIPAddress, PUINT32 puDeviceID)
{
	for (*puDeviceID = 0; *puDeviceID < m_aFinsDevice.size(); (*puDeviceID)++)
	{
		// 중간에 비어 있는 디바이스가 있는지 찾는다.
		if (!m_aFinsDevice[*puDeviceID].get())
			break;
	}

	if (*puDeviceID == m_aFinsDevice.size())	// 중간에 비어 있는 디바이스가 없을 경우 마지막에 추가한다.
	{

		DebugPrint(_T("AddFinsDevice Add : DeviceID=%d, IPAddress=%X\n"), *puDeviceID, uIPAddress);
		m_aFinsDevice.push_back(CFinsDeviceT(new CFinsDevice(uIPAddress, *puDeviceID)));
	}
	else	// 중간에 비어 있다면 그 디바이스를 다시 reset 해준다.
	{

		DebugPrint(_T("AddFinsDevice reset : DeviceID=%d\n"), *puDeviceID);
		m_aFinsDevice[*puDeviceID].reset(new CFinsDevice(uIPAddress, *puDeviceID));
	}

	return OR_Ok;
}

UINT32 CDeviceFactory::AddAnalogIODevice(UINT32 uIPAddress, PUINT32 puDeviceID)
{
	for (*puDeviceID = 0; *puDeviceID < m_aFinsDevice.size(); (*puDeviceID)++)
	{
		// 중간에 비어 있는 디바이스가 있는지 찾는다.
		if (!m_aFinsDevice[*puDeviceID].get())
			break;
	}

	if (*puDeviceID == m_aFinsDevice.size())	// 중간에 비어 있는 디바이스가 없을 경우 마지막에 추가한다.
	{

		DebugPrint(_T("AddAnalogIODevice Add : DeviceID=%d, IPAddress=%X\n"), *puDeviceID, uIPAddress);

		m_aFinsDevice.push_back(static_cast<CFinsDeviceT>(CAnalogIODeviceT(new CAnalogIODevice(uIPAddress, *puDeviceID))));
	}
	else	// 중간에 비어 있다면 그 디바이스를 다시 reset 해준다.
	{

		DebugPrint(_T("AddFinsDevice reset : DeviceID=%d\n"), *puDeviceID);
		m_aFinsDevice[*puDeviceID].reset(new CAnalogIODevice(uIPAddress, *puDeviceID));
	}

	return OR_Ok;
}
*/

UINT32 CDeviceFactory::AddFinsDevice(UINT32 uDeviceType, UINT32 uIPAddress, UINT16 uPortNumber,PUINT32 puDeviceID)
{
	PCFinsDevice	pFinsDevice;

	for (*puDeviceID = 0; *puDeviceID < m_aFinsDevice.size(); (*puDeviceID)++)
	{
		// 중간에 비어 있는 디바이스가 있는지 찾는다.
		if (!m_aFinsDevice[*puDeviceID].get())
			break;
	}

	switch (uDeviceType)
	{
	case COFD_BASE_O_PLC:
		pFinsDevice = new CFinsDevice(uIPAddress, uPortNumber, *puDeviceID);
		break;

	case COFD_NJ_PLC:
		//pFinsDevice = new CAnalogIODevice(uIPAddress, *puDeviceID);
		pFinsDevice = new CFinsDevice(uIPAddress, uPortNumber, *puDeviceID);
		break;

	case COFD_FINS_TCP:
		//pFinsDevice = new CAnalogIODevice(uIPAddress, *puDeviceID);
		pFinsDevice = new CFinsDevice(uIPAddress, uPortNumber, *puDeviceID);
		break;

	case COFD_FINS_UDP:
		//pFinsDevice = new CAnalogIODevice(uIPAddress, *puDeviceID);
		pFinsDevice = new CFinsUDPDevice(uIPAddress, uPortNumber, *puDeviceID);
		break;

	default:
		return COFR_InvalidDevice;
		break;
	}

	if (*puDeviceID == m_aFinsDevice.size())	// 중간에 비어 있는 디바이스가 없을 경우 마지막에 추가한다.
	{
		DebugPrint(_T("AddFinsDevice Add : DeviceType=%X, IPAddress=%X, DeviceID=%d\n"), uDeviceType, uIPAddress, *puDeviceID);
		m_aFinsDevice.push_back(CFinsDeviceT(pFinsDevice));
	}
	else	// 중간에 비어 있다면 그 디바이스를 다시 reset 해준다.
	{

		DebugPrint(_T("AddFinsDevice reset : DeviceID=%d\n"), *puDeviceID);
		m_aFinsDevice[*puDeviceID].reset(pFinsDevice);
	}

	return COFR_Ok;
}


UINT32 CDeviceFactory::DeleteFinsDevice(UINT32 uDeviceID)
{
	if (uDeviceID < m_aFinsDevice.size())
	{
		// 원시 포인터에 대한 소유권을 박탈하고 원시 포인터를 반환 한다.
		delete m_aFinsDevice[uDeviceID].release();
		//		m_aFinsDevice.erase(m_aFinsDevice.begin() + uDeviceID);
		DebugPrint(_T("DeleteFinsDevice release : DeviceID=%d\n"), uDeviceID);

		return COFR_Ok;
	}
	else
		return COFR_InvalidDevice;
}

PCFinsDevice CDeviceFactory::GetFinsDevice(UINT32 uDeviceID)
{
	if (uDeviceID < m_aFinsDevice.size())
		return m_aFinsDevice[uDeviceID].get();
	else
		return NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	//TCHAR* pMsg = _T("다른 프로그램으로 부터 메시지를 전달 받는다.");
	//DebugPrint(_T("WndProc =%d %d\n"), message, wParam);
	switch (message)
	{
	case WM_IPC:
		//	hdc = GetDC(hWnd);
		DebugPrint(_T("%d aaaaa.\n"), wParam);

		//	ReleaseDC(hWnd, hdc);
		break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);

}
HINSTANCE hInstance = GetModuleHandle(NULL);
CONST TCHAR* lpszClass = _T("OSEthernet");
VOID CDeviceFactory::MessageProc()
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = NULL;
	WndClass.hCursor = NULL;
	WndClass.hIcon = NULL;
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = NULL;
	RegisterClass(&WndClass);
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, SW_SHOW);
	DebugPrint(_T("MessageProc Start =%d\n"), hWnd);

	while (m_bMessageThread && GetMessage(&Message, NULL, 0, 0))
	{
		//TranslateMessage(&Message);
		DispatchMessage(&Message);
	}




	DebugPrint(_T("\nMessageProc End\n"));
}



DWORD WINAPI MessageThread(LPVOID lpArg)
{
	CDeviceFactory* pDeviceFactory = (CDeviceFactory*)lpArg;

	pDeviceFactory->MessageProc();

	return 0;
}

BOOL CDeviceFactory::OpenMessageThread()
{
	DebugPrint(_T("OpenMessageThread\n"));
	m_bMessageThread = TRUE;
	m_hMessageThread = CreateThread(NULL, 0, MessageThread, this, 0, NULL);

	Sleep(1000);
	HWND hWnd = FindWindow(NULL, _T("OSEthernet C/C++"));
	::SendMessage(hWnd, WM_IPC, (WPARAM)1234, (LPARAM)0);
	::SendMessage(hWnd, WM_IPC, (WPARAM)1234, (LPARAM)0);
	SendMessage((HWND)GetModuleHandle(NULL), WM_IPC, (WPARAM)1234, (LPARAM)0);
	return TRUE;
}

BOOL CDeviceFactory::CloseMessageThread()
{
	m_bMessageThread = FALSE;
	WaitForSingleObject(m_hMessageThread, INFINITE);
	CloseHandle(m_hMessageThread);
	DebugPrint(_T("CloseMessageThread\n"));
	return TRUE;
}