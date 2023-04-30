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

namespace OmronFinsViewerCS
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {

        ReadPage mReadPage;
        WritePage mWritePage;
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
            
            UInt32 uIPAddress = 0xC0A800DD;//192.168.0.221
                                    //uIPAddress = 0xC0A8FA01;//192.168.250.1
                                    //uIPAddress = 0xC0A8FB01;//192.168.251.1
            UInt32 uPortNum = 9600;
            mFinsDevice.Open("192.168.0.221", uPortNum);
           // mFinsDevice.Open("10.0.0.10", uPortNum);

            mFinsDevice.Connect();
            //OFinsOpen(UINT32 uDeviceType, UINT32 uIPAddress, UINT32 uPortNum);

            //
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
