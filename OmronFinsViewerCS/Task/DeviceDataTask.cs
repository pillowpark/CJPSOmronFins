using Microsoft.Office.Interop.Excel;
using OmronFinsViewerCS.Device;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;
using static OmronFinsViewerCS.PlcFinsData;

namespace OmronFinsViewerCS
{
    public class DeviceDataTask
    {

        private DataManager mDataManager;
        //
        private Boolean m_bFinsDataTask = false;
        private Task m_taskFinsData = null;
        private Task m_taskFinsConnect = null;
        //
        public delegate void OnFinsPlcConnectDelegate();
        public event OnFinsPlcConnectDelegate OnFinsPlcConnect;

        public DeviceDataTask()
        {
            IniFile iniFile = new IniFile();
            //String strSettings = "SystemSetup.ini";
            string strSettings = string.Format(@"{0}\SystemSetup.ini", System.Environment.CurrentDirectory); //파일 경로
            FileInfo fileInfo = new FileInfo(strSettings);

            mDataManager = App.GetApp().GetDataManager();

            if (fileInfo.Exists)
            {
                iniFile.Load(strSettings);
                String strSectionProgram = "OMRON_PLC";
                mDataManager.m_strOmronPlcIP = iniFile[strSectionProgram]["FinsIP"].ToString();
                mDataManager.m_strOmronFinsPort = iniFile[strSectionProgram]["FinsPort"].ToInt();
                //dataManager.m_strDatabaseIP = iniFile[strSectionProgram]["DatabaseIP"].ToString();
                //pmacData._nMotorNo = iniFile[strSectionProgram]["MotorNo"].ToInt();
            }
            else
            {
                mDataManager.m_strOmronPlcIP = "192.168.250.1";
                mDataManager.m_strOmronFinsPort = 9601;
                // dataManager.m_strDatabaseIP = "127.0.0.1";
                //pmacData._nMotorNo = 1;
            }

            //for Debug
            Console.WriteLine("Fins IP = {0} / Fins Port = {1}",
                mDataManager.m_strOmronPlcIP, mDataManager.m_strOmronFinsPort);


            // PlcFinsData plcFinsData = new PlcFinsData();
        }



        public void SatartDeviceDataTask()
        {
            m_taskFinsData = new Task(new System.Action(taskFinsDataReadLoop));
            //	m_taskPmacConnect = new Task(new Action<Int32>(taskPmacConnectLoop), 10);
            //m_taskEthernetData = new Task(new Action(taskEthernetDataLoop));

            m_bFinsDataTask = true;
            m_taskFinsData.Start();

            //m_bEthernetDataTask = true;
            //m_taskEthernetData.Start();
        }


        public void StoptDeviceDataTask()
        {
            //m_bEthernetDataTask = false;
            //m_taskEthernetData.Wait();

            m_bFinsDataTask = false;
            m_taskFinsData.Wait();
        }

        public void OneShotWriteArea()
        {
            Console.WriteLine("DeviceDataTask::OneShotWriteArea()");
            taskFinsDataWriteLoop();
        }

        private void taskFinsDataWriteLoop()
        {
            Console.WriteLine("DeviceDataTask::taskFinsDataWriteLoop()");

            UInt32 uResult;
            UInt32 uAddress;
            Int32 nSize;
            Byte[] bBuffer4ReadArea;
            UInt32 uLength;

            Int32 nIsConnected;
            nIsConnected = 1;

            if (nIsConnected == 0)
                return;

            FinsDevice _finsDevice = mDataManager.GetFinsDevice();
            PlcFinsData _fincData = mDataManager.GetFinsData();

           
            reg_data[] _reg_wrtie_data = _fincData.GetWriteData();
            //pmacDevice.Open("192.168.0.200", (UInt32)DTK_MODE_TYPE.DM_GPASCII);
            //StartPmacConnectTask();

            uint _byte_offset;
            uint _count;
            uint _size;
            bool _unsigned;

            //
            uAddress = _fincData.WriteStartIndex;
            nSize = (int)_fincData.WriteWordSize;
            bBuffer4ReadArea = new Byte[(nSize * 2)];
            //
            _count = _fincData.WriteVariableCount;
            _byte_offset = 0;
            try
            {

                UInt16 _set_value_u16;
                Int16 _set_value_s16;
                byte[] _byteArray;

                for (int i = 0; i < _count; ++i)
                {
                    _unsigned = _reg_wrtie_data[i].unsign;
                    _size = _reg_wrtie_data[i].size;
                    if (_size == 2)
                    {
                        if (_unsigned)
                        {
                            try
                            {
                                _set_value_u16 = Convert.ToUInt16(_reg_wrtie_data[i].set_value);
                            }
                            catch (Exception ex)
                            {
                                Console.WriteLine(ex.Message);
                                _set_value_u16 = 0;
                            }
                            finally
                            {                                
                            }
                            _byteArray = BitConverter.GetBytes(_set_value_u16);
                        }
                        else
                        {
                            _set_value_s16 = Convert.ToInt16(_reg_wrtie_data[i].set_value);
                            _byteArray = BitConverter.GetBytes(_set_value_s16);
                        }     
                                                

                        //바이트 오더 고려해서 작업할것
                        bBuffer4ReadArea[_byte_offset + 0] = _byteArray[ 0];
                        bBuffer4ReadArea[_byte_offset + 1] = _byteArray[ 1];

                    }
                    else
                    {
                        if (_unsigned)
                        {
                            UInt32 _set_value=0;
                            try {
                                _set_value = Convert.ToUInt32(_reg_wrtie_data[i].set_value);
                            }
                            catch(Exception ex) 
                            { 
                                Console.WriteLine(ex.Message);
                                _set_value = 0;
                            }
                            finally
                            {
                                
                            }
                            _byteArray = BitConverter.GetBytes(_set_value);
                        }
                        else
                        {
                            Int32 _set_value = Convert.ToInt32(_reg_wrtie_data[i].set_value);
                            _byteArray = BitConverter.GetBytes(_set_value);
                        }

                        //바이트 오더 고려해서 작업할것
                        bBuffer4ReadArea[_byte_offset + 0] = _byteArray[ 0];
                        bBuffer4ReadArea[_byte_offset + 1] = _byteArray[ 1];
                        bBuffer4ReadArea[_byte_offset + 2] = _byteArray[ 2];
                        bBuffer4ReadArea[_byte_offset + 3] = _byteArray[ 3];
                    }

                    _byte_offset = _byte_offset + _size;

                }//end for (int i = 0; i < _count; ++i)
            }
            catch (IndexOutOfRangeException ioex)
            { 
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }
            uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);
           //Console.WriteLine("taskFinsDataWriteLoop()::  {2} = SetFinsMem:: Address = {0} / Size = {1}",
            //  uAddress, nSize, uResult);
        }


        private void taskFinsDataReadLoop()
        {
            Int32 nIndex = 0;
            Int32 nIsConnected;
            UInt32 uResult;
            String strCommand;
            String[] astrResponse;
            
            
            //
            
            UInt32 uAddress4ReadArea;
            Int32 nSize4ReadArea;
            Byte[] bBuffer4ReadArea;
            //
            UInt32 uAddress4WriteArea;
            Int32 nSize4WriteArea;
            Byte[] bBuffer4WriteArea;
            //
            UInt32 uLength;

            IntPtr pRespBuffer;


            FinsDevice   _finsDevice   = mDataManager.GetFinsDevice();
            PlcFinsData _fincData    = mDataManager.GetFinsData();

            //pmacDevice.Open("192.168.0.200", (UInt32)DTK_MODE_TYPE.DM_GPASCII);
            //StartPmacConnectTask();


            uAddress4ReadArea = _fincData.ReadStartIndex;
            nSize4ReadArea = (int)_fincData.ReadWordSize;
            bBuffer4ReadArea = new Byte[(nSize4ReadArea * 2)];
            //
            uAddress4WriteArea = _fincData.WriteStartIndex; 
            nSize4WriteArea = (int)_fincData.WriteWordSize;
            bBuffer4WriteArea = new Byte[(nSize4WriteArea * 2)];
            //
            while (m_bFinsDataTask)
            {
                Thread.Sleep(500);
                //Task.Delay(5000);
                //uResult = pmacDevice.IsConnected(out nIsConnected);
                nIsConnected = 1;

                if (nIsConnected == 0)
                    continue;

              
                uResult = _finsDevice.GetFinsMem(uAddress4ReadArea, nSize4ReadArea, bBuffer4ReadArea, out uLength);
               // Console.WriteLine("taskFinsDataLoop()::[Read Area]  {2} = GetFinsMem:: Address = {0} / Size = {1}",
                //  uAddress4ReadArea, nSize4ReadArea, uResult);
                //uResult = pmacDevice.GetResponse(strCommand, out astrResponse);


                // "Motor[5].HomePos Motor[5].ActPos Motor[5].ActVel Motor[5].DesPos"
                if (uResult > (UInt32)CJPSOF_RESULT.COFR_Ok)
                {
                    if (uResult > 1000 || uResult == (UInt32)CJPSOF_RESULT.COFR_TimeOut || uResult == (UInt32)CJPSOF_RESULT.COFR_Failed 
                        || uResult == (UInt32)CJPSOF_RESULT.COFR_DataRemains || uResult == (UInt32)CJPSOF_RESULT.COFR_NotConnected)
                    {
                        System.Diagnostics.Debug.WriteLine("통신 에러 발생 : GetResponse ErrorCode={0}", uResult);
                        _finsDevice.Disconnect();
                        App.GetApp().GetMainWindow().Dispatcher.BeginInvoke(new System.Action(OnFinsPlcConnect));
                        //StartPmacConnectTask();
                    }
                    continue;
                }


                _fincData.ReadAreaDataParsing(bBuffer4ReadArea);
                _fincData.ReadAreaDataCopyNChangeCheck();

                //----------------------

                //Write Area Read

                Thread.Sleep(500);
               

                uResult = _finsDevice.GetFinsMem(uAddress4WriteArea, nSize4WriteArea, bBuffer4WriteArea, out uLength);
               // Console.WriteLine("taskFinsDataLoop()::[Write Area]  {2} = GetFinsMem:: Address = {0} / Size = {1}",
               //   uAddress4WriteArea, nSize4WriteArea, uResult);


                // "Motor[5].HomePos Motor[5].ActPos Motor[5].ActVel Motor[5].DesPos"
                if (uResult > (UInt32)CJPSOF_RESULT.COFR_Ok)
                {
                    if (uResult > 1000 || uResult == (UInt32)CJPSOF_RESULT.COFR_TimeOut || uResult == (UInt32)CJPSOF_RESULT.COFR_Failed
                        || uResult == (UInt32)CJPSOF_RESULT.COFR_DataRemains || uResult == (UInt32)CJPSOF_RESULT.COFR_NotConnected)
                    {
                        System.Diagnostics.Debug.WriteLine("통신 에러 발생 : GetResponse ErrorCode={0}", uResult);
                        _finsDevice.Disconnect();
                        App.GetApp().GetMainWindow().Dispatcher.BeginInvoke(new System.Action(OnFinsPlcConnect));
                        //StartPmacConnectTask();
                    }
                    continue;
                }


                _fincData.WriteAreaDataParsing(bBuffer4WriteArea);
                _fincData.WriteAreaDataCopyNChangeCheck();


                //pmacData.sSystem.dServoPeriod = Convert.ToDouble(astrResponse[0]);


                /*if (pmacData.IsGatherEnable())
				{

				}*/


                /*

				Double dd;
				int i;
				String strCommand2 = "";
				String strResponse;
				for (i = 0; i < 574 * 6; i+=6)
				{
					//strCommand2 = string.Format("gather.ddata[{0}]", i + 1);
					strCommand2 = string.Format("Sys.ddata[{0}]", i + 1+ 1024);
					uResult = pmacDevice.GetResponse(strCommand2, out strResponse);

					//dd = Convert.ToDouble(strResponse) - Convert.ToDouble(astrResponse[20]);
					dd = Convert.ToDouble(strResponse);
					System.Diagnostics.Debug.WriteLine(dd.ToString("F6"));
				}*/

                nIndex++;
            }
        }//end private void taskFinsDataLoop()

    }//end public class DeviceDataTask
}//end namespace OmronFinsViewerCS
