using Microsoft.Office.Interop.Excel;
using OmronFinsViewerCS.Device;
using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.IO;
using System.Linq;
using System.Net;
using System.Security.RightsManagement;
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
        private Boolean m_bManualDriveTask = false;
        private CJogMoveData m_JogMoveCtrlCmd = null;
        //
        private Task m_taskFinsData = null;
        private Task m_taskFinsConnect = null;
        private Task m_taskManualDrive = null;
        //
        public delegate void OnFinsPlcConnectDelegate();
        public event OnFinsPlcConnectDelegate OnFinsPlcConnect;

        public DeviceDataTask()
        {
            IniFile iniFile = new IniFile();
            //String strSettings = "SystemSetup.ini";
            string strSettings = string.Format(@"{0}\SystemSetup.ini", System.Environment.CurrentDirectory); //파일 경로
            FileInfo fileInfo = new FileInfo(strSettings);

            //
            Int32 nTempInt32;

            mDataManager = App.GetApp().GetDataManager();
            m_JogMoveCtrlCmd = mDataManager.GetJogCmdData();

            if (fileInfo.Exists)
            {
                iniFile.Load(strSettings);
                String strSectionProgram = "OMRON_PLC";
                mDataManager.m_strOmronPlcIP = iniFile[strSectionProgram]["FinsIP"].ToString();
                mDataManager.m_nOmronFinsPort = iniFile[strSectionProgram]["FinsPort"].ToInt();
                //
                nTempInt32 = iniFile[strSectionProgram]["FinsTCP1UDP2Sel"].ToInt();
                if (nTempInt32 == 1)
                {
                    mDataManager.m_nOmronFinsTCPUDPSel = 1; //1: TCP, 2:UDP
                }
                else 
                {
                    mDataManager.m_nOmronFinsTCPUDPSel = 2; //1: TCP, 2:UDP
                }
                //
                nTempInt32 = iniFile[strSectionProgram]["FinsArea"].ToInt();
                if (nTempInt32 == 0)
                {
                    mDataManager.m_nOmronFinsAreaBlock = 0x82; //0x82 : DM영역
                }
                else
                {
                    mDataManager.m_nOmronFinsAreaBlock = nTempInt32;
                }
                //
                mDataManager.m_nOmronFinsDNA = iniFile[strSectionProgram]["FinsDNA"].ToInt();
                mDataManager.m_nOmronFinsDA1 = iniFile[strSectionProgram]["FinsDA1"].ToInt();
                mDataManager.m_nOmronFinsDA2 = iniFile[strSectionProgram]["FinsDA2"].ToInt();
                //
                mDataManager.m_nOmronFinsSNA = iniFile[strSectionProgram]["FinsSNA"].ToInt();
                mDataManager.m_nOmronFinsSA1 = iniFile[strSectionProgram]["FinsSA1"].ToInt();
                mDataManager.m_nOmronFinsSA2 = iniFile[strSectionProgram]["FinsSA2"].ToInt();
                //dataManager.m_strDatabaseIP = iniFile[strSectionProgram]["DatabaseIP"].ToString();
                //pmacData._nMotorNo = iniFile[strSectionProgram]["MotorNo"].ToInt();
            }
            else
            {
                mDataManager.m_strOmronPlcIP = "192.168.250.1";
                mDataManager.m_nOmronFinsPort = 9601;
                mDataManager.m_nOmronFinsTCPUDPSel = 1; //1: TCP, 2:UDP
                mDataManager.m_nOmronFinsAreaBlock = 0x82; //0x82 : DM영역
                //
                mDataManager.m_nOmronFinsDNA = 0;
                mDataManager.m_nOmronFinsDA1 = 01;
                mDataManager.m_nOmronFinsDA2 = 0;
                //
                mDataManager.m_nOmronFinsSNA = 0;
                mDataManager.m_nOmronFinsSA1 = 0xEA;
                mDataManager.m_nOmronFinsSA2 = 0  ;
        
            }

            //for Debug
            Console.WriteLine("Fins IP = {0} / Fins Port = {1}",
                mDataManager.m_strOmronPlcIP, mDataManager.m_nOmronFinsPort);
            Console.WriteLine("Fins Communication Type = {0} / {1}",
               mDataManager.m_nOmronFinsTCPUDPSel, (mDataManager.m_nOmronFinsTCPUDPSel == 0 ? "FINS/TCP":"FINS/UDP"));
            Console.WriteLine("Fins Memory Area Code = 0x{0:X2} / {1}",
              mDataManager.m_nOmronFinsAreaBlock, (mDataManager.m_nOmronFinsAreaBlock == 0x82 ? "DM" : "Another"));

            Console.WriteLine("Fins Header DNA={0}, DA1={1}, DA2={2}, SNA={3}, SA1={4}, SA2={5}",
            mDataManager.m_nOmronFinsDNA,
            mDataManager.m_nOmronFinsDA1,
            mDataManager.m_nOmronFinsDA2,
            //
            mDataManager.m_nOmronFinsSNA,
            mDataManager.m_nOmronFinsSA1,
            mDataManager.m_nOmronFinsSA2);

            // PlcFinsData plcFinsData = new PlcFinsData();
        }//end  public DeviceDataTask()

        public void SatartManualDriveTask()
        {
            Console.WriteLine("DeviceDataTask::SatartManualDriveTask()");

            m_taskManualDrive = new Task(new System.Action(taskManualDriveJog));
            m_bManualDriveTask = true;
            m_taskManualDrive.Start();
        }



        public void AutoDriveGoalTargetDriveTask(int _goal)
        {
            Console.WriteLine("DeviceDataTask::StopManualDriveTask()");

           

            // 마무리로 모두 0으로 한번 전송한다.
            Task _task = new Task(() => taskAutoDriveGoalTarget(_goal));
            _task.Start();
        }

        public void StopTriggerTask()
        {
            Console.WriteLine("DeviceDataTask::StopManualDriveTask()");

           

            // 마무리로 모두 0으로 한번 전송한다.
            Task _task = new Task(() => taskStop_Trigger());
            _task.Start();
        }



        public void StopManualDriveTask()
        {
            Console.WriteLine("DeviceDataTask::StopManualDriveTask()");

            if (m_taskManualDrive != null)
            {
                m_bManualDriveTask = false;
                m_taskManualDrive.Wait();
            }

            // 마무리로 모두 0으로 한번 전송한다.
            Task _task = new Task(() => taskManualDriveJogTerminate());
            _task.Start();
        }

        public void SatartDeviceDataTask()
        {
            Console.WriteLine("DeviceDataTask::SatartDeviceDataTask()");

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
            Console.WriteLine("DeviceDataTask::StoptDeviceDataTask()");
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

        private void taskManualDriveJogTerminate()
        {
            Console.WriteLine("DeviceDataTask::taskManualDriveJogTerminate()");
            UInt32 uTaskIndexCount;

            UInt32 uResult;
            UInt32 uAddress;
            Int32 nSize;
            Byte[] bBuffer4ReadArea;
            UInt32 uLength;

            FinsDevice _finsDevice = mDataManager.GetFinsDevice();
            PlcFinsData _fincData = mDataManager.GetFinsData();

            bool nIsConnected;
           
           
            bBuffer4ReadArea = new Byte[(4 * 2)] ;
            Array.Clear(bBuffer4ReadArea, 0, bBuffer4ReadArea.Length);


            uint _byte_offset;
            uint _count;
            uint _size;
            bool _unsigned;
            byte[] _byteArray;
            Int32 _set_value_s16 = 0;


            nIsConnected = _finsDevice.IsConected();

            if (nIsConnected == false)
                return;


          


            //D60 Jog_X_val         range: 0 ~2000(중앙 1000)
            //D61 Jog_Y_val         range: 0 ~2000(중앙 1000)
            //D62 Jog_Speed_val     range: 0 ~2000(기본값 0)

            _byte_offset = 0;
            try
            {

                //m_JogMoveCtrlCmd.JogModeEnable = false;

                //D60 Jog_X_val   range: 0 ~2000(중앙 1000)
                //D61 Jog_Y_val   range: 0 ~2000(중앙 1000)
                //D62 Jog_Speed_val   range: 0 ~2000(기본값 0)

                // Jog 방향 X , range: 0 ~ 2000
                m_JogMoveCtrlCmd.JogValueDirectionX = Constants._JOYSTIC_XY_DAC_CENTER_VALUE; ;
                _set_value_s16 = m_JogMoveCtrlCmd.JogValueDirectionX;
                _byteArray = BitConverter.GetBytes(_set_value_s16);
                //
                bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                _byte_offset = _byte_offset + 2;


                // Jog 방향 Y , range: 0 ~2000
                m_JogMoveCtrlCmd.JogValueDirectionY = Constants._JOYSTIC_XY_DAC_CENTER_VALUE; ; 
                _set_value_s16 = m_JogMoveCtrlCmd.JogValueDirectionY;
                _byteArray = BitConverter.GetBytes(_set_value_s16);
                //                    
                bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                _byte_offset = _byte_offset + 2;

                //Jog Speed , range : 0 ~ 2000
                //m_JogMoveCtrlCmd.JogValueSpeed = 0;
                _set_value_s16 = m_JogMoveCtrlCmd.JogValueSpeed;
                _byteArray = BitConverter.GetBytes(_set_value_s16);

                //바이트 오더 고려해서 작업할것
                bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                _byte_offset = _byte_offset + 2;

                //바이트 오더 고려해서 작업할것
                //bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                //bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                //bBuffer4ReadArea[_byte_offset + 2] = _byteArray[2];
                //bBuffer4ReadArea[_byte_offset + 3] = _byteArray[3];

            }
            catch (IndexOutOfRangeException ioex)
            {
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }

            uAddress = 60; //D60번지부터
            nSize    = 3;    //3워드
            uResult  = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);


            //D0.01   AMR_Connect AMR 접속 트리거
            //
            //9   D0.08   AMR_Stop 정지 트리거
            //10  D0.09   AMR_JOG_Mode 조그모드 트리거
            //11  D0.10   AMR_Goto_Mode 목적지 이동모드 트리거
            //12  D0.11   AVR_GotoPoint_Mode 좌표 이동모드 트리거

            Thread.Sleep(300);

            try
            {
                _set_value_s16 = 0x0006; // D0.08   AMR_Stop 정지 트리거
                _byteArray = BitConverter.GetBytes(_set_value_s16);
                //
                bBuffer4ReadArea[0] = _byteArray[0];
                bBuffer4ReadArea[1] = _byteArray[1];
            }
            catch (IndexOutOfRangeException ioex)
            {
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }
            uAddress = 0; //D000번지부터
            nSize = 1;    //1워드
            uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);

        }//end private void taskManualDriveJogTerminate()


        
        private void taskAutoDriveGoalTarget(int _goal_number)
        {
            Console.WriteLine("DeviceDataTask::taskAudoDriveGoalTarget( {0} )", _goal_number);
            UInt32 uTaskIndexCount;

            UInt32 uResult;
            UInt32 uAddress;
            Int32 nSize;
            Byte[] bBuffer4ReadArea;
            UInt32 uLength;

            FinsDevice _finsDevice = mDataManager.GetFinsDevice();
            PlcFinsData _fincData = mDataManager.GetFinsData();

            bool nIsConnected;
           


            bBuffer4ReadArea = new Byte[(24 * 2)];
            Array.Clear(bBuffer4ReadArea, 0, bBuffer4ReadArea.Length);


            

            uint _byte_offset;
            uint _count;
            uint _size;
            bool _unsigned;
            byte[] _byteArray;
            Int32 _set_value_s16 = 0;


            nIsConnected = _finsDevice.IsConected();

            if (nIsConnected == false)
                return;

           

            try
            {
                //D0.01   AMR_Connect AMR 접속 트리거
                //D0.02   AMR_Status AMR 상태 트리거(On : 500ms 마다 상태 갱신)
                _set_value_s16 = 0x0006; // D0.08	AMR_Stop	정지 트리거


                _byteArray = BitConverter.GetBytes(_set_value_s16);
                //
                bBuffer4ReadArea[0] = _byteArray[0];
                bBuffer4ReadArea[1] = _byteArray[1];
            }
            catch (IndexOutOfRangeException ioex)
            {
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }
            uAddress = 0; //D000번지부터
            nSize = 1;    //1워드
            uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);



            Thread.Sleep(500);

            _byte_offset = 0;
            string str_GoalTargetString = "";
            if (_goal_number == 1)
            {
                str_GoalTargetString = "a";
            }
            else if (_goal_number == 2)
            {
                str_GoalTargetString = "b";
            }
            else if (_goal_number == 3)
            {
                str_GoalTargetString = "a ";
            }
            else if (_goal_number == 4)
            {
                str_GoalTargetString = "b ";
            }
            else
            {
                str_GoalTargetString = "e";
            }
            try
            {

                _byteArray = Encoding.ASCII.GetBytes(str_GoalTargetString);

                if (_goal_number == 1)
                {
                    //str_GoalTargetString = "a";
                    bBuffer4ReadArea[0] = 0;
                    nSize = 2;
                }
                else if (_goal_number == 2)
                {
                    //str_GoalTargetString = "b";
                    bBuffer4ReadArea[0] = 0;
                    nSize = 2;
                }
                else if (_goal_number == 3)
                {
                    // str_GoalTargetString = "a ";
                    nSize = _byteArray.Length;
                }
                else if (_goal_number == 4)
                {
                    // str_GoalTargetString = "b ";
                    nSize = _byteArray.Length;

                }
                else
                {
                    // str_GoalTargetString = "e";
                    bBuffer4ReadArea[0] = 0;
                    nSize = 2;
                }

                // Array.Copy(byte[] source, int sourceIndex, byte[] destination, int destinationIndex, int length)
                Array.Copy(_byteArray, 0, bBuffer4ReadArea, 0, nSize);// _byteArray.Length);

            }
            catch (IndexOutOfRangeException ioex)
            {
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }

            uAddress = 500; //D60번지부터
            nSize = 25;    //25워드
            uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);

            Thread.Sleep(500);
            //D0.01   AMR_Connect AMR 접속 트리거
            //
            //9   D0.08   AMR_Stop 정지 트리거
            //10  D0.09   AMR_JOG_Mode 조그모드 트리거
            //11  D0.10   AMR_Goto_Mode 목적지 이동모드 트리거
            //12  D0.11   AVR_GotoPoint_Mode 좌표 이동모드 트리거

            try
            {
                //D0.01   AMR_Connect AMR 접속 트리거
                //D0.02   AMR_Status AMR 상태 트리거(On : 500ms 마다 상태 갱신)
                _set_value_s16 = 0x0406; // D0.10	AMR_Goto_Mode	목적지 이동모드 트리거

                _byteArray = BitConverter.GetBytes(_set_value_s16);
                //
                bBuffer4ReadArea[0] = _byteArray[0];
                bBuffer4ReadArea[1] = _byteArray[1];
            }
            catch (IndexOutOfRangeException ioex)
            {
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }
            uAddress = 0; //D000번지부터
            nSize = 1;    //1워드
            uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);


        }//end private void taskAudoDriveGoalTarget(int _goal_number)



        private void taskStop_Trigger()
        {
            Console.WriteLine("DeviceDataTask::taskStop_Trigger( )");
            UInt32 uTaskIndexCount;

            UInt32 uResult;
            UInt32 uAddress;
            Int32 nSize;
            Byte[] bBuffer4ReadArea;
            UInt32 uLength;

            FinsDevice _finsDevice = mDataManager.GetFinsDevice();
            PlcFinsData _fincData = mDataManager.GetFinsData();

            bool nIsConnected;
           


            bBuffer4ReadArea = new Byte[(24 * 2)];
            Array.Clear(bBuffer4ReadArea, 0, bBuffer4ReadArea.Length);




            uint _byte_offset;
            uint _count;
            uint _size;
            bool _unsigned;
            byte[] _byteArray;
            Int32 _set_value_s16 = 0;


            nIsConnected = _finsDevice.IsConected();

            if (nIsConnected == false)
                return;

            

            try
            {
                //D0.01   AMR_Connect AMR 접속 트리거
                //D0.02   AMR_Status AMR 상태 트리거(On : 500ms 마다 상태 갱신)
                _set_value_s16 = 0x0006; // D0.08	AMR_Stop	정지 트리거


                _byteArray = BitConverter.GetBytes(_set_value_s16);
                //
                bBuffer4ReadArea[0] = _byteArray[0];
                bBuffer4ReadArea[1] = _byteArray[1];
            }
            catch (IndexOutOfRangeException ioex)
            {
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }
            uAddress = 0; //D000번지부터
            nSize = 1;    //1워드
            uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);

        }//end private void taskStop_Trigger()

        private void taskManualDriveJog()
        {
            Console.WriteLine("DeviceDataTask::taskManualDriveJog()");
            UInt32 uTaskIndexCount;

            UInt32 uResult;
            UInt32 uAddress;
            Int32 nSize;
            Byte[] bBuffer4ReadArea;
            UInt32 uLength;

            FinsDevice _finsDevice = mDataManager.GetFinsDevice();
            PlcFinsData _fincData = mDataManager.GetFinsData();

            bool nIsConnected;
           

            //D60 Jog_X_val         range: 0 ~2000(중앙 1000)
            //D61 Jog_Y_val         range: 0 ~2000(중앙 1000)
            //D62 Jog_Speed_val     range: 0 ~2000(기본값 0)

            uAddress = 60; //D0060번지부터
            nSize       = 3;    //3워드
            bBuffer4ReadArea = new Byte[(nSize * 2)];
            uint _byte_offset;
            uint _count;
            uint _size;
            bool _unsigned;
            byte[] _byteArray;
            Int32 _set_value_s16 = 0;

            nIsConnected = _finsDevice.IsConected();

            if (nIsConnected == false)
                return;

          

            //D0.01   AMR_Connect AMR 접속 트리거
            //
            //9   D0.08   AMR_Stop 정지 트리거
            //10  D0.09   AMR_JOG_Mode 조그모드 트리거
            //11  D0.10   AMR_Goto_Mode 목적지 이동모드 트리거
            //12  D0.11   AVR_GotoPoint_Mode 좌표 이동모드 트리거

            try
            {
                _set_value_s16 = 0x0206; // D0.09   AMR_JOG_Mode 조그모드 트리거
                _byteArray = BitConverter.GetBytes(_set_value_s16);
                //
                bBuffer4ReadArea[0] = _byteArray[0];
                bBuffer4ReadArea[1] = _byteArray[1];
            }
            catch (IndexOutOfRangeException ioex)
            {
                Console.WriteLine(ioex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine(exc.Message);
            }
            uAddress = 0; //D000번지부터
            nSize = 1;    //1워드
            uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);

            Thread.Sleep(100);

            while (m_bManualDriveTask)
            {
                Console.WriteLine("DeviceDataTask::taskManualDriveJog() :: while (m_bManualDriveTask)");

                nIsConnected = _finsDevice.IsConected();

                if (nIsConnected == false)
                    continue;

                //Console.WriteLine("DeviceDataTask::taskManualDriveJog() m_bManualDriveTask == ture");
                _byte_offset = 0;
                try
                {

                    //m_JogMoveCtrlCmd.JogModeEnable = false;

                    //D60 Jog_X_val   range: 0 ~2000(중앙 1000)
                    //D61 Jog_Y_val   range: 0 ~2000(중앙 1000)
                    //D62 Jog_Speed_val   range: 0 ~2000(기본값 0)

                    // Jog 방향 X , range: 0 ~ 2000
                    _set_value_s16 = m_JogMoveCtrlCmd.JogValueDirectionX;
                    _byteArray = BitConverter.GetBytes(_set_value_s16);
                    //
                    bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                    bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                    _byte_offset = _byte_offset + 2;


                    // Jog 방향 Y , range: 0 ~2000
                    _set_value_s16 = m_JogMoveCtrlCmd.JogValueDirectionY;
                    _byteArray = BitConverter.GetBytes(_set_value_s16);
                    //                    
                    bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                    bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                    _byte_offset = _byte_offset + 2;

                    //Jog Speed , range : 0 ~ 2000
                    _set_value_s16 = m_JogMoveCtrlCmd.JogValueSpeed;
                    _byteArray = BitConverter.GetBytes(_set_value_s16);

                    //바이트 오더 고려해서 작업할것
                    bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                    bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                    _byte_offset = _byte_offset + 2;

                    //바이트 오더 고려해서 작업할것
                    //bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                    //bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                    //bBuffer4ReadArea[_byte_offset + 2] = _byteArray[2];
                    //bBuffer4ReadArea[_byte_offset + 3] = _byteArray[3];

                }
                catch (IndexOutOfRangeException ioex)
                {
                    Console.WriteLine(ioex.Message);
                }
                catch (Exception exc)
                {
                    Console.WriteLine(exc.Message);
                }

                uAddress = 60; //D60번지부터
                nSize = 3;    //1워드
                uResult = _finsDevice.SetFinsMem(uAddress, nSize, bBuffer4ReadArea, out uLength);
                Thread.Sleep(300);

        }//end  while (m_bManualDriveTask)

    }

        private void taskFinsDataWriteLoop()
        {
            Console.WriteLine("DeviceDataTask::taskFinsDataWriteLoop()");

            UInt32 uResult;
            UInt32 uAddress;
            Int32 nSize;
            Byte[] bBuffer4ReadArea;
            UInt32 uLength;

            bool nIsConnected;
        

           

            FinsDevice _finsDevice = mDataManager.GetFinsDevice();
            PlcFinsData _fincData = mDataManager.GetFinsData();

            nIsConnected = _finsDevice.IsConected();

            if (nIsConnected == false)
                return;

            reg_data[] _reg_wrtie_data = _fincData.GetWriteData();
            //pmacDevice.Open("192.168.0.200", (UInt32)DTK_MODE_TYPE.DM_GPASCII);
            //StartPmacConnectTask();

            uint _byte_offset;
            uint _count;
            uint _size;
            bool? _unsigned;

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
                        if (_unsigned == true)
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
                        bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                        bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];

                    }//end if (_size == 2)
                    else if (_size == 4)
                    {
                        if (_unsigned == true)
                        {
                            UInt32 _set_value = 0;
                            try
                            {
                                _set_value = Convert.ToUInt32(_reg_wrtie_data[i].set_value);
                            }
                            catch (Exception ex)
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
                        bBuffer4ReadArea[_byte_offset + 0] = _byteArray[0];
                        bBuffer4ReadArea[_byte_offset + 1] = _byteArray[1];
                        bBuffer4ReadArea[_byte_offset + 2] = _byteArray[2];
                        bBuffer4ReadArea[_byte_offset + 3] = _byteArray[3];
                    }//end else if (_size == 4)
                    else
                    {
                        //크기가 2,4 가 아닌 크기
                        
                        byte[] _full_size_byte = new byte[_size];
                        Array.Clear(_full_size_byte, 0, (int)_size);

                        UInt32 _set_value = 0;
                        try
                        {
                            byte[] StrByte;
                            StrByte = Encoding.UTF8.GetBytes(_reg_wrtie_data[i].set_value);
                            // Array.Copy(byte[] source, int sourceIndex, byte[] destination, int destinationIndex, int length)
                            Array.Copy(StrByte, 0, _full_size_byte, 0, StrByte.Length);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine(ex.Message);
                            //StrByte = new byte[_size];
                            //Array.Clear(StrByte, 0,(int) _size);
                        }
                        finally
                        {

                        }
                        //_byteArray = BitConverter.GetBytes(_set_value);
                        // Array.Copy(byte[] source, int sourceIndex, byte[] destination, int destinationIndex, int length)
                        Array.Copy(_full_size_byte, 0, bBuffer4ReadArea, _byte_offset, _size);
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
            bool  nIsConnected;
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
                nIsConnected = _finsDevice.IsConected();

                if (nIsConnected == false)
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
