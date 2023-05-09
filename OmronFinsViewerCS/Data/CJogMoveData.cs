using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OmronFinsViewerCS
{
    public class CJogMoveData
    {

        public int JogValueDirectionX { get; set; }
        public int JogValueDirectionY { get; set; }
        public int JogValueSpeed { get; set; }
        public bool JogModeEnable { get; set; }  
        public CJogMoveData() 
        {
            JogValueDirectionX = 0;
            JogValueDirectionY = 0;
            JogValueSpeed = 0;
            JogModeEnable = false;
        }

    }
}
