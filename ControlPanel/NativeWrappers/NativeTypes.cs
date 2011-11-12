using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ControlPanel.NativeWrappers
{
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
        ImageProductsCount
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
}
