
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
    /// JoyStickWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class JoyStickWindow : Window
    {

        private DataManager mDataManager;// = App.GetApp().GetDataManager();
        private CJogMoveData m_JogMoveCtrlCmd = null;
        private DeviceDataTask mDeviceDataTask = null;

        int _release_count = 0;
        int _capture_count = 0;
        public JoyStickWindow()
        {
            InitializeComponent();
            //
            mDataManager  = App.GetApp().GetDataManager();
            m_JogMoveCtrlCmd = mDataManager.GetJogCmdData();
            mDeviceDataTask = App.GetApp().GetDeviceDataTask();
            //
            _release_count = 0;
            _capture_count = 0;


            onScreenJoystick.Moved += VirtualJoystick_Moved;

            onScreenJoystick.Released += VirtualJoystick_Released;
            onScreenJoystick.Captured += VirtualJoystick_Captured;
            //virtualJoystick.ResetKnobAfterRelease = false;
        }

        private void VirtualJoystick_Moved(object sender, WpfControlLibrary.VirtualJoystickEventArgs args)
        {
            joystickInfoLabel.Text = $"Joystick Distance:{args.Distance}, Joystick Angle:{(int)args.Angle},R{_release_count},C{_capture_count}";

            double _x_axis, _y_axis;
            double _radius;
            double _degree;
            double _angle;
            //
            _radius = args.Distance;
            _degree = args.Angle;
            _angle  = Math.PI * _degree / 180.0; //degree to radian
            //            
             _y_axis = Math.Cos(_angle) * _radius;
            _x_axis = Math.Sin(_angle)* _radius;
            Console.WriteLine("VirtualJoyStick:: X = {0} , Y = {1}",(int) _x_axis, (int)_y_axis);

            m_JogMoveCtrlCmd.JogValueDirectionY = (int)_y_axis;
            m_JogMoveCtrlCmd.JogValueDirectionX = (int)_x_axis;
        }

        private void VirtualJoystick_Released(object sender)
        {
            _release_count++;
            joystickInfoLabel_Release_Capture.Text = $"Joystick Release:{_release_count}, Capture:{_capture_count}";

            m_JogMoveCtrlCmd.JogValueDirectionY = 0;
            m_JogMoveCtrlCmd.JogValueDirectionX = 0;
        }

        private void VirtualJoystick_Captured(object sender)
        {
            _capture_count++;
            joystickInfoLabel_Release_Capture.Text = $"Joystick Capture:{_capture_count}, Release:{_release_count}";

            m_JogMoveCtrlCmd.JogValueDirectionY = 0;
            m_JogMoveCtrlCmd.JogValueDirectionX = 0;
        }

        private void x_CheckBox_JogEnable_CheckedChanged(object sender, RoutedEventArgs e)
        {
            double _dSpeedValue;
            CheckBox _checkBox  = (CheckBox)sender;
            var _isChecked = _checkBox.IsChecked;
            //
            Console.WriteLine("VirtualJoyStick::x_CheckBox_JogEnable = {0}", _isChecked==true? " = Checked ":" = UnChecked ");

            if (_isChecked == false)
            {
                m_JogMoveCtrlCmd.JogModeEnable = false;
                m_JogMoveCtrlCmd.JogValueDirectionY = 0;
                m_JogMoveCtrlCmd.JogValueDirectionX = 0;
                m_JogMoveCtrlCmd.JogValueSpeed = 0;

                mDeviceDataTask.StopManualDriveTask();
                //
                //쓰레드 끄고, 마지막 한번 더 데이터를 보내야 될듯
               
            }
            else
            {
                m_JogMoveCtrlCmd.JogModeEnable = true;
                _dSpeedValue = x_Slider_SpeedlValue.Value;
                m_JogMoveCtrlCmd.JogValueSpeed = (int)_dSpeedValue;
                //

                m_JogMoveCtrlCmd.JogValueDirectionY = 0;
                m_JogMoveCtrlCmd.JogValueDirectionX = 0;
                //
                mDeviceDataTask.SatartManualDriveTask();
            }
        }
    }
}
