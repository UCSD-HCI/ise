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
using InteractiveSpace.EngineController.NativeWrappers;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Diagnostics;

namespace InteractiveSpace.EngineController
{
    /// <summary>
    /// Interaction logic for CalibrationWindow.xaml
    /// </summary>
    public partial class CalibrationWindow : Window
    {
        private const int DPI_X = 96;
        private const int DPI_Y = 96;
        private const double CALIBRATION_POINT_RADIUS = 10;
        private const double CALIBRATION_POINT_STROKE = 5;
        private const double TEST_POINT_RADIUS = 5;

        private WriteableBitmap rgbSource, depthSource;
        private RGBCalibrationFinishedDelegate onRGBChessboardDetectedDelegate;
        private ViscaCommandDelegate onPanTiltFinishedDelegate;

        private bool isAllCalibrated = false;
        private ProjectorFeedbackWindow projectorFeedbackWindow;

        int rgbWidth, rgbHeight, depthWidth, depthHeight;

        private List<Ellipse> depthRefEllipses;
        private Ellipse draggingDepthRefPoint = null;
        private Vector draggingDepthRefPointOffset;

        private FloatPoint3D[] refCorners;
        private FloatPoint3D? testPointInTableSurface = null;

        private Ellipse testPointRGB = null, testPointDepth = null, testPointTable = null;

        public CalibrationWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            EngineBackgroundWorker.Instance.EngineUpdateOnMainUI += new EventHandler(engineUpdate);

            unsafe
            {
                rgbWidth = CommandDllWrapper.getRGBWidth();
                rgbHeight = CommandDllWrapper.getRGBHeight();
                depthWidth = CommandDllWrapper.getDepthWidth();
                depthHeight = CommandDllWrapper.getDepthHeight();
            }

            rgbSource = new WriteableBitmap(rgbWidth, rgbHeight, DPI_X, DPI_Y, PixelFormats.Rgb24, null);
            depthSource = new WriteableBitmap(depthWidth, depthHeight, DPI_X, DPI_Y, PixelFormats.Gray8, null);

            projectorFeedbackWindow = new ProjectorFeedbackWindow();
            projectorFeedbackWindow.Show();
            projectorFeedbackWindow.HitTestLayer.MouseMove += new MouseEventHandler(HitTestLayer_MouseMove);
            projectorFeedbackWindow.HitTestLayer.MouseLeave += new MouseEventHandler(HitTestLayer_MouseLeave);
            projectorFeedbackWindow.HitTestLayer.MouseDown += new MouseButtonEventHandler(HitTestLayer_MouseDown);

            refCorners = projectorFeedbackWindow.ShowChessboard();
            CommandDllWrapper.systemCalibrationStart();
            unsafe
            {
                onRGBChessboardDetectedDelegate = new RGBCalibrationFinishedDelegate(onRGBChessboardDetected);
                onPanTiltFinishedDelegate = new ViscaCommandDelegate(panTiltCallback);

                IntPtr callbackPtr = Marshal.GetFunctionPointerForDelegate(onRGBChessboardDetectedDelegate);
                fixed (FloatPoint3D* refCornersPtr = refCorners)
                {
                    CommandDllWrapper.systemCalibrationDetectChessboardCorner(callbackPtr, refCornersPtr, ProjectorFeedbackWindow.CHESSBOARD_ROWS, ProjectorFeedbackWindow.CHESSBOARD_COLS);
                }
            }
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            projectorFeedbackWindow.HitTestLayer.MouseMove -= HitTestLayer_MouseMove;
            projectorFeedbackWindow.HitTestLayer.MouseLeave -= HitTestLayer_MouseLeave;
            projectorFeedbackWindow.HitTestLayer.MouseDown -= HitTestLayer_MouseDown;

            projectorFeedbackWindow.globalCanvas.Children.Remove(testPointTable);

            projectorFeedbackWindow.Close();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            CommandDllWrapper.systemCalibrationStop();
        }

        void engineUpdate(object sender, EventArgs e)
        {
            if (rgbSource != null)
            {
                IntPtr ptr = ResultsDllWrapper.getCalibrationRGBImage();
                rgbSource.Lock();
                rgbSource.WritePixels(new Int32Rect(0, 0, rgbWidth, rgbHeight), ptr, rgbWidth * rgbHeight * 3, rgbWidth * 3);
                rgbSource.Unlock();
                rgbVideo.ImageSource = rgbSource;
            }

            if (depthSource != null)
            {
                IntPtr ptr = ResultsDllWrapper.getCalibrationDepthImage();
                depthSource.Lock();
                depthSource.WritePixels(new Int32Rect(0, 0, depthWidth, depthHeight), ptr, depthWidth * depthHeight, depthWidth);
                depthSource.Unlock();
                depthVideo.ImageSource = depthSource;
            }
            
        }

        unsafe void onRGBChessboardDetected(FloatPoint3D* checkPoints, int checkPointNum, FloatPoint3D* depthRefPoints, int depthRefPointNum)
        {
            projectorFeedbackWindow.DrawCheckpoints(checkPoints, checkPointNum);

            //draw depth ref corners
            Dispatcher.BeginInvoke((Action)delegate()
            {
                depthRefEllipses = new List<Ellipse>();
                for (int i = 0; i < depthRefPointNum; i++)
                {
                    Ellipse e = new Ellipse()
                    {
                        Fill = Brushes.Yellow,
                        Opacity = 0.8,
                        Stroke = Brushes.Orange,
                        StrokeThickness = CALIBRATION_POINT_STROKE,
                        Width = CALIBRATION_POINT_RADIUS * 2,
                        Height = CALIBRATION_POINT_RADIUS * 2
                    };
                    e.MouseDown += new MouseButtonEventHandler(depthRefPoint_MouseDown);
                    depthVideo.UiCanvas.Children.Add(e);
                    Canvas.SetLeft(e, depthRefPoints[i].x - CALIBRATION_POINT_RADIUS);
                    Canvas.SetTop(e, depthRefPoints[i].y - CALIBRATION_POINT_RADIUS);
                    depthRefEllipses.Add(e);
                }

                calibrateDepthButton.IsEnabled = true;
                calibrateDepthSaveButton.IsEnabled = true;
                calibrateDepthLoadButton.IsEnabled = true;

                //now we can do automatic calibration
                calibrateDepthButton_Click(this, new RoutedEventArgs());
            }, null);
        }

        void depthRefPoint_MouseDown(object sender, MouseButtonEventArgs e)
        {
            draggingDepthRefPoint = sender as Ellipse;
            Point pos = e.GetPosition(depthVideo.UiCanvas);
            draggingDepthRefPointOffset.X = pos.X - Canvas.GetLeft(draggingDepthRefPoint);
            draggingDepthRefPointOffset.Y = pos.Y - Canvas.GetTop(draggingDepthRefPoint);
        }

        private void video_MouseMove(object sender, MouseEventArgs e)
        {
            if (sender == depthVideo)
            {
                if (draggingDepthRefPoint != null)
                {
                    Point mousePoint = e.GetPosition(depthVideo.UiCanvas);
                    Canvas.SetLeft(draggingDepthRefPoint, mousePoint.X - draggingDepthRefPointOffset.X);
                    Canvas.SetTop(draggingDepthRefPoint, mousePoint.Y - draggingDepthRefPointOffset.Y);
                }
            }

            ZoomableVideoUserControl video = sender as ZoomableVideoUserControl;

            if (isAllCalibrated)
            {
                Point mousePos = e.GetPosition(video.UiCanvas);
                FloatPoint3D p = new FloatPoint3D()
                {
                    x = (float)mousePos.X,
                    y = (float)mousePos.Y,
                    z = 0
                };

                CalibratedCoordinateSystem srcSpace;
                if (sender == depthVideo)
                {
                    srcSpace = CalibratedCoordinateSystem.Depth2D;
                }
                else if (sender == rgbVideo)
                {
                    srcSpace = CalibratedCoordinateSystem.RGB2D;
                }
                else
                {
                    throw new NotImplementedException();
                }

                unsafe
                {
                    testPointInTableSurface = CommandDllWrapper.transformPoint(p, srcSpace, CalibratedCoordinateSystem.Table2D);
                }

                refreshTestPoint();
            }
        }

        private void video_MouseLeave(object sender, MouseEventArgs e)
        {
            testPointInTableSurface = null;
            refreshTestPoint();
        }

        private void video_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (isAllCalibrated && testPointInTableSurface.HasValue)
            {
                
            }
        }

        private void panTiltCallback(bool isCommandCompleted, IntPtr callbackState)
        {
            Trace.WriteLine("Pan-Tilt " + (isCommandCompleted ? "completed. " : "aborted."));
        }

        void HitTestLayer_MouseMove(object sender, MouseEventArgs e)
        {
            if (isAllCalibrated)
            {
                Point mousePos = e.GetPosition(sender as Rectangle);
                testPointInTableSurface = new FloatPoint3D()
                {
                    x = (float)mousePos.X,
                    y = (float)mousePos.Y,
                    z = 0
                };

                refreshTestPoint();
            }
        }

        void HitTestLayer_MouseLeave(object sender, MouseEventArgs e)
        {
            testPointInTableSurface = null;
            refreshTestPoint();
        }

        void HitTestLayer_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (isAllCalibrated && testPointInTableSurface.HasValue)
            {
                
            }
        }


        private void refreshTestPoint()
        {
            if (testPointRGB == null)
            {
                testPointRGB = new Ellipse
                {
                    Fill = Brushes.Yellow,
                    Opacity = 0.8,
                    Width = TEST_POINT_RADIUS * 2,
                    Height = TEST_POINT_RADIUS * 2,
                    Visibility = Visibility.Hidden,
                };
                rgbVideo.UiCanvas.Children.Add(testPointRGB);
            }

            if (testPointDepth == null)
            {
                testPointDepth = new Ellipse
                {
                    Fill = Brushes.Yellow,
                    Opacity = 0.8,
                    Width = TEST_POINT_RADIUS * 2,
                    Height = TEST_POINT_RADIUS * 2,
                    Visibility = Visibility.Hidden,
                };
                depthVideo.UiCanvas.Children.Add(testPointDepth);
            }

            if (testPointTable == null)
            {
                testPointTable = new Ellipse
                {
                    Fill = Brushes.Yellow,
                    Opacity = 0.8,
                    Width = TEST_POINT_RADIUS * 2,
                    Height = TEST_POINT_RADIUS * 2,
                    Visibility = Visibility.Hidden,
                };
                projectorFeedbackWindow.globalCanvas.Children.Add(testPointTable);
            }

            if (testPointInTableSurface.HasValue)
            {
                FloatPoint3D rgbPos, depthPos;
                unsafe
                {
                    rgbPos = CommandDllWrapper.transformPoint(testPointInTableSurface.Value, CalibratedCoordinateSystem.Table2D, CalibratedCoordinateSystem.RGB2D);
                    depthPos = CommandDllWrapper.transformPoint(testPointInTableSurface.Value, CalibratedCoordinateSystem.Table2D, CalibratedCoordinateSystem.Depth2D);
                }

                Canvas.SetLeft(testPointTable, testPointInTableSurface.Value.x - TEST_POINT_RADIUS);
                Canvas.SetTop(testPointTable, testPointInTableSurface.Value.y - TEST_POINT_RADIUS);

                Canvas.SetLeft(testPointRGB, rgbPos.x - TEST_POINT_RADIUS);
                Canvas.SetTop(testPointRGB, rgbPos.y - TEST_POINT_RADIUS);

                Canvas.SetLeft(testPointDepth, depthPos.x - TEST_POINT_RADIUS);
                Canvas.SetTop(testPointDepth, depthPos.y - TEST_POINT_RADIUS);

                testPointTable.Visibility = Visibility.Visible;
                testPointRGB.Visibility = Visibility.Visible;
                testPointDepth.Visibility = Visibility.Visible;

                realCoordLabel.Content = testPointInTableSurface.ToString();
                rgbCoordLabel.Content = rgbPos.ToString();
                depthCoordLabel.Content = depthPos.ToString();
            }
            else
            {
                testPointTable.Visibility = Visibility.Hidden;
                testPointRGB.Visibility = Visibility.Hidden;
                testPointDepth.Visibility = Visibility.Hidden;
            }
        }

        private void depthVideo_MouseUp(object sender, MouseButtonEventArgs e)
        {
            draggingDepthRefPoint = null;
        }

        private void calibrateDepthButton_Click(object sender, RoutedEventArgs e)
        {
            FloatPoint3D[] depthCorners = new FloatPoint3D[depthRefEllipses.Count];
            for (int i = 0; i < depthRefEllipses.Count; i++)
            {
                depthCorners[i].x = (float)(Canvas.GetLeft(depthRefEllipses[i]) + CALIBRATION_POINT_RADIUS);
                depthCorners[i].y = (float)(Canvas.GetTop(depthRefEllipses[i]) + CALIBRATION_POINT_RADIUS);
                depthCorners[i].z = 0;
            }

            unsafe
            {
                fixed (FloatPoint3D* refCornersPtr = refCorners, depthCornersPtr = depthCorners)
                {
                    CommandDllWrapper.systemCalibrationCalibrateDepthCamera(depthCornersPtr, refCornersPtr, refCorners.Length);
                    isAllCalibrated = true;
                }
            }

            
            foreach (var ellipse in depthRefEllipses)
            {
                //depthVideo.UiCanvas.Children.Remove(ellipse);
                ellipse.MouseDown -= depthRefPoint_MouseDown;
            }
            

            calibrateDepthButton.IsEnabled = false;
            calibrateDepthSaveButton.IsEnabled = false;
            calibrateDepthLoadButton.IsEnabled = false;

            messageLabel.Content = "Calibration done.";
        }

        private void calibrateDepthSaveButton_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.SaveFileDialog dialog = new Microsoft.Win32.SaveFileDialog()
            {
                FileName = "DepthCalibration.bin",
                DefaultExt = ".bin",
                Filter = "Binary|*.bin|All|*.*"
            };

            bool? result = dialog.ShowDialog();

            if (result != true)
            {
                return;
            }

            List<Point2D> points = new List<Point2D>(); ;
            foreach (var ellipse in depthRefEllipses)
            {
                points.Add(new Point2D()
                {
                    x = Canvas.GetLeft(ellipse),
                    y = Canvas.GetTop(ellipse),
                });
            }

            BinaryFormatter formatter = new BinaryFormatter();
            FileStream stream = new FileStream(dialog.FileName, FileMode.Create);
            formatter.Serialize(stream, points);
            stream.Close();
        }

        private void calibrateDepthLoadButton_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog()
            {
                FileName = "DepthCalibration.bin",
                DefaultExt = ".bin",
                Filter = "Binary|*.bin|All|*.*"
            };

            bool? result = dialog.ShowDialog();

            if (result != true)
            {
                return;
            }

            BinaryFormatter formatter = new BinaryFormatter();
            FileStream stream = new FileStream(dialog.FileName, FileMode.Open);
            List<Point2D> points = formatter.Deserialize(stream) as List<Point2D>;
            stream.Close();

            for (int i = 0; i < points.Count; i++)
            {
                Canvas.SetLeft(depthRefEllipses[i], points[i].x);
                Canvas.SetTop(depthRefEllipses[i], points[i].y);
            }
        }

        [Serializable]
        struct Point2D //just for serialization
        {
            public double x;
            public double y;
        }

        private void rgbVideo_MouseDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void okButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
