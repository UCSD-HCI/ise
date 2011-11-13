using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ControlPanel.NativeWrappers
{
    public static class ResultsDllWrapper
    {
        private const string DLL_NAME = "InteractiveSpaceEngine.dll";

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockFactoryImage(ImageProductType product);

        [DllImport(DLL_NAME)]
        public static extern unsafe void releaseReadLockedWrapperPtr(ReadLockedWrapperPtr imgPtr);

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockFingers(int* fingerNum);

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockHands(int* handNum);
    }

}
