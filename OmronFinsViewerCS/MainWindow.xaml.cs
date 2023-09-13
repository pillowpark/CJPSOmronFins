using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
//
using OmronFinsViewerCS;
//
using Excel = Microsoft.Office.Interop.Excel;
using System.IO;
using System.Windows.Markup;
using System.Net;
using System.Diagnostics;

using OmronFinsViewerCS.Device;
using static OmronFinsViewerCS.Device.FinsDevice;

namespace OmronFinsViewerCS
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {

        ReadPage mReadPage;
        WritePage mWritePage;
        LoadingPage mLoadingPage;
        //
        private DataManager mDataManager;// = App.GetApp().GetDataManager();
        private FinsDevice mFinsDevice;// = dataManager.GetPmacDevice();
        private DeviceDataTask mDeviceDataTask = null;
        //
        UInt32 muDeviceID;
        public MainWindow()
        {
            InitializeComponent();
            //
            //
            App.GetApp().SetMainWindow(this);
            App.GetApp().Initailized();
            //

            //
            mReadPage = new ReadPage();
            CenterLeftFrame.Content = mReadPage;
            //
            mWritePage = new WritePage();
            CenterRightFrame.Content = mWritePage;

            //
            mLoadingPage = new LoadingPage();
            CenterLoadingFrame.Content = mLoadingPage;
            mLoadingPage.Visibility = Visibility.Hidden;

              mDataManager = App.GetApp().GetDataManager();
            mFinsDevice = mDataManager.GetFinsDevice();
            mDeviceDataTask = App.GetApp().GetDeviceDataTask();
        }

        private void x_ButtonSetting_Click(object sender, RoutedEventArgs e)
        {
            App.GetApp().GetDataManager().PlcFinsDataCreate();
            //
            mReadPage.InitDataUpdate();
            mWritePage.InitDataUpdate();

        }

        public ReadPage GetReadPage()
        {
            return mReadPage;
        }

        public WritePage GetWritePage()
        {
            return mWritePage;
        }

        private void Grid_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void x_ButtonMode01_Click(object sender, RoutedEventArgs e)
        {
            //mReadPage.ReadValueChanged();
            //mReadPage.Dispatcher.Invoke(new Action(() => { }));
            //this.UpdateLayout();
        }

        private void x_ButtonConnect_Click(object sender, RoutedEventArgs e)
        {

            //UInt32 uIPAddress = 0xC0A800DD;//192.168.0.221
            //uIPAddress = 0xC0A8FA01;//192.168.250.1
            //uIPAddress = 0xC0A8FB01;//192.168.251.1
            String strIPAddr;
            UInt32 uPortNum = 9600;
            Int32 nTCPUDPSel;
            Int32 nFinsBlockArea;
            FinsHeaderT _hotlink_header = new FinsHeaderT(
                    mDataManager.m_nOmronFinsDNA, mDataManager.m_nOmronFinsDA1, mDataManager.m_nOmronFinsDA2,
                    mDataManager.m_nOmronFinsSNA, mDataManager.m_nOmronFinsSA1, mDataManager.m_nOmronFinsSA2);

            strIPAddr   = mDataManager.m_strOmronPlcIP;// = "192.168.0.221"
            uPortNum    = (uint)mDataManager.m_nOmronFinsPort;//
            nTCPUDPSel  = mDataManager.m_nOmronFinsTCPUDPSel;// = 1; //1: TCP, 2:UDP
            nFinsBlockArea = mDataManager.m_nOmronFinsAreaBlock;//= 0x82; //0x82 : DM영역
            
            //1: TCP, 2:UDP
            if (nTCPUDPSel == 1)
            {
                mFinsDevice.Open(strIPAddr, uPortNum, (UInt32)CJPSOF_DEVICE_TYPE.COFD_FINS_TCP);
                // mFinsDevice.Open("10.0.0.10", uPortNum);
                mFinsDevice.Connect();
            } 
            else
            {
                mFinsDevice.Open(strIPAddr, uPortNum, (UInt32)CJPSOF_DEVICE_TYPE.COFD_FINS_UDP);
                // mFinsDevice.Open("10.0.0.10", uPortNum);
                
                mFinsDevice.Connect();
                mFinsDevice.SetFinsHeader(_hotlink_header, nFinsBlockArea);
            //OFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum);

            }
        }

        private void x_ButtonDisconnect_Click(object sender, RoutedEventArgs e)
        {
            mFinsDevice.Disconnect();
            //
            mFinsDevice.Close();
        }

        private void x_ButtonReadStart_Click(object sender, RoutedEventArgs e)
        {
            mDeviceDataTask.SatartDeviceDataTask();
            //
            //mDeviceDataTask.StoptDeviceDataTask();
        }

        private void x_ButtonWriteOne_Click(object sender, RoutedEventArgs e)
        {
            mDeviceDataTask.OneShotWriteArea();
        }

        //Event Handler
        private void OnFinsPlcConnect()
        {
            DataManager dataManager = App.GetApp().GetDataManager();
            //PmacDevice pmacDevice = dataManager.GetPmacDevice();
            //PmacData pmacData = dataManager.GetPmacData();
            //UInt32 uResult;
            //Int32 nIsConnected;

            //uResult = pmacDevice.IsConnected(out nIsConnected);
            //if (nIsConnected == 1)
            //{
            //    x_TextMaxSamples.Text = pmacData.sGather.nMaxSamples.ToString();
            //    x_TextPeriod.Text = pmacData.sGather.nPeriod.ToString();

            //    x_TextBoxServoKp.Text = pmacData.sMotor.dServoKp.ToString("F2");
            //    x_TextBoxServoKi.Text = pmacData.sMotor.dServoKi.ToString("F6");
            //    x_TextBoxServoKvfb.Text = pmacData.sMotor.dServoKvfb.ToString("F2");
            //    x_TextBoxServoKvff.Text = pmacData.sMotor.dServoKvff.ToString("F2");
            //}

            //x_TextBlockDevicePmac.Foreground = new SolidColorBrush(nIsConnected == 1 ? Color.FromArgb(255, 0x0e, 0x4b, 0xce) : Color.FromArgb(255, 0xce, 0x44, 0x0e));
            //x_FontIconDevicePmac.Foreground = new SolidColorBrush(nIsConnected == 1 ? Color.FromArgb(255, 0x0e, 0x4b, 0xce) : Color.FromArgb(255, 0xce, 0x44, 0x0e));
            //x_FontIconDevicePmac.Text = (nIsConnected == 1 ? "\uE8CE" : "\uE8CD");
        }//end private void OnFinsPlcConnect()
    }
}
