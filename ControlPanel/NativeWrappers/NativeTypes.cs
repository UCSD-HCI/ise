using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ControlPanel.NativeWrappers
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
        DepthSynchronizedProduct,	//this depth data is synchronized with the engine thread

        DepthThresholdFilteredProduct,
        DepthOpenedProduct,
        DepthSobeledProduct,

        DebugDepthHistogramedProduct,
        DebugOmniOutputProduct,
        DebugThresholdOutputProduct,
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
	    Table2D = 0,
	    Table3D,
	    RGB2D,
	    Depth2D,
	    Depth3D,
	    Motion2D
    };

    /// <summary>
    /// <remarks>ThreadUtils.h, ReadLockedWrapperPtr</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct ReadLockedWrapperPtr
    {
        private IntPtr obj;
        private IntPtr readLock;

        public byte* BytePtr { get { return (byte*)obj; } }
        public ushort* UShortPtr { get { return (ushort*)obj; } }
        public IntPtr IntPtr { get { return (IntPtr)obj; } }
    }

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
        private FloatPoint3D positionInRealWorld;
        private IntPoint3D positionInKinectProj;
        private FingerType fingerType;
        private FingerState fingerState;

        public Finger(int id, FloatPoint3D positionInRealWorld, IntPoint3D positionInKinectPersp, FingerType fingerType, FingerState fingerState)
        {
            this.id = id;
            this.positionInKinectProj = positionInKinectPersp;
            this.positionInRealWorld = positionInRealWorld;
            this.fingerType = fingerType;
            this.fingerState = fingerState;
        }

        public int ID { get { return id; } }
        public FloatPoint3D PositionInRealWorld { get { return positionInRealWorld; } }
        public IntPoint3D PositionInKinectPersp { get { return positionInKinectProj; } }
        public FingerType FingerType { get { return fingerType; } }
    }
    
    /// <summary>
    /// <remarks>HandTracker.h, Hand</remarks>
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct Hand
    {
        private HandType handType;
	    private UInt32 id;
	    private FloatPoint3D positionInRealWorld;
	    private IntPoint3D positionInKinectProj;
	    private double confidence;

        public HandType HandType { get { return handType; } }
        public UInt32 ID { get { return id; } }
        public FloatPoint3D PositionInRealWorld { get { return positionInRealWorld; } }
        public IntPoint3D PositionInKinectProj { get { return positionInKinectProj; } }
        public double Confidence { get { return confidence; } }
    }

    public unsafe delegate void RGBCalibrationFinishedDelegate(FloatPoint3D* checkPoints, int checkPointNum, FloatPoint3D* depthRefCorners, int depthRefCornerNum);

    /// <summary>
    /// <remarks>FingerEventsGenerator.h, FingerEventType</remarks>
    /// </summary>
    public enum FingerEventType
    {
	    FingerMove,
	    FingerDown,
	    FingerUp,

	    //These two are ignored currently because we only process on-surface fingers.
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
	    private FloatPoint3D position;
	    private FingerEventType eventType;

        public int ID { get { return id; } }
        public FloatPoint3D Position { get { return position; } }
        public FingerEventType EventType { get { return eventType; } }
    };
}
