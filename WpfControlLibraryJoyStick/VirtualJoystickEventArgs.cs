﻿using System;

namespace WpfControlLibrary
{
    public class VirtualJoystickEventArgs : EventArgs
    {
        public double Angle { get; set; }
        public double Distance { get; set; }
    }
}
