using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using System.Windows;

namespace InteractiveSpace.EngineController
{
    public enum VideoSourceType
    {
        None = 0,
        RGB,
        DepthHistogramed,
        OmniTouch,
        RectifiedTabletopProduct,
    }

    public class VideoSources
    {
        private const int DPI_X = 96;
        private const int DPI_Y = 96;

        private static VideoSources instance = null;

        private int rgbWidth, rgbHeight, depthWidth, depthHeight;

        private WriteableBitmap rgbSource, depthHistogramedSource, omniTouchSource, rectifiedTabletopSource;

        public static VideoSources SharedVideoSources
        {
            get
            {
                if (instance == null)
                {
                    instance = new VideoSources();
                }

                return instance;
            }
        }

        public VideoSources()
        {
            unsafe
            {
                rgbWidth = CommandDllWrapper.getRGBWidth();
                rgbHeight = CommandDllWrapper.getRGBHeight();
                depthWidth = CommandDllWrapper.getDepthWidth();
                depthHeight = CommandDllWrapper.getDepthHeight();
            }

            rgbSource = new WriteableBitmap(rgbWidth, rgbHeight, DPI_X, DPI_Y, PixelFormats.Rgb24, null);
            depthHistogramedSource = new WriteableBitmap(depthWidth, depthHeight, DPI_X, DPI_Y, PixelFormats.Gray8, null);
            omniTouchSource = new WriteableBitmap(depthWidth, depthHeight, DPI_X, DPI_Y, PixelFormats.Rgb24, null);
            rectifiedTabletopSource = new WriteableBitmap(rgbWidth, rgbHeight, DPI_X, DPI_Y, PixelFormats.Rgb24, null);
        }

        public ImageSource GetSource(VideoSourceType type)
        {
            switch (type)
            {
                case VideoSourceType.RGB:
                    return RGBSource;

                case VideoSourceType.DepthHistogramed:
                    return DepthHistogramedSource;

                case VideoSourceType.OmniTouch:
                    return OmniTouchSource;

                case VideoSourceType.RectifiedTabletopProduct:
                    return RectifiedTabletopSource;

                default:
                    return null;
            }
        }

        public ImageSource RGBSource
        {
            get
            {
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

                return rgbSource;
            }
        }

        public ImageSource DepthHistogramedSource
        {
            get
            {
                unsafe
                {
                    if (depthHistogramedSource != null)
                    {
                        ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFactoryImage(ImageProductType.DebugDepthHistogramedProduct);
                        depthHistogramedSource.Lock();
                        depthHistogramedSource.WritePixels(new Int32Rect(0, 0, depthWidth, depthHeight), ptr.IntPtr, depthWidth * depthHeight, depthWidth);
                        depthHistogramedSource.Unlock();
                        ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    }
                }

                return depthHistogramedSource;
            }
        }

        public ImageSource OmniTouchSource
        {
            get
            {
                unsafe
                {
                    if (omniTouchSource != null)
                    {
                        ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFactoryImage(ImageProductType.DebugOmniOutputProduct);
                        omniTouchSource.Lock();
                        omniTouchSource.WritePixels(new Int32Rect(0, 0, depthWidth, depthHeight), ptr.IntPtr, depthWidth * depthHeight * 3, depthWidth * 3);
                        omniTouchSource.Unlock();
                        ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    }
                }

                return omniTouchSource;
            }
        }

        public ImageSource RectifiedTabletopSource
        {
            get
            {
                unsafe
                {
                    if (rectifiedTabletopSource != null)
                    {
                        ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFactoryImage(ImageProductType.RectifiedTabletopProduct);
                        rectifiedTabletopSource.Lock();
                        rectifiedTabletopSource.WritePixels(new Int32Rect(0, 0, rgbWidth, rgbHeight), ptr.IntPtr, rgbWidth * rgbHeight * 3, rgbWidth * 3);
                        rectifiedTabletopSource.Unlock();
                        ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    }
                }

                return rectifiedTabletopSource;
            }
        }
    }
}
