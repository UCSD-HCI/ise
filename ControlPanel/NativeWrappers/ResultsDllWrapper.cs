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
        public static extern unsafe ReadLockedWrapperPtr lockRgbSourceImage();

        [DllImport(DLL_NAME)]
        public static extern unsafe ReadLockedWrapperPtr lockDepthSourceImage();

        [DllImport(DLL_NAME)]
        public static extern unsafe void releaseReadLockedWrapperPtr(ReadLockedWrapperPtr imgPtr);
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    unsafe public struct ReadLockedWrapperPtr
    {
        private IntPtr obj;
        private IntPtr readLock;

        public byte* BytePtr { get { return (byte*) obj; } }
        public ushort* UShortPtr { get { return (ushort*) obj; } }
        public IntPtr IntPtr { get { return (IntPtr)obj; } }
    }
}
