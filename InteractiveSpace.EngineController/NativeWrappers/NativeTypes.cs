﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace InteractiveSpace.EngineController.NativeWrappers
{
    public static class NativeConstants
    {
        /// <summary>
        /// <remarks>FingerSelector.h, MAX_FINGER_NUM</remarks>
        /// </summary>
        public const int MAX_FINGER_NUM = 10;

        /// <summary>
        /// <remarks>HandTracker.h, MAX_HAND_NUM</remarks>
        /// </summary>
        public const int MAX_HAND_NUM = 2;
    }

    /// <summary>
    /// <remarks>ImageProcessingFactory.h, ImageProductType</remarks>
    /// </summary>
    public enum ImageProductType
    {
        RGBSourceProduct,
        DepthSourceProduct,	//this depth data is from the Kinect thread
        DepthToRGBCoordProduct,

        DebugDepthHistogramedProduct,
        DebugOmniOutputProduct,
        DebugOmniTransposedOutputProduct,

        RectifiedTabletopProduct,

        ImageProductsCount
    }

    /// <summary>
    /// <remarks>HandTracker.h, HandType</remarks>
    /// </summary>
    public enum HandType
    {
	    NewHandHint = 0,	//new added hand hint, track not requested
	    TrackRequestedHandHint,	//track requested, but OpenNI have not detect yet
	    TrackingHand	//OpenNI is tracking this hand
    }

    /// <summary>
    /// <remarks>FingerSelector.h, FingerType</remarks>
    /// </summary>
    public enum FingerType
    {
        OmniFinger,
        ThresholdFinger
    }

    public enum FingerState
    {
	    FingerHovering,
	    FingerOnSurface
    }

    /// <summary>
    /// <remarks>Calibrator.h, CalibratedCoordinateSystem</remarks>
    /// </summary>
    public enum CalibratedCoordinateSystem
    {
        SpaceTabletop = 0,
        SpaceRGB,
        SpaceDepthProjective,
        SpaceDepthReal
    };

    /// <summary>
    /// <remarks>Calibrator.h, CoordinateSpaceConversion</remarks>
    /// </summary>
    public enum CoordinateSpaceConversion
    {
        //forward
        SpaceRGBToDepthProjective = 0,          //warning: from RGB to depth/tabletop is very expensive
        SpaceDepthProjectiveToDepthReal,
        SpaceDepthRealToTabletop,
        SpaceRGBToDepthReal,
        SpaceRGBToTabletop,
        SpaceDepthProjectiveToTabletop,

        //backward
        SpaceTabletopToDepthReal,
        SpaceDepthRealToDepthProjective,
        SpaceDepthProjectiveToRGB,
        SpaceTabletopToDepthProjective,
        SpaceDepthRealToRGB,
        SpaceTabletopToRGB
    };

    /// <summary>
    /// <remarks>InteractiveSpaceTypes.h, IntPoint3D</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct IntPoint3D
    {
        public int x, y, z;
    }

    /// <summary>
    /// <remarks>InteractiveSpaceTypes.h, FloatPoint3D</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct FloatPoint3D
    {
        public float x, y, z;
        public FloatPoint3D(double x, double y, double z)
        {
            this.x = (float)x;
            this.y = (float)y;
            this.z = (float)z;
        }
        public override string ToString()
        {
            return string.Format("{0:0.00},{1:0.00},{2:0.00}", x, y, z);
        }
    }

    /// <summary>
    /// <remarks>FingerSelector.h, Finger</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct Finger
    {
        private int id;
        private FloatPoint3D positionInKinectReal;
        private IntPoint3D positionInKinectProj;
        private FingerType fingerType;
        private FingerState fingerState;

        public Finger(int id, FloatPoint3D positionInKinectReal, IntPoint3D positionInKinectPersp, FingerType fingerType, FingerState fingerState)
        {
            this.id = id;
            this.positionInKinectProj = positionInKinectPersp;
            this.positionInKinectReal = positionInKinectReal;
            this.fingerType = fingerType;
            this.fingerState = fingerState;
        }

        public int ID { get { return id; } }
        public FloatPoint3D PositionInKinectReal { get { return positionInKinectReal; } }
        public IntPoint3D PositionInKinectPersp { get { return positionInKinectProj; } }
        public FingerType FingerType { get { return fingerType; } }
        public FingerState FingerState { get { return fingerState; } }
    }
    
    /// <summary>
    /// <remarks>HandTracker.h, Hand</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct Hand
    {
        private HandType handType;
	    private UInt32 id;
	    private FloatPoint3D positionInKinectReal;
	    private IntPoint3D positionInKinectProj;
	    private double confidence;
        private int captured;

        public HandType HandType { get { return handType; } }
        public UInt32 ID { get { return id; } }
        public FloatPoint3D PositionInKinectReal { get { return positionInKinectReal; } }
        public IntPoint3D PositionInKinectProj { get { return positionInKinectProj; } }
        public double Confidence { get { return confidence; } }
        public int Captured { get { return captured; } }
    }

    /// <summary>
    /// <remarks>FingerEventsGenerator.h, FingerEventType</remarks>
    /// </summary>
    public enum FingerEventType
    {
	    FingerMove,
	    FingerDown,
	    FingerUp,

	    FingerCaptured,
	    FingerLost
    };

    /// <summary>
    /// <remarks>FingerEventsGenerator.h, FingerEvent</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct FingerEvent
    {
	    private int id;
	    private FloatPoint3D positionInKinectReal;
        private FloatPoint3D positionTabletop;
	    private FingerEventType eventType;
        private FingerState fingerState;

        public int ID { get { return id; } }
        public FloatPoint3D PositionInKinectReal { get { return positionInKinectReal; } }
        public FloatPoint3D PositionTabletop { get { return positionTabletop; } }
        public FingerEventType EventType { get { return eventType; } }
        public FingerState FingerState { get { return fingerState; } }
    };

    /// <summary>
    /// <remarks>HandTracker.h, HandEventType</remarks>
    /// </summary>
    public enum HandEventType
    {
	    HandMove,
	    HandCaptured,
	    HandLost
    };


    /// <summary>
    /// <remarks>HandTracker.h, HandEvent</remarks>
    /// </summary> 
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct HandEvent
    {
	    private int id;
	    private FloatPoint3D positionInKinectReal;
	    private FloatPoint3D positionTabletop;
	    private HandEventType eventType;

        public int ID { get { return id; } }
        public FloatPoint3D PositionInKinectReal { get { return positionInKinectReal; } }
        public FloatPoint3D PositionTabletop { get { return positionTabletop; } }
        public HandEventType EventType { get { return eventType; } }
    };

    /// <summary>
    /// <remarks>InteractiveSpaceTypes.h, Quadrilateral</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
    public struct Quadrilateral
    {
        private FloatPoint3D p1, p2, p3, p4;

        public FloatPoint3D P1 { get { return p1; } }
        public FloatPoint3D P2 { get { return p2; } }
        public FloatPoint3D P3 { get { return p3; } }
        public FloatPoint3D P4 { get { return p4; } }
    }

    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
    public struct Matrix33
    {
        private double m11, m12, m13, m21, m22, m23, m31, m32, m33;

        public double M11 { get { return m11; } }
        public double M12 { get { return m12; } }
        public double M13 { get { return m13; } }
        public double M21 { get { return m21; } }
        public double M22 { get { return m22; } }
        public double M23 { get { return m23; } }
        public double M31 { get { return m31; } }
        public double M32 { get { return m32; } }
        public double M33 { get { return m33; } }
    }

    public unsafe delegate void ViscaCommandDelegate(bool isCommandCompleted, IntPtr callbackState);
    public unsafe delegate void RGBCalibrationFinishedDelegate(FloatPoint3D* checkPoints, int checkPointNum, FloatPoint3D* depthRefCorners, int depthRefCornerNum);

    /// <summary>
    /// <remarks>HandTracker.h, HandEventCallback</remarks>
    /// </summary>
    /// <param name="e"></param>
    public unsafe delegate void HandEventCallback(HandEvent e);
}
