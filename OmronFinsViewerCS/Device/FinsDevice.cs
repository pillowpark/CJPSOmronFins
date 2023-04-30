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
        private UInt32 _uDeviceID;
        Ping _Ping = new Ping();

        public UInt32 Open(UInt32 dwIPAddress, UInt32 uPortNum)
        {
            UInt32 uRetVal = 0;
            UInt32 uDeviceType = 0;

            uRetVal = OEKFins.CJPSOFinsOpen(uDeviceType, dwIPAddress, uPortNum, out _uDeviceID);
            Console.WriteLine("[DEBUG][APP] {0} = FinsOpen( ID = {1} ) return \n", uRetVal, _uDeviceID);

            return _uDeviceID;
        }

        public UInt32 Open(String strIPAddress, UInt32 uPortNum)
        {
            String[] strField = strIPAddress.Split('.');
            UInt32 dwIPAddress  = 0;
            UInt32 uRetVal      = 0;
            UInt32 uDeviceType  = 0;

            if (strField.Length != 4)
                return Convert.ToUInt32(CJPSOF_RESULT.COFR_Failed);

            dwIPAddress = (Convert.ToUInt32(strField[0]) << 24) | (Convert.ToUInt32(strField[1]) << 16) | (Convert.ToUInt32(strField[2]) << 8) | Convert.ToUInt32(strField[3]);
            
            uRetVal = OEKFins.CJPSOFinsOpen(uDeviceType, dwIPAddress, uPortNum, out _uDeviceID);
            Console.WriteLine("[DEBUG][APP] {0} = FinsOpen( ID = {1} ) return \n", uRetVal, _uDeviceID);


            return _uDeviceID;
        }


        public UInt32 Close()
        {
            UInt32 uRetVal = 0;
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

            return uReturn;
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
            Console.WriteLine("[DEBUG][APP] SetFinsMem - CJPSOFinsMemWrite call ");
            uRetVal = OEKFins.CJPSOFinsMemWrite(_uDeviceID, uAddress, nSize, pValue, out pLength);

            Console.WriteLine("[DEBUG][APP] return {0} = CJPSOFinsMemWrite(ID={1}, Addr={2}, Size={3}",
                uRetVal, _uDeviceID, uAddress, nSize);

            return uRetVal;
        }
    }
}
