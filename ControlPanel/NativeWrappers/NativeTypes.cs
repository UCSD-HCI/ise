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
        DepthSourceProduct,

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

        public Finger(int id, FloatPoint3D positionInRealWorld, IntPoint3D positionInKinectPersp)
        {
            this.id = id;
            this.positionInKinectProj = positionInKinectPersp;
            this.positionInRealWorld = positionInRealWorld;
        }

        public int ID { get { return id; } }
        public FloatPoint3D PositionInRealWorld { get { return positionInRealWorld; } }
        public IntPoint3D PositionInKinectPersp { get { return positionInKinectProj; } }
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
}
