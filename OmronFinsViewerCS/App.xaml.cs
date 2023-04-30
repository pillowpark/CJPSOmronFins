
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

//


namespace OmronFinsViewerCS
{
    /// <summary>
    /// App.xaml에 대한 상호 작용 논리
    /// </summary>
    /// 

    namespace OmronFinsViewerCS
    {
       
    }
    public partial class App : Application
    {

        [STAThread]
        public static void Main()
        {
            var application = new App();
            application.InitializeComponent();
            application.Run();
        }



        private static App m_theApp = null;
        private MainWindow m_MainWindow = null;
        private DataManager m_DataManager = null;
        private DeviceDataTask m_DeviceDataTask = null;
        private PlcFinsData m_PlcFinsData = null;

        public static App GetApp()
        {
            return m_theApp;
        }

        public DataManager GetDataManager()
        {
            return m_DataManager;
        }

        public DeviceDataTask GetDeviceDataTask()
        {
            return m_DeviceDataTask;
        }

        public MainWindow GetMainWindow()
        {
            return m_MainWindow;
        }

        public void SetMainWindow(MainWindow mainWindow)
        {
            m_MainWindow = mainWindow;
        }

        public void Initailized()
        {
            m_DataManager = new DataManager();
            m_DeviceDataTask = new DeviceDataTask();
        }


        public App()
        {
            m_theApp = this;

        }
    }
}
