using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using ControlPanel.NativeWrappers;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace ControlPanel
{
    /// <summary>
    /// Interaction logic for CalibrationWindow.xaml
    /// </summary>
    public partial class CalibrationWindow : Window
    {
        private const int DPI_X = 96;
        private const int DPI_Y = 96;

        private WriteableBitmap rgbSource, depthSource;
        private BackgroundWorker refreshWorker;
        private CalibrationFinishedDelegate onRGBChessboardDetectedDelegate;

        public ProjectorFeedbackWindow ProjectorFeedbackWindow { get; set; }

        int rgbWidth, rgbHeight, depthWidth, depthHeight;

        public CalibrationWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            unsafe
            {
                rgbWidth = CommandDllWrapper.getRGBWidth();
                rgbHeight = CommandDllWrapper.getRGBHeight();
                depthWidth = CommandDllWrapper.getDepthWidth();
                depthHeight = CommandDllWrapper.getDepthHeight();
            }

            rgbSource = new WriteableBitmap(rgbWidth, rgbHeight, DPI_X, DPI_Y, PixelFormats.Rgb24, null);
            depthSource = new WriteableBitmap(depthWidth, depthHeight, DPI_X, DPI_Y, PixelFormats.Gray8, null);

            refreshWorker = new BackgroundWorker();
            refreshWorker.DoWork += new DoWorkEventHandler(refreshWorker_DoWork);
            CompositionTarget.Rendering += new EventHandler(CompositionTarget_Rendering);

            FloatPoint3D[] refCorners = ProjectorFeedbackWindow.ShowChessboard();
            CommandDllWrapper.systemCalibrationStart();
            unsafe
            {
                onRGBChessboardDetectedDelegate = new CalibrationFinishedDelegate(onRGBChessboardDetected);
                IntPtr callbackPtr = Marshal.GetFunctionPointerForDelegate(onRGBChessboardDetectedDelegate);
                fixed (FloatPoint3D* refCornersPtr = refCorners)
                {
                    CommandDllWrapper.systemCalibrationDetectChessboardCorner(callbackPtr, refCornersPtr, ProjectorFeedbackWindow.CHESSBOARD_ROWS, ProjectorFeedbackWindow.CHESSBOARD_COLS);
                }
            }
        }


        private void Window_Closed(object sender, EventArgs e)
        {
            ProjectorFeedbackWindow.HideChecssboard();
            CommandDllWrapper.systemCalibrationStop();
        }

        void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            if (!refreshWorker.IsBusy)
            {
                refreshWorker.RunWorkerAsync();
            }
        }

        void refreshWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate
            {
                if (rgbSource != null)
                {
                    ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockCalibrationRGBImage();
                    rgbSource.Lock();
                    rgbSource.WritePixels(new Int32Rect(0, 0, rgbWidth, rgbHeight), ptr.IntPtr, rgbWidth * rgbHeight * 3, rgbWidth * 3);
                    rgbSource.Unlock();
                    ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    rgbVideo.ImageSource = rgbSource;
                }

                if (depthSource != null)
                {
                    ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockCalibrationDepthImage();
                    depthSource.Lock();
                    depthSource.WritePixels(new Int32Rect(0, 0, depthWidth, depthHeight), ptr.IntPtr, depthWidth * depthHeight, depthWidth);
                    depthSource.Unlock();
                    ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    depthVideo.ImageSource = depthSource;
                }

            }, null);
        }

        unsafe void onRGBChessboardDetected(FloatPoint3D* checkPoints, int checkPointNum)
        {
            for (int i = 0; i < checkPointNum; i++)
            {
                ProjectorFeedbackWindow.DrawCheckpoints(checkPoints, checkPointNum);
            }
        }

    }
}
