using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

//
using Excel = Microsoft.Office.Interop.Excel;
using System.IO;
using System.Windows.Markup;
using System.Diagnostics.Eventing.Reader;
using System.Data.Common;
using Microsoft.Office.Interop.Excel;
using System.Reflection;
using System.Windows;
using System.Security.RightsManagement;
using System.Data.SqlTypes;

namespace OmronFinsViewerCS
{
    public class PlcFinsData
    {
        //PLC에서 읽어왔을때 변경이 됬는지 안됬는지를 확인하기 위한 변수
        //Local Data 라는 의미

        //읽기 영역용
        private LData _CurrReadLData;
        private LData _PrevReadLData;

        //쓰기 영역용
        private LData _CurrWriteLData;
        private LData _PrevWriteLData;

        //이벤트, 델리게이트 선언
        public delegate void OnReadAreaDataChangedDelegate();
        public event OnReadAreaDataChangedDelegate OnReadAreaDataChanged;
        //
        public delegate void OnWriteAreaDataChangedDelegate();
        public event OnWriteAreaDataChangedDelegate OnWriteAreaDataChanged;
        //
        internal class LData
        {
            //int[] numbers = new int[5];
            public uint[] size;
            public uint[] value;
            public string[] str_byte_value;
            //public bool[] unsign;
            private uint count;
            private reg_data[] preg_data;



            public LData(uint _count, ref reg_data[] _reg_data) 
            {
                count = _count;
                size = new uint[count];
                value = new uint[count];
                str_byte_value = new string[count];
                //
                preg_data = _reg_data;
              
                //unsign = new bool[count];
            }

            public uint Copy_If_Diff(LData para)
            {
                uint _diff_count = 0;
                bool _unsigned;
                uint _var_size;
                bool _b_diff_str;
                

                for (int i = 0; i < count; ++i)
                {
                    if (para.str_byte_value[i] == null)
                    {
                        _b_diff_str = false;
                    }
                    else
                    {
                        _b_diff_str = this.str_byte_value[i].Equals(para.str_byte_value[i]);
                    }
                    if ((this.value[i] != para.value[i]) || !_b_diff_str)
                    {
                        para.value[i] = this.value[i];
                        para.str_byte_value[i] = this.str_byte_value[i];
                        _diff_count++;
                        //
                        _var_size = preg_data[i].size;
                        _unsigned = preg_data[i].unsign;

                        try
                        {

                            if (_var_size == 4)
                            {
                                if (_unsigned)
                                {
                                    preg_data[i].curr_value = Convert.ToString(this.value[i]);
                                }
                                else
                                {
                                    Int32 uint32_temp = (Int32)(this.value[i]);
                                    preg_data[i].curr_value = Convert.ToString(Convert.ToInt32(uint32_temp));
                                }
                            }//end if (_var_size == 4)
                            else if (_var_size == 2)
                            {
                                if (_unsigned)
                                {
                                    UInt16 uint16_temp = (UInt16)(this.value[i]);
                                    preg_data[i].curr_value = Convert.ToString(Convert.ToUInt16(uint16_temp));
                                }
                                else
                                {
                                    Int16 int16_temp = (Int16)(this.value[i]);
                                    preg_data[i].curr_value = Convert.ToString(Convert.ToInt16(int16_temp));
                                }
                            }//end else if (_var_size == 2)
                            else
                            {
                                //size가 2,4 바이트가 아닌 길이
                                preg_data[i].curr_value = this.str_byte_value[i];


                            }
                            // end //size가 2,4 바이트가 아닌 길이

                        }//end try
                        catch (OverflowException oex)
                        { 
                            Console.WriteLine(oex.Message);
                        }
                        catch (Exception exc)
                        {
                            Console.WriteLine(exc.Message);
                        }
                    }
                }

                return _diff_count;
            }

            public void Pasing(byte[] _raw)
            {
                int _offset;
                uint _size;
                //bool _unsigned;

                _offset = 0;

                for (int i = 0; i < count; ++i)
                {
                    _size = this.preg_data[i].size;
                    //_unsigned = this.unsign[i];
                    if (_size == 4)
                    {
                        this.value[i] = BitConverter.ToUInt32(_raw, _offset);
                    }
                    else if (_size == 2)
                    {
                        this.value[i] = BitConverter.ToUInt16(_raw, _offset);
                    }
                    else
                    {
                        byte[] _strByte;
                        _strByte = new byte[_size];
                        Array.Copy(_raw, _offset, _strByte, 0, _size);
                        // 바이트 배열을 String으로 변환 
                        string str = Encoding.Default.GetString(_strByte);
                        this.value[i] = 0;
                        this.str_byte_value[i] = str;
                    }
                    _offset += (int)_size;
                }
            }//end public void Pasing(byte[] _raw)

        }//end internal class LData

        public class reg_data
        {
            public int index { get; set; }
            public uint size { get; set; }
            public string name { get; set; }
            public string set_value { get; set; }
            public string curr_value { get; set; }
            public string description { get; set; }       
            public bool unsign { get; set; }  //unsigned int = true, signed int= false;

                
            public reg_data()
            {
                index = 0;
                size = 0;
                name = "";
                set_value = "0";
                curr_value = "0";
                description = "";
                unsign = true;
            }
        }
        
       
        public uint ReadStartIndex { get; set; }
        public uint ReadWordSize { get; set; }
        public uint ReadVariableCount { get; set; }
        public uint WriteStartIndex { get; set; }
        public uint WriteWordSize { get; set; }
        public uint WriteVariableCount { get; set; }


        private reg_data[] m_read_data = null;
        private reg_data[] m_write_data = null;
        //
        private uint m_add_index_no_read;
        private uint m_add_index_no_write;
        //
        public PlcFinsData()
        {

            //for Debug
            Console.WriteLine("PlcFinsData() call");

            m_add_index_no_read = 0;
            m_add_index_no_write = 0;
            //
        }

        public void ReadAreaDataParsing(byte[] _raw)
        {
            try
            {
                _CurrReadLData.Pasing(_raw);
            }
            catch (NullReferenceException nex)
            {
                Console.WriteLine("PlcFinsData::ReadAreaDataParsing()");
                Console.WriteLine(nex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine("PlcFinsData::ReadDataParsing()");
                Console.WriteLine(exc.Message);
            }
            
        }

        public void ReadAreaDataCopyNChangeCheck()
        {
            uint uResult;
            uResult = _CurrReadLData.Copy_If_Diff(_PrevReadLData);
            Console.WriteLine("PlcFinsData::ReadDataCopyNChangeCheck() Diff count = {0}", uResult);
            if(uResult > 0) 
            {
                App.GetApp().GetMainWindow().GetReadPage().Dispatcher.BeginInvoke(new System.Action(OnReadAreaDataChanged));
            }

        }
        //-----------------------------------------
        public void WriteAreaDataParsing(byte[] _raw)
        {
            try
            {
                _CurrWriteLData.Pasing(_raw);
            }
            catch (NullReferenceException nex)
            {
                Console.WriteLine("PlcFinsData::WriteAreaDataParsing()");
                Console.WriteLine(nex.Message);
            }
            catch (Exception exc)
            {
                Console.WriteLine("PlcFinsData::WriteAreaDataParsing()");
                Console.WriteLine(exc.Message);
            }

        }

        public void WriteAreaDataCopyNChangeCheck()
        {
            uint uResult;
            uResult = _CurrWriteLData.Copy_If_Diff(_PrevWriteLData);
            Console.WriteLine("PlcFinsData::WriteAreaDataCopyNChangeCheck() Diff count = {0}", uResult);
            if (uResult > 0)
            {
                App.GetApp().GetMainWindow().GetWritePage().Dispatcher.BeginInvoke(new System.Action(OnWriteAreaDataChanged));
            }

        }

        public void FinsDataSetting()
        {
            ExcelMapFileRead();            
        }

        public reg_data[] GetReadData()
        {
            return m_read_data;
        }
        public reg_data[] GetWriteData()
        {
            return m_write_data;
        }

        private void ExcelMapFileRead()
        {
            Console.WriteLine("MapFileRead() - execl file read...");

            //excel file read
            string fileName = string.Format(@"{0}\fins_data_map.xlsx", System.Environment.CurrentDirectory); //파일 경로
            ReadExcelData(fileName); // 엑셀파일 읽기
        }

        private void ReleaseObject(object obj)
        {
            try
            {
                if (obj != null)
                {
                    // 객체 메모리 해제
                    Marshal.ReleaseComObject(obj);
                    obj = null;
                }
            }
            catch (Exception ex)
            {
                obj = null;
                throw ex;
            }
            finally
            {
                GC.Collect();   // 가비지 수집
            }
        }

        private void ReadExcelData(string fileExcel)
        {
            bool _is_read_map_data = false;
            bool _is_write_map_data = false;
            //
            bool _next_is_start_index = false;
            bool _next_is_total_word_size = false;
            bool _next_is_variables_count = false;
            bool _next_is_map_data = false;
            bool _map_data_parsing = false;
            //
            int _parsing_sheet_count = 0;
            //listBox1.Items.Clear();
            //List<string> buff = new List<string>();

            if (File.Exists(fileExcel) == false)
            {
                return;
            }

            Excel.Application excelApp = null;
            Excel.Workbook workBook = null;

            try
            {
                excelApp = new Excel.Application();                             // 엑셀 어플리케이션 생성
                workBook = excelApp.Workbooks.Open(fileExcel,
                    0,
                    true,
                    5,
                    "",
                    "",
                    true,
                    Excel.XlPlatform.xlWindows,
                    "\t",
                    false,
                    false,
                    0,
                    true,
                    1,
                    0);

                // Sheet항목들을 돌아가면서 내용을 확인
                foreach (Excel.Worksheet workSheet in workBook.Worksheets)
                {
                    //buff.Add("");
                    //buff.Add(workSheet.Name); // 표시용 데이터 추가                  

                    _next_is_start_index        = false;
                    _next_is_total_word_size    = false;
                    _next_is_variables_count    = false;
                    _next_is_map_data           = false;
                    _map_data_parsing           = false;
                    //
                    _is_read_map_data = false;
                    _is_write_map_data = false;
                   
                    string str_sheet_name = workSheet.Name;

                    //for Debug
                    Console.WriteLine(str_sheet_name);
                    //
                    _is_read_map_data  = string.Equals(str_sheet_name, "read_map");
                    _is_write_map_data = string.Equals(str_sheet_name, "write_map");
                    //
                    if (_is_read_map_data )
                    { 
                        ++_parsing_sheet_count;
                    }
                    else if( _is_write_map_data)
                    { 
                        ++_parsing_sheet_count;                        
                    }

                    
                    //
                    Excel.Range range = workSheet.UsedRange;    // 사용중인 셀 범위를 가져오기

                    // 가져온 행(row) 만큼 반복
                    //[map_data]
                    //index size(byte)  name init_value  description
                    // 위 2줄 때문에 -2 를 연산함.
                    for (int row = 1; row <= (range.Rows.Count - 2); row++)
                    {
                        //List<string> lstCell = new List<string>();

                        // 가져온 열(row) 만큼 반복
                        for (int column = 1; column <= range.Columns.Count; column++)
                        {
                            try
                            {
                                //start_index
                                //total_word_size
                                //variables_count
                                //[map_data]
                                // 위의 문구들 바로 다음 열(column)에 값이 온다고 가정

                                object obj = (range.Cells[row, column] as Excel.Range).Value2;
                                if (obj == null) { }
                                else
                                {
                                    string str = obj.ToString();  // 셀 데이터 가져옴

                                    if (_map_data_parsing == false)
                                    {
                                        if (_next_is_start_index)
                                        {
                                            if (_is_read_map_data)
                                                ReadStartIndex = UInt32.Parse(str);
                                            else
                                                WriteStartIndex = UInt32.Parse(str);

                                            _next_is_start_index = false;
                                        }
                                        else if (_next_is_total_word_size)
                                        {
                                            if (_is_read_map_data)
                                                ReadWordSize = UInt32.Parse(str);
                                            else
                                                WriteWordSize = UInt32.Parse(str);

                                            _next_is_total_word_size = false;
                                        }
                                        else if (_next_is_variables_count)
                                        {
                                            if (_is_read_map_data)
                                            {
                                                ReadVariableCount = UInt32.Parse(str);
                                               
                                            }
                                            else
                                                WriteVariableCount = UInt32.Parse(str);

                                            _next_is_variables_count = false;
                                        }

                                        _next_is_start_index = string.Equals(str, "start_index");
                                        _next_is_total_word_size = string.Equals(str, "total_word_size");
                                        _next_is_variables_count = string.Equals(str, "variables_count");
                                        _next_is_map_data = string.Equals(str, "[map_data]");
                                        if (_next_is_map_data)
                                        {
                                            _map_data_parsing = true;

                                            if (_is_read_map_data)
                                            {
                                                m_read_data = new reg_data[ReadVariableCount];
                                                //
                                                _CurrReadLData = new LData(ReadVariableCount, ref m_read_data);
                                                _PrevReadLData = new LData(ReadVariableCount, ref m_read_data);
                                            }
                                            else
                                            {
                                                m_write_data = new reg_data[WriteVariableCount];
                                                //
                                                _CurrWriteLData = new LData(WriteVariableCount, ref m_write_data);
                                                _PrevWriteLData = new LData(WriteVariableCount, ref m_write_data);
                                            }                                            
                                            
                                            break;
                                        }
                                    }//end if (_map_data_parsing == false)                                    

                                    //lstCell.Add(str); // 리스트에 할당

                                    //for Debug
                                    Console.WriteLine("{2} : row={0},column={1}", row, column, str);
                                    //buff.Add(string.Join(",", lstCell.ToArray())); // 표시용 데이터 추가                                   
                                }                               
                            }
                            catch (NullReferenceException nex)
                            {
                                Console.WriteLine(nex.ToString());
                                //continue;
                            }
                        }//end for (int column = 1; column <= range.Columns.Count; column++)

                        if (_map_data_parsing == true)
                        {
                            //_map_data_parsing = true 상태

                            try
                            {                            
                                // 셀 데이터 가져옴
                                //[1]index	[2]size(byte)	[3]name	[4]init_value	[5]description
                                object obj_data_index = (range.Cells[(row + 2), 1] as Excel.Range).Value2;
                                object obj_data_size = (range.Cells[(row + 2), 2] as Excel.Range).Value2;
                                object obj_data_name = (range.Cells[(row + 2), 3] as Excel.Range).Value2;
                                object obj_data_ivalue = (range.Cells[(row + 2), 4] as Excel.Range).Value2;
                                object obj_data_sign = (range.Cells[(row + 2), 5] as Excel.Range).Value2;
                                object obj_data_desc = (range.Cells[(row + 2), 6] as Excel.Range).Value2;

                                if (_is_read_map_data)
                                {
                                    uint _idx = m_add_index_no_read;
                                    m_read_data[_idx] = new reg_data();
                                    //
                                    if (obj_data_index == null) { }
                                    else
                                    {
                                        string str = obj_data_index.ToString();
                                        m_read_data[_idx].index = int.Parse(str);
                                    }

                                    if (obj_data_size == null) { }
                                    else
                                    {
                                        string str = obj_data_size.ToString();
                                        m_read_data[_idx].size = uint.Parse(str);
                                    }

                                    if (obj_data_name == null) { }
                                    else
                                    {
                                        string str = obj_data_name.ToString();
                                        m_read_data[_idx].name = str;
                                    }

                                    if (obj_data_ivalue == null) { }
                                    else
                                    {
                                        string str = obj_data_ivalue.ToString();
                                        m_read_data[_idx].set_value = str;//int.Parse(str);
                                    }

                                    if (obj_data_desc == null) { }
                                    else
                                    {
                                        string str = obj_data_desc.ToString();
                                        m_read_data[_idx].description = str;
                                    }
                                    if(obj_data_sign == null)
                                    { }
                                    else
                                    {
                                        string str = obj_data_sign.ToString();
                                        if( string.Equals(str, "signed") )
                                        {
                                            m_read_data[_idx].unsign = false;
                                        }
                                        else
                                        {
                                            m_read_data[_idx].unsign = true;
                                        }

                                    }//end else(obj_data_sign == null)
                                    //
                                    m_add_index_no_read++;
                                    if (m_add_index_no_read >= ReadVariableCount)
                                    {
                                        //엑셀파일에 의도치 않게 쓰레기값을 입력해 놓을 경우를 대비해서 
                                        //예정된 갯수만 처리하고 끝
                                        break;
                                    }
                                }//end  if (_is_read_map_data)
                                else if (_is_write_map_data)
                                {
                                    uint _idx = m_add_index_no_write;
                                    m_write_data[_idx] = new reg_data();
                                    //
                                    if (obj_data_index == null) { }
                                    else
                                    {
                                        string str = obj_data_index.ToString();
                                        m_write_data[_idx].index = int.Parse(str);
                                    }

                                    if (obj_data_size == null) { }
                                    else
                                    {
                                        string str = obj_data_size.ToString();
                                        m_write_data[_idx].size = uint.Parse(str);
                                    }
                                    if (obj_data_name == null) { }
                                    else
                                    {
                                        string str = obj_data_name.ToString();
                                        m_write_data[_idx].name = str;
                                    }

                                    if (obj_data_ivalue == null)
                                    {
                                        m_write_data[_idx].set_value = "0";
                                    }
                                    else
                                    {
                                        string str = obj_data_ivalue.ToString();
                                        m_write_data[_idx].set_value = str;//int.Parse(str);
                                    }

                                    if (obj_data_desc == null) { }
                                    else
                                    {
                                        string str = obj_data_desc.ToString();
                                        m_write_data[_idx].description = str;
                                    }
                                    if (obj_data_sign == null){ }
                                    else
                                    {
                                        string str = obj_data_sign.ToString();
                                        if (string.Equals(str, "signed"))
                                        {
                                            m_write_data[_idx].unsign = false;
                                        }
                                        else
                                        {
                                            m_write_data[_idx].unsign = true;
                                        }

                                    }//end else(obj_data_sign == null)
                                    //
                                    m_add_index_no_write++;
                                    if (m_add_index_no_write >= WriteVariableCount)
                                    {
                                        //엑셀파일에 의도치 않게 쓰레기값을 입력해 놓을 경우를 대비해서 
                                        //예정된 갯수만 처리하고 끝
                                        break;
                                    }
                                }
                            }
                            catch (System.FormatException fex)
                            {
                                Console.WriteLine(fex.ToString());
                            }
                            catch (System.Exception exc)
                            {
                                Console.WriteLine(exc.ToString());
                            }
                        }

                    }//end for (int row = 1; row <= range.Rows.Count; row++)
                    //
                    if (_parsing_sheet_count >= 2)
                    {
                        Console.WriteLine("read/wrtie parsing complete!!");
                        break;
                    }
                }//end foreach (Excel.Worksheet workSheet in workBook.Worksheets)

                object missing = Type.Missing;
                object noSave = false;
                workBook.Close(noSave, missing, missing); // 엑셀 웨크북 종료
                excelApp.Quit();        // 엑셀 어플리케이션 종료
            }
            finally
            {
                ReleaseObject(workBook);
                ReleaseObject(excelApp);
            }

           
        //listBox1.Items.Add(buff.ToArray());
    }
    }
}
