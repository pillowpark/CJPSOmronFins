// ConsoleAppFinsDllTest.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "framework.h"


#include "../Include/CJPSOmronFinsDef.h"
#include "../Include/CJPSOmronFins.h"

//#include <winsock.h>

template <class T> 
void EndianSwap(T* objp)
{
    unsigned char* memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}

void FinsLibDynamicLoadTest();
void FinsLibStaticLoadTest();
int main()
{
    std::cout << "Hello ConsoleAppFinsDllTest !\n";
    //

    FinsLibDynamicLoadTest();

   //FinsLibStaticLoadTest();
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

void FinsLibStaticLoadTest()
{
    UINT32 uDeviceType = 0;
    UINT32 uIPAddress = 0;
    UINT32 uPortNum = 0;
    UINT32 uDeviceID = 0;
    UINT32 uRetVal;

    //uIPAddress = 0x7F000001; //127.0.0.1
    uRetVal =  CJPSOFinsOpen(uDeviceType, uIPAddress, uPortNum, &uDeviceID);

    return;
}

//------------------------------------------
// DLL 함수 포인터 자료형 정의

typedef UINT32(*FuncFinsOpen)(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum, PUINT32 puDeviceID);
typedef UINT32(*FuncFinsClose)(UINT32 uDeviceID);
typedef UINT32(*FuncFinsConnect)(UINT32 uDeviceID);
typedef UINT32(*FuncFinsDisconnect)(UINT32 uDeviceID);
typedef UINT32(*FuncFinsMemRead)(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength);
typedef UINT32(*FuncFinsMemWrite)(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pWriteBuff, PINT32 pnLength);


// DLL 함수 포인터 선언

FuncFinsOpen        OFinsOpen = NULL;
FuncFinsClose       OFinsClose = NULL;
FuncFinsConnect     OFinsConnect = NULL;
FuncFinsDisconnect  OFinsDisconnect = NULL;
FuncFinsMemRead     OFinsMemRead = NULL;
FuncFinsMemWrite    OFinsMemWrite = NULL;


void FinsLibDynamicLoadTest()
{

    // DLL 파일 열기

    HINSTANCE mhOmronFinsDll = LoadLibrary(_T("CJPSOmronFins.dll"));

    // 함수 포인터 얻기
    if (mhOmronFinsDll)
    {

        OFinsOpen       = (FuncFinsOpen)        GetProcAddress(mhOmronFinsDll, "CJPSOFinsOpen");
        OFinsClose      = (FuncFinsClose)       GetProcAddress(mhOmronFinsDll, "CJPSOFinsClose");
        OFinsConnect    = (FuncFinsConnect)     GetProcAddress(mhOmronFinsDll, "CJPSOFinsConnect");
        OFinsDisconnect = (FuncFinsDisconnect)  GetProcAddress(mhOmronFinsDll, "CJPSOFinsDisconnect");
        OFinsMemRead    = (FuncFinsMemRead)     GetProcAddress(mhOmronFinsDll, "CJPSOFinsMemRead");
        OFinsMemWrite   = (FuncFinsMemWrite)    GetProcAddress(mhOmronFinsDll, "CJPSOFinsMemWrite");
    }
    else
    {
        printf("[DEBUG][APP] mhOmronFinsDll == NULL Pointer \n");
    }

    UINT32 uDeviceType  =   0;
    UINT32 uIPAddress   =   0;
    UINT32 uPortNum     =   0;
    UINT32 uDeviceID    =   0;

    UINT32 uRetVall = 0;
    UINT32 uTemp32;

    UINT32  uStart = 0;
    UINT32  uWordLength = 0;
    WORD    wReadBuff[800];
    WORD    wWriteBuff[800];
    INT32   nRetLength = 0;

    UINT32  uInt32Buff[80];


    //2023.03.16 
    // DM 0 ~ 32768
    uDeviceType = COFD_FINS_UDP;
    //uIPAddress = 0x7F000001; //127.0.0.1
    //uIPAddress = 0xC0A800DD;//192.168.0.221
    //uIPAddress = 0xC0A8FA01;//192.168.250.1
    //uIPAddress = 0xC0A8FB01;//192.168.251.1
    uIPAddress = 0x0a00000a;//192.168.251.1

    uPortNum = 9600;
    //OFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum);
    if (OFinsOpen != NULL)
    {
        uRetVall = OFinsOpen(uDeviceType, uIPAddress, uPortNum, &uDeviceID);
        printf("[DEBUG][APP] %d = OFinsOpen( ID = %d ) return \n", uRetVall, uDeviceID);
    }
    else
    {
        printf("[DEBUG][APP] OFinsOpen == NULL Pointer \n");
    }

    //통신 접속
    //OFinsConnect(UINT32 uDeviceID);
    if (OFinsConnect != NULL)
    {
        printf("[DEBUG][APP] OFinsConnect Call \n");
        uRetVall = OFinsConnect(uDeviceID);
        printf("[DEBUG][APP] %d = OFinsConnect( ID = %d ) return \n", uRetVall, uDeviceID);
    }
    else
    {
        printf("[DEBUG][APP] OFinsConnect == NULL Pointer \n");
    }

    int _index = 0;
    while (true)
    {
        _index++;
        //쓰기
        //OFinsMemWrite(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pWriteBuff, PINT32 pnLength);
        if (OFinsMemWrite != NULL)
        {
            //uRetVall = OFinsMemWrite(uDeviceID, uStart, uWordLength, wWriteBuff, &nRetLength);
            wWriteBuff[0] = ((_index) & 0x0000FFFF);
            wWriteBuff[1] = ((_index >> 16) & 0x0000FFFF);
            wWriteBuff[2] = 0;
            wWriteBuff[3] = 0;
            wWriteBuff[4] = 0;
            wWriteBuff[5] = 0;

            uTemp32 = 0x12345678;
            memcpy(&wWriteBuff[2], &uTemp32, 4);

            uTemp32 = _index++;
            //EndianSwap(&uTemp32);
            memcpy(&wWriteBuff[4], &uTemp32, 4);

            uStart = 0;
            uWordLength = 16;
            memset(wReadBuff, 0, 100);
            printf("[DEBUG][APP] OFinsMemWrite( ID = %d ) call \n", uDeviceID);
            uRetVall = OFinsMemWrite(uDeviceID, uStart, uWordLength, wWriteBuff, &nRetLength);
            printf("[DEBUG][APP] %d = OFinsMemWrite(start = %d, size =%d ) return \n", uRetVall, uStart, uWordLength);

        }

        Sleep(1000);

        //읽기
        //OFinsMemRead(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength);
        //if (OFinsMemRead != NULL)
        //{
        //    uStart = 0;
        //    uWordLength = 8;
        //    memset(wReadBuff, 0, 100);
        //    printf("[DEBUG][APP] OFinsMemRead( ID = %d ) call \n", uDeviceID);
        //    uRetVall = OFinsMemRead(uDeviceID, uStart, uWordLength, wReadBuff, &nRetLength);
        //    printf("[DEBUG][APP] %d = OFinsMemRead(start = %d, size =%d ) return \n", uRetVall, uStart, uWordLength);

        //    memcpy(&uInt32Buff[0], &wReadBuff[4], 8);

        //    //
        //    printf("[DEBUG][APP][OFinsMemRead] [0]=[0x%04x] / [1]=[0x%04x] / [2]=[0x%04x] / [3]=[0x%04x] \n",
        //        wReadBuff[0], wReadBuff[1], wReadBuff[2], wReadBuff[3]);
        //    printf("[DEBUG][APP][OFinsMemRead] Int32 = [0]=[0x%08x] / [1]=[0x%08x]   \n",
        //        uInt32Buff[0], uInt32Buff[1]);

        //}
        //else
        //{
        //    printf("[DEBUG][APP] OFinsMemRead == NULL Pointer \n");
        //}

       // Sleep(1000);
    }




    //통신 끊기
    //OFinsDisconnect(UINT32 uDeviceID);
    if (OFinsDisconnect != NULL)
    {
        uRetVall = OFinsDisconnect(uDeviceID);
        printf("[DEBUG][APP] %d = OFinsDisconnect( ID = %d ) return \n", uRetVall, uDeviceID);
    }
    else
    {
        printf("[DEBUG][APP] OFinsDisconnect == NULL Pointer \n");
    }

    //라이브러리 리소스 해제
    //OFinsClose(UINT32 uDeviceID);
    if (OFinsClose != NULL)
    {
        uRetVall = OFinsClose(uDeviceID);
        printf("[DEBUG][APP] %d = OFinsClose( ID = %d ) return ", uRetVall, uDeviceID);
    }
    else
    {
        printf("[DEBUG][APP] OFinsClose == NULL Pointer \n");
    }

    // 라이브러리 해제
    if (mhOmronFinsDll)
    {
        FreeLibrary(mhOmronFinsDll);
        printf("[DEBUG][APP] FreeLibrary(mhOmronFinsDll) call \n");
    }
    


    return;
}