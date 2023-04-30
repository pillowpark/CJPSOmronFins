using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OmronFinsViewerCS.Device;

namespace OmronFinsViewerCS
{
    public class DataManager
    {
        private PlcFinsData m_PlcFinsData = null;
        private FinsDevice m_FinsDevice = null;

        public String m_strOmronPlcIP;
        public Int32 m_strOmronFinsPort;

        public DataManager() 
        {
            m_PlcFinsData = new PlcFinsData();
            m_FinsDevice = new FinsDevice();
        }

        //----
        public PlcFinsData GetFinsData()
        {
            return m_PlcFinsData;
        }

        public void PlcFinsDataCreate()
        {
            m_PlcFinsData.FinsDataSetting();
        }

        public FinsDevice GetFinsDevice()
        {
            return m_FinsDevice;
        }
    }
}
