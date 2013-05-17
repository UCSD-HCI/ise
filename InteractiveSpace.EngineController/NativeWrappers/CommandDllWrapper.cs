using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace InteractiveSpace.EngineController.NativeWrappers
{
    public static class CommandDllWrapper
    {
        private const string DLL_NAME = "InteractiveSpaceEngine.dll";

        //TODO: change this to engineInit()
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe void engineRun();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe void engineStop(IntPtr stoppedCallback);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe void engineMainLoopUpdate();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe int getRGBWidth();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe int getRGBHeight();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe int getDepthWidth();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe int getDepthHeight();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void setOmniTouchParameters(double fingerMinWidth, double fingerMaxWidth, double fingerMinLength, double fingerMaxLength, double fingerRisingThreshold, double fingerFallingThreshold, double clickFloodMaxGrad);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void systemCalibrationStart();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void systemCalibrationStop();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void systemCalibrationDetectChessboardCorner(IntPtr onFinishedCallback, FloatPoint3D* refCorners, int rows, int cols);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void systemCalibrationCalibrateDepthCamera(FloatPoint3D* depthCorners, FloatPoint3D* refCorners, int cornerCount);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void transformPoints(FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, int pointNum, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe FloatPoint3D transformPoint(FloatPoint3D srcPoint, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void registerHandEventCallbacks(IntPtr handMoveCallback, IntPtr handCapturedCallback, IntPtr handLostCallback);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void registerEngineUpdateCallback(IntPtr engineUpdateCallback);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void startRecording(byte* filepath);

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void stopRecording();

        [DllImport(DLL_NAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern unsafe void setOmniTouchCropping(int left, int top, int right, int bottom);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe void setOmniTouchEnabled(int isEnabled);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe void setTabletopRectifiedEnabled(int isEnabled);
    }
}
