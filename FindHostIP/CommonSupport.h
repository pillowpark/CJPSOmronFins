#pragma once



#ifndef __COMMON_SUPPORT_H__
#define __COMMON_SUPPORT_H__



#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// Windows ��� ����
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <winsock2.h>
#include <atlstr.h>
#include <vector>
#include <memory>
#include <Windows.h>

VOID DebugPrint(const TCHAR* ptszFormat, ...);



#endif
//__COMMON_SUPPORT_H__