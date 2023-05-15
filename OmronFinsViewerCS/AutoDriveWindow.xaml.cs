using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace OmronFinsViewerCS
{
    /// <summary>
    /// AutoDriveWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class AutoDriveWindow : Window
    {
        private DataManager mDataManager;// = App.GetApp().GetDataManager();
     
        private DeviceDataTask mDeviceDataTask = null;

        public AutoDriveWindow()
        {
            InitializeComponent();
            //
            //
            mDataManager = App.GetApp().GetDataManager();
            mDeviceDataTask = App.GetApp().GetDeviceDataTask();
            //
        }

        private void x_ButtonAutoDriveMode_Click(object sender, RoutedEventArgs e)
        {

        }

        private void x_ButtonStopMode_Click(object sender, RoutedEventArgs e)
        {
            mDeviceDataTask.StopTriggerTask();
        }

        private void x_ButtonCoordXYMode_Click(object sender, RoutedEventArgs e)
        {

        }

        private void x_ButtonGoalTargetMode_Click(object sender, RoutedEventArgs e)
        {

        }

        private void x_ButtonGoal_A_Click(object sender, RoutedEventArgs e)
        {
            mDeviceDataTask.AutoDriveGoalTargetDriveTask(1);
        }

        private void x_ButtonGoal_B_Click(object sender, RoutedEventArgs e)
        {
            mDeviceDataTask.AutoDriveGoalTargetDriveTask(2);
        }

        private void x_ButtonGoal_C_Click(object sender, RoutedEventArgs e)
        {
            mDeviceDataTask.AutoDriveGoalTargetDriveTask(3);
        }

        private void x_ButtonGoal_D_Click(object sender, RoutedEventArgs e)
        {
            mDeviceDataTask.AutoDriveGoalTargetDriveTask(4);
        }

    }
}
