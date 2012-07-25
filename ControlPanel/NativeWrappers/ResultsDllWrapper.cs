﻿using System;
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

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockCalibrationRGBImage();

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockCalibrationDepthImage();

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockFingerEvents(int* fingerNum, long* frame);

        [DllImport(DLL_NAME)]
        public static extern unsafe long getEngineFrameCount();

        [DllImport(DLL_NAME)]
        public static extern unsafe float getFPS();

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockMotionCameraLastGrabbedImage();

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockTrackingDocBounds(int* trackingDocNum);

        /*
        [DllImport(DLL_NAME)]
        public static extern unsafe double getKinectSoundAngle();
         */
    }

}
