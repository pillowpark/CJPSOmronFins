

#ifndef __CDEVICE_FACTORY_H__
#define __CDEVICE_FACTORY_H__


#include "pch.h"

#include "CFinsDevice.h"
//#include "CAnalogIODevice.h"

class CDeviceFactory
{
public:
	CDeviceFactory(void);
	virtual ~CDeviceFactory(void);

	//	UINT32 AddFinsDevice(UINT32 uIPAddress, PUINT32 puDeviceID);
	//	UINT32 AddAnalogIODevice(UINT32 uIPAddress, PUINT32 puDeviceID);
	UINT32 AddFinsDevice(UINT32 uDeviceType, UINT32 uIPAddress, PUINT32 puDeviceID);

	UINT32 DeleteFinsDevice(UINT32 uDeviceID);

	PCFinsDevice GetFinsDevice(UINT32 uDeviceID);

	VOID MessageProc();

private:
	BOOL OpenMessageThread();
	BOOL CloseMessageThread();

private:
	typedef std::unique_ptr<CFinsDevice> CFinsDeviceT, * PCFinsDeviceT;
	//typedef std::unique_ptr<CAnalogIODevice> CAnalogIODeviceT, * PCAnalogIODeviceT;

	std::vector<CFinsDeviceT> m_aFinsDevice;

	HANDLE m_hMessageThread;
	BOOL m_bMessageThread;

};



#endif
//__CDEVICE_FACTORY_H__