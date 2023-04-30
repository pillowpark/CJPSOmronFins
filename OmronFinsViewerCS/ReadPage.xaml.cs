using OmronFinsViewerCS.Device;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using static OmronFinsViewerCS.PlcFinsData;

namespace OmronFinsViewerCS
{
    /// <summary>
    /// ReadPage.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class ReadPage : Page
    {

        public ObservableCollection<Data> m_ReadDataCollection = null;
        Page m_ThisPage = null;

        private DataManager m_dataManager = null;
        private PlcFinsData m_plcFinsData = null;
        private FinsDevice m_finsDevice = null;

        public ReadPage()
        {
            InitializeComponent();
            //


            //
            m_ThisPage = this;
            m_ReadDataCollection = new ObservableCollection<Data>();
            //
            m_dataManager = App.GetApp().GetDataManager();
            m_finsDevice = m_dataManager.GetFinsDevice();
            m_plcFinsData = m_dataManager.GetFinsData();
            //
            m_plcFinsData.OnReadAreaDataChanged += new PlcFinsData.OnReadAreaDataChangedDelegate(OnReadValueChanged);
        }

        //값이 업데이트 되었을때 UI를 변경하려면 : INotifyPropertyChanged 인터페이스를 상속해서 처리해야함.
        public class Data : INotifyPropertyChanged
        {
            public event PropertyChangedEventHandler PropertyChanged;
            public int index { get; set; }
            public uint size { get; set; }

            private string reg_value;
            public string value
            {
                get { return reg_value; }
                set
                {
                    if (reg_value != value)
                    {
                        reg_value = value;
                        OnPropertyChanged("value");
                    }
                }
            }

            private string set_valeu;
            public string write_value 
            { 
                get { return set_valeu; }
                set
                {
                    if (set_valeu != value)
                    {
                        set_valeu = value;
                        OnPropertyChanged("write_value");
                    }
                } 
            }
            public string name { get; set; }
            public string description { get; set; }
            public string remark { get; set; }

            //--
            protected void OnPropertyChanged(string name)
            {
                PropertyChangedEventHandler handler = PropertyChanged;
                if (handler != null)
                {
                    handler(this, new PropertyChangedEventArgs(name));
                }
            }
        }

       
        public void InitDataUpdate()
        {
            uint _data_count;
            PlcFinsData _plcFinsData =  App.GetApp().GetDataManager().GetFinsData();
            reg_data[] _reg_read_data = _plcFinsData.GetReadData();

            _data_count = _plcFinsData.ReadVariableCount;

           
            for(int i=0; i< _data_count; ++i)
            {
                m_ReadDataCollection.Add(new Data { 
                    index = _reg_read_data[i].index, 
                    size = _reg_read_data[i].size,
                    name = _reg_read_data[i].name,
                    value = Convert.ToString(_reg_read_data[i].curr_value),
                    description = _reg_read_data[i].description,
                    remark = "" });
            }
            //list.Add(new Data { name = "이지원", id = "210651", major = "컴퓨터공학", grade = 1, etc = "" });

            FinsReadDataList.ItemsSource = m_ReadDataCollection;
            FinsReadDataList.DataContext = m_ReadDataCollection;

        }

        private void OnReadValueChanged()
        {
            int temp_int1, temp_int2;
            uint _data_count;

            PlcFinsData _plcFinsData = App.GetApp().GetDataManager().GetFinsData();
            reg_data[] _reg_read_data = _plcFinsData.GetReadData();

            _data_count = _plcFinsData.ReadVariableCount;

            //List<Data> read_data_list = FinsReadDataList.DataContext as List<Data>;
            //temp_int1 = m_ReadDataCollection[1].value;
            //temp_int2 = m_ReadDataCollection[2].value;
            //m_ReadDataCollection[idx].value = temp_int1 + 3;
            //m_ReadDataCollection[idx].value = temp_int2 + 5;

            for (int idx = 0; idx < _data_count; ++idx)
            {
                m_ReadDataCollection[idx].value = _reg_read_data[idx].curr_value;
            }
            Console.WriteLine("ReadPage::OnReadValueChanged() call");

            //FinsReadDataList.ItemsSource = m_ReadDataCollection;
            //FinsReadDataList.DataContext = m_ReadDataCollection;

            //FinsReadDataList.ItemsSource = m_ReadList;
            //m_ReadList
            //this.m_ThisPage.UpdateLayout();
            //FinsReadDataList.UpdateLayout();
            //FinsReadDataList.UpdateDefaultStyle();
            //FinsReadDataList.ItemsSource = m_ReadList;
            // Dispatcher.BeginInvoke(new Action(() => { }));
            //DoEvents();
        }

        //public static void DoEvents()

        //{

        //    Application.Current.Dispatcher.Invoke(DispatcherPriority.Background,

        //                                          new Action(delegate { }));

        //}

        private void Grid_Loaddded(object sender, RoutedEventArgs e)
        {
            //List<Data> list = new List<Data>();
            //list.Add(new Data { name = "이지원", id = "210651", major = "컴퓨터공학", grade = 1, etc = "" });
            //list.Add(new Data { name = "김현호", id = "210184", major = "컴퓨터공학", grade = 1, etc = "" });
            //list.Add(new Data { name = "강희진", id = "210017", major = "컴퓨터공학", grade = 1, etc = "" });
            //list.Add(new Data { name = "박서준", id = "210439", major = "컴퓨터공학", grade = 1, etc = "" });
            //list.Add(new Data { name = "강나연", id = "210005", major = "컴퓨터공학", grade = 1, etc = "" });
            //StudentList.ItemsSource = list;
        }
    }
}