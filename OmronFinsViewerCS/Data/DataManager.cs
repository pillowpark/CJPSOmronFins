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
        //
        private CJogMoveData m_JogMoveData = null;

        public String m_strOmronPlcIP;
        public Int32  m_nOmronFinsPort;
        //
        public Int32 m_nOmronFinsTCPUDPSel;
        public Int32 m_nOmronFinsAreaBlock;
        //
        public Int32 m_nOmronFinsDNA;
        public Int32 m_nOmronFinsDA1;
        public Int32 m_nOmronFinsDA2;
        //
        public Int32 m_nOmronFinsSNA;
        public Int32 m_nOmronFinsSA1;
        public Int32 m_nOmronFinsSA2;
        //


        public DataManager() 
        {
            m_PlcFinsData = new PlcFinsData();
            m_FinsDevice = new FinsDevice();
            //
            m_JogMoveData = new CJogMoveData();
        }

        //----
        public CJogMoveData GetJogCmdData()
        {
            return m_JogMoveData;
        }
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
