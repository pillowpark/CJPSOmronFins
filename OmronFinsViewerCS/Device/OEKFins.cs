using System;
using System.Runtime.InteropServices;
//
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;
using System.Windows.Media.Media3D;

namespace OmronFinsViewerCS
{


    enum CJPSOF_DEVICE_TYPE
    {
        COFD_BASE_O_PLC = 0x00000000,
        COFD_NJ_PLC = 0x00001000,
        //NX102-1200 : 리틀엔디안, 통신할때 WORD 내에서는 빅엔디안으로 보내주는중 

        COFD_NX_PLC = 0x00002000,
        COFD_CJ_PLC = 0x00003000,
        COFD_FINS_UDP = 0x00004000,
        COFD_FINS_TCP = 0x00005000


    };
    
    enum CJPSOF_RESULT
    {
        //omron fins result
        COFR_Ok = 0,
        COFR_NotSupported = 1,
        COFR_Exception = 10,
        COFR_Connected = 11,
        COFR_NotConnected = 12,
        COFR_Failed = 21,
        COFR_TimeOut = 22,
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


    };


    enum CJPSOF_CONNECTION_STATUS
    {
        COFCS_UnKnown = 0,
        COFCS_Connected = 1,
        COFCS_DisConnected = 2,
        COFCS_DatagramOpen = 3

    };



    public class OEKFins
    {
#if (_x64_)   // x64는 속성에서 조건부 컴파일 기호란에 추가
        const string dllName = "CJPSOmronFins.dll";   // 64Bit 플랫폼일 때 활성화
#else
    const string dllName = "PowerPmac32.dll";   // 32Bit 플랫폼일 때 활성화
#endif


        // 라이브러리 오픈
        // CJPSOFinsOpen(디바이스타입, IP주소, PORT번호);
        // 객체생성
        [DllImport(dllName)]
        //UINT32 WINAPI CJPSOFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum, PUINT32 puDeviceID);
        public static extern UInt32 CJPSOFinsOpen(UInt32 dwDeviceType, UInt32 dwIPAddress, UInt32 uPortNum, out UInt32 puDeviceID);

        // 라이브리리 클로즈
        //객체삭제, 리소스 정리
        [DllImport(dllName)]
        // UINT32 WINAPI CJPSOFinsClose(UINT32 uDeviceID);
        public static extern UInt32 CJPSOFinsClose(UInt32 uDeviceID);





        // 상태 확인
        //객체 상태 확인(접속상태 획인)
        [DllImport(dllName)]
        // UINT32 WINAPI CJPSOFinsStatus(UINT32 uDeviceID);
        public static extern UInt32 CJPSOFinsStatus(UInt32 uDeviceID);


        // PLC에 접속 후
        //노드번호 받아오는것까지 진행
        [DllImport(dllName)]
        //UINT32 WINAPI CJPSOFinsConnect(UINT32 uDeviceID);
        public static extern UInt32 CJPSOFinsConnect(UInt32 uDeviceID);

        // 접속끊기
        [DllImport(dllName)]
        //UINT32 WINAPI CJPSOFinsDisconnect(UINT32 uDeviceID);
        public static extern UInt32 CJPSOFinsDisconnect(UInt32 uDeviceID);

        //읽어오기
        [DllImport(dllName)]
        //UINT32 WINAPI CJPSOFinsMemRead(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pReadBuff, PINT32 pnLength);
        public static extern UInt32 CJPSOFinsMemRead(UInt32 uDeviceID, UInt32 uAddress, Int32 nSize,
            Byte[] pValue, out UInt32 pnLength);

        //메모리에 쓰기
        [DllImport(dllName)]
        //UINT32 WINAPI CJPSOFinsMemWrite(UINT32 uDeviceID, UINT32 uStart, UINT32 uWordLength, PVOID pWriteBuff, PINT32 pnLength);
        public static extern UInt32 CJPSOFinsMemWrite(UInt32 uDeviceID, UInt32 uAddress, Int32 nSize,
            Byte[] pValue, out UInt32 pnLength);


        //2023.05.09 Add
        //Hostlink Header 셋팅
        [DllImport(dllName)]
        public static extern UInt32 CJPSOFinsSetNodeInfo(UInt32 uDeviceID, Int32 nBlockArea,
            Int32 nDestNetworkAddr, Int32 nDestNodeNum, Int32 nDestUnitAddr,
            Int32 nSourceNetworkAddr, Int32 nSourceNodeNum, Int32 nSourceUnitAddr);


        //Hostlink Header 확인
        [DllImport(dllName)]
        public static extern UInt32 CJPSOFinsGetNodeInfo(UInt32 uDeviceID, out Int32 nBlockArea,
            out Int32 nDestNetworkAddr, out Int32 nDestNodeNum, out Int32 nDestUnitAddr,
            out Int32 nSourceNetworkAddr, out Int32 nSourceNodeNum, out Int32 nSourceUnitAddr);
    }

}
