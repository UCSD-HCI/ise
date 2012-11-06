using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;
using InteractiveSpace.EngineController;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Windows.Media;
using System.Windows;
using System.Windows.Threading;

namespace InteractiveSpace.SDK.DLL
{
    public class RawVideoStreamingDLL : RawVideoStreaming
    {
        private const int DPI_X = 96;
        private const int DPI_Y = 96;

        WriteableBitmap rgbSource, depthSource;
        int rgbWidth, rgbHeight, depthWidth, depthHeight;

        public RawVideoStreamingDLL()
        {
            unsafe
            {
                rgbWidth = CommandDllWrapper.getRGBWidth();
                rgbHeight = CommandDllWrapper.getRGBHeight();
                depthWidth = CommandDllWrapper.getDepthWidth();
                depthHeight = CommandDllWrapper.getDepthHeight();
            }

            rgbSource = new WriteableBitmap(rgbWidth, rgbHeight, DPI_X, DPI_Y, PixelFormats.Rgb24, null);
            depthSource = new WriteableBitmap(depthWidth, depthHeight, DPI_X, DPI_Y, PixelFormats.Gray16, null);
        }

        internal void Refresh(Dispatcher dispatcher)
        {
            dispatcher.BeginInvoke((Action)delegate
            {
                //RGB
                unsafe
                {
                    if (rgbSource != null)
                    {
                        ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFactoryImage(ImageProductType.RGBSourceProduct);
                        rgbSource.Lock();
                        rgbSource.WritePixels(new Int32Rect(0, 0, rgbWidth, rgbHeight), ptr.IntPtr, rgbWidth * rgbHeight * 3, rgbWidth * 3);
                        rgbSource.Unlock();
                        ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    }
                }

                //Depth
                unsafe
                {
                    if (depthSource != null)
                    {
                        ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFactoryImage(ImageProductType.DepthSynchronizedProduct);
                        depthSource.Lock();
                        depthSource.WritePixels(new Int32Rect(0, 0, depthWidth, depthHeight), ptr.IntPtr, depthWidth * depthHeight * 2, depthWidth * 2);
                        depthSource.Unlock();
                        ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    }
                }
            }, null);
        }


        public WriteableBitmap RgbVideo
        {
            get { return rgbSource; }
        }

        public WriteableBitmap DepthVideo
        {
            get { return depthSource; }
        }
    }
}
