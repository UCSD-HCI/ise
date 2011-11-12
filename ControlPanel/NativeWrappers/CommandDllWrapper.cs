﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ControlPanel.NativeWrappers
{
    public static class CommandDllWrapper
    {
        private const string DLL_NAME = "InteractiveSpaceEngine.dll";

        [DllImport(DLL_NAME)]
        public static extern unsafe void engineRun();

        [DllImport(DLL_NAME)]
        public static extern unsafe void engineStop();

        [DllImport(DLL_NAME)]
        public static extern unsafe int getRGBWidth();

        [DllImport(DLL_NAME)]
        public static extern unsafe int getRGBHeight();

        

        [DllImport(DLL_NAME)]
        public static extern unsafe int getDepthWidth();

        [DllImport(DLL_NAME)]
        public static extern unsafe int getDepthHeight();
    }
}
