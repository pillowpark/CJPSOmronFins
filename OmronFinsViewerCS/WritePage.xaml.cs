using OmronFinsViewerCS.Device;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
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
using static OmronFinsViewerCS.PlcFinsData;
using static OmronFinsViewerCS.ReadPage;

namespace OmronFinsViewerCS
{
    /// <summary>
    /// WritePage.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class WritePage : Page
    {

        Page m_ThisPage = null;

        private DataManager m_dataManager = null;
        private PlcFinsData m_plcFinsData = null;
        private FinsDevice m_finsDevice = null;
        public WritePage()
        {
            InitializeComponent();
            //
            m_ThisPage = this;
            //
            m_dataManager = App.GetApp().GetDataManager();
            m_finsDevice = m_dataManager.GetFinsDevice();
            m_plcFinsData = m_dataManager.GetFinsData();
            //
            m_plcFinsData.OnWriteAreaDataChanged += new PlcFinsData.OnWriteAreaDataChangedDelegate(OnWriteAreaValueChanged);
        }

        public ObservableCollection<Data> m_WriteDataCollection = null;
        int m_prev_select_index;
        string m_prev_select_cell_string;
        //private List<Data> m_WriteList;
        public void InitDataUpdate()
        {
            uint _data_count;
            PlcFinsData _plcFinsData = App.GetApp().GetDataManager().GetFinsData();
            reg_data[] _reg_wrtie_data = _plcFinsData.GetWriteData();

            _data_count = _plcFinsData.WriteVariableCount;

            //m_WriteDataCollection = new List<Data>();
            m_WriteDataCollection = new ObservableCollection<Data>();
           
           


            for (int i = 0; i < _data_count; ++i)
            {
                m_WriteDataCollection.Add(new Data
                {
                    index = _reg_wrtie_data[i].index,
                    size = _reg_wrtie_data[i].size,
                    name = _reg_wrtie_data[i].name,
                    value = _reg_wrtie_data[i].curr_value,
                    write_value = _reg_wrtie_data[i].set_value,
                    description = _reg_wrtie_data[i].description,
                    remark = ""
                }) ;
            }
            //list.Add(new Data { name = "이지원", id = "210651", major = "컴퓨터공학", grade = 1, etc = "" });

            FinsWriteDataList.ItemsSource = m_WriteDataCollection;
            
            FinsWriteDataList.SelectedCellsChanged += OnSetValueSelectCellChanged;
            FinsWriteDataList.CellEditEnding += OnSetValueCellEditEnding;
            m_WriteDataCollection.CollectionChanged += this.OnSetValueCollectionChanged;
            FinsWriteDataList.LostFocus += OnDataGridCellLostFocus;

        }// public void InitDataUpdate()


        private void OnWriteAreaValueChanged()
        {
            int temp_int1, temp_int2;
            uint _data_count;

            PlcFinsData _plcFinsData = App.GetApp().GetDataManager().GetFinsData();
            reg_data[] _reg_write_data = _plcFinsData.GetWriteData();

            _data_count = _plcFinsData.WriteVariableCount;

            //List<Data> read_data_list = FinsReadDataList.DataContext as List<Data>;
            //temp_int1 = m_ReadDataCollection[1].value;
            //temp_int2 = m_ReadDataCollection[2].value;
            //m_ReadDataCollection[idx].value = temp_int1 + 3;
            //m_ReadDataCollection[idx].value = temp_int2 + 5;

            for (int idx = 0; idx < _data_count; ++idx)
            {
                m_WriteDataCollection[idx].value = _reg_write_data[idx].curr_value;
            }
            Console.WriteLine("ReadPage::OnWriteAreaValueChanged() call");

          
        }


        void OnDataGridCellLostFocus(object sender, RoutedEventArgs e)
        {
            int _lostIndex = 0;
            bool _unsign;
            PlcFinsData _plcFinsData = App.GetApp().GetDataManager().GetFinsData();
            reg_data[] _reg_wrtie_data = _plcFinsData.GetWriteData();

            _lostIndex = m_prev_select_index;
            Console.WriteLine("WritePage::OnDataGridCellLostFocus :: LostIndex = {0}", _lostIndex);

            _unsign = _reg_wrtie_data[_lostIndex].unsign;

            if (_unsign)
            {
                Regex regex = new Regex(@"[^ -]\d");
                string _value_string = m_WriteDataCollection[_lostIndex].write_value;
                Boolean ismatch = regex.IsMatch(_value_string);
                if (!ismatch)
                {
                    MessageBox.Show("양수 숫자만 입력해주세요");
                    m_WriteDataCollection[_lostIndex].write_value = m_prev_select_cell_string;
                }
                else
                {
                    _reg_wrtie_data[_lostIndex].set_value = _value_string;
                }
            }
            else
            {
                Regex regex = new Regex(@"[-0-9]");
                string _value_string = m_WriteDataCollection[_lostIndex].write_value;
                Boolean ismatch = regex.IsMatch(_value_string);
                if (!ismatch)
                {
                    MessageBox.Show("-(음수기호)와 숫자만 입력해주세요");
                    m_WriteDataCollection[_lostIndex].write_value = m_prev_select_cell_string;
                }
                else
                {
                    _reg_wrtie_data[_lostIndex].set_value = _value_string;
                }

            }
        }
        void OnSetValueCellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            int _changed_row =0 ;

            //Get Row Number
            DataGridRow _row = e.Row;

            _changed_row = _row.GetIndex();


            Console.WriteLine("WritePage::OnSetValueCellEditEnding row = {0}", _changed_row);
        }
       
        void OnSetValueCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            int _changed_row;
            //Get the sender observable collection
            ObservableCollection<Data> obsSender = sender as ObservableCollection<Data>;
            _changed_row = e.NewStartingIndex;

            Console.WriteLine("WritePage::OnSetValueChanged row = {0}", _changed_row);
        }

        private void SendButton_Click(object sender, RoutedEventArgs e)
        {
            PlcFinsData _plcFinsData = App.GetApp().GetDataManager().GetFinsData();
            reg_data[] _reg_wrtie_data = _plcFinsData.GetWriteData();

            Console.WriteLine("WritePage::SendButton Clicked!!");

            int _index;
            _index = m_prev_select_index;
            var sel_string = m_WriteDataCollection[_index].write_value;
            

            Console.WriteLine("WritePage::row[{0}] = {1}", _index,  sel_string);
            Console.WriteLine("WritePage::data = {0} ",  _reg_wrtie_data[_index].set_value);


        }

        //----------------------------
        void OnSetValueSelectCellChanged(object sender, SelectedCellsChangedEventArgs e)
        {
            int _changed_row = 0;

            //Get the newly selected cells
            IList<DataGridCellInfo> selectedcells = e.AddedCells;
            var currentRowIndex = FinsWriteDataList.Items.IndexOf(FinsWriteDataList.CurrentItem);

            m_prev_select_index = currentRowIndex;
            m_prev_select_cell_string = m_WriteDataCollection[m_prev_select_index].write_value;

            Console.WriteLine("WritePage::OnSetValueSelectCellChanged row = {0}", currentRowIndex);
        }

        private void TextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            
           
            Console.WriteLine("WritePage::TextBox_LostFocus !");
        }

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            Console.WriteLine("WritePage::TextBox_TextChanged");
            TextBox _tb = (TextBox)e.Source;
           
            Console.WriteLine("WritePage::TextBox_TextChanged {0} ", _tb.Text);
        }

        private void TextBox_KeyDown(object sender, KeyEventArgs e)
        {

        }
    }
}
