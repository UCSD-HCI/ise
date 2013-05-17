using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace InteractiveSpace.EngineController.NativeWrappers
{
    public static class ResultsDllWrapper
    {
        private const string DLL_NAME = "InteractiveSpaceEngine.dll";

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe IntPtr getFactoryImage(ImageProductType product);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe IntPtr getFingers(int* fingerNum);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe IntPtr getHands(int* handNum);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe IntPtr getCalibrationRGBImage();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe IntPtr getCalibrationDepthImage();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe IntPtr getFingerEvents(int* fingerNum, long* frame);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe long getEngineFrameCount();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe float getFPS();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe Matrix33 getRgbSurfHomography();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe Matrix33 getDepthSurfHomography();
    }

}
