using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

namespace OmronFinsViewerCS.Device
{
    public class FinsDevice
    {
        public class FinsHeaderT
        {
            public Int32 DNA;
            public Int32 DA1;
            public Int32 DA2;
            public Int32 SNA;
            public Int32 SA1;
            public Int32 SA2;

            public FinsHeaderT(int _dna, int _da1, int _da2, int _sna, int _sa1, int _sa2)
            {
                this.DNA = _dna;
                this.DA1 = _da1;
                this.DA2 = _da2;
                this.SNA = _sna;
                this.SA1 = _sa1;
                this.SA2 = _sa2;
            }
        }
    
        private UInt32 _uDeviceID;
        Ping _Ping = new Ping();

        private bool m_IsConected;

        public UInt32 Open(UInt32 dwIPAddress, UInt32 uPortNum)
        {
            UInt32 uRetVal = 0;
            UInt32 uDeviceType = (UInt32)CJPSOF_DEVICE_TYPE.COFD_FINS_UDP;
            m_IsConected = false;


            uRetVal = OEKFins.CJPSOFinsOpen(uDeviceType, dwIPAddress, uPortNum, out _uDeviceID);
            Console.WriteLine("[DEBUG][APP] {0} = FinsOpen( ID = {1} ) return \n", uRetVal, _uDeviceID);

            return _uDeviceID;
        }

        public UInt32 Open(String strIPAddress, UInt32 uPortNum, UInt32 uDeviceType)
        {
            String[] strField = strIPAddress.Split('.');
            UInt32 dwIPAddress  = 0;
            UInt32 uRetVal      = 0;
            // UInt32 uDeviceType  = 0;

            UInt32 dwDeviceType = uDeviceType;// (UInt32)CJPSOF_DEVICE_TYPE.COFD_FINS_UDP;

            m_IsConected = false;

            if (strField.Length != 4)
                return Convert.ToUInt32(CJPSOF_RESULT.COFR_Failed);

            dwIPAddress = (Convert.ToUInt32(strField[0]) << 24) | (Convert.ToUInt32(strField[1]) << 16) | (Convert.ToUInt32(strField[2]) << 8) | Convert.ToUInt32(strField[3]);
            
            uRetVal = OEKFins.CJPSOFinsOpen(dwDeviceType, dwIPAddress, uPortNum, out _uDeviceID);
            Console.WriteLine("[DEBUG][APP] {0} = FinsOpen( ID = {1} ) return \n", uRetVal, _uDeviceID);


            return _uDeviceID;
        }


        public UInt32 Close()
        {
            UInt32 uRetVal = 0;
            //
            m_IsConected = false;
            //
            uRetVal = OEKFins.CJPSOFinsClose(_uDeviceID);
            Console.WriteLine("[DEBUG][APP] {0} = OFinsClose( ID =  {1} ) return ", uRetVal, _uDeviceID);

            return uRetVal;
        }
    

        public UInt32 Connect()
        {
            UInt32 uReturn;
            uReturn = OEKFins.CJPSOFinsConnect(_uDeviceID);
            Console.WriteLine("[DEBUG][APP] {0} = OFinsConnect( ID =  {1} ) return \n", uReturn, _uDeviceID);
            //
            m_IsConected = true;
            //if (uReturn == (UInt32)DTK_STATUS.DS_Ok) SetEchoMode(7);
            //_bIsConnected = uReturn == (UInt32)DTK_STATUS.DS_Ok ? true : false;

            return uReturn;
        }

        public UInt32 Disconnect()
        {
            UInt32 uReturn;
            //_bIsConnected = false;
            uReturn = OEKFins.CJPSOFinsDisconnect(_uDeviceID);
            Console.WriteLine("[DEBUG][APP] {0} = OFinsDisconnect( ID = {1} ) return \n", uReturn, _uDeviceID);

            m_IsConected = false;
            //
            return uReturn;
        }

        public bool IsConected() 
        {
            return m_IsConected;
        }
        public UInt32 GetFinsMem(UInt32 uAddress, Int32 nSize, Byte[] pValue, out UInt32 pLength)
        {
            uint uRetVal = 0;
            //Console.WriteLine("[DEBUG][APP] GetFinsMem - CJPSOFinsMemRead call ");
            uRetVal = OEKFins.CJPSOFinsMemRead(_uDeviceID, uAddress, nSize, pValue, out pLength);
            //Console.WriteLine("[DEBUG][APP] return {0} = CJPSOFinsMemRead(ID={1}, Addr={2}, Size={3}", uRetVal, _uDeviceID, uAddress,nSize);
            return uRetVal;

        }

        public UInt32 SetFinsMem(UInt32 uAddress, Int32 nSize, Byte[] pValue, out UInt32 pLength)
        {
            uint uRetVal = 0;
            //Console.WriteLine("[DEBUG][APP] SetFinsMem - CJPSOFinsMemWrite call ");
            uRetVal = OEKFins.CJPSOFinsMemWrite(_uDeviceID, uAddress, nSize, pValue, out pLength);

            //Console.WriteLine("[DEBUG][APP] return {0} = CJPSOFinsMemWrite(ID={1}, Addr={2}, Size={3}", uRetVal, _uDeviceID, uAddress, nSize);

            return uRetVal;
        }


        public UInt32 SetFinsHeader(FinsHeaderT _hotlink_header, Int32 FinsBlockArea)
        {
            uint uRetVal = 0;
            Int32 nBlockArea;
            Int32 nDestNetworkAddr;
            Int32 nDestNodeNum;
            Int32 nDestUnitAddr;
            Int32 nSourceNetworkAddr;
            Int32 nSourceNodeNum;
            Int32 nSourceUnitAddr;
            //
            uRetVal = 0;
            //
            nBlockArea = FinsBlockArea;
            nDestNetworkAddr = _hotlink_header.DNA;
            nDestNodeNum = _hotlink_header.DA1;
            nDestUnitAddr = _hotlink_header.DA2;
            nSourceNetworkAddr = _hotlink_header.SNA;
            nSourceNodeNum = _hotlink_header.SA1;
            nSourceUnitAddr = _hotlink_header.SA2;
            
            //
            Console.WriteLine("[DEBUG][APP] SetFinsHeader - CJPSOFinsSetNodeInfo call ");
            uRetVal = OEKFins.CJPSOFinsSetNodeInfo(_uDeviceID, 
                  nBlockArea,
             nDestNetworkAddr,  nDestNodeNum,  nDestUnitAddr,
             nSourceNetworkAddr,  nSourceNodeNum,  nSourceUnitAddr);
            
            return uRetVal;
        }
    }
}
