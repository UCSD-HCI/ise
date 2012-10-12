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
using System.Diagnostics;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Windows.Media.Effects;
using System.ComponentModel;

namespace InteractiveSpace.EngineController
{
    /// <summary>
    /// Interaction logic for ProjectorFeedbackWindow.xaml
    /// </summary>
    public partial class ProjectorFeedbackWindow : Window
    {
        public const int CHESSBOARD_ROWS = 4;
        public const int CHESSBOARD_COLS = 5;

        private const double HAND_RADIUS = 50;
        private const double FINGER_RADIUS = 10;

        private List<Ellipse> checkPointEllipses;
        private List<Ellipse> handPoints;
        private List<Ellipse> fingerPoints;
        private BackgroundWorker refreshWorker;

        public ProjectorFeedbackWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Try to position application to first non-primary monitor
            System.Windows.Forms.Screen secondaryScreen = null;
            foreach (var screen in System.Windows.Forms.Screen.AllScreens)
            {
                if (!screen.Primary)
                {
                    secondaryScreen = screen;
                    break;
                }
            }

            Debug.Assert(secondaryScreen != null);

            System.Drawing.Rectangle workingArea = secondaryScreen.WorkingArea;
            this.Left = workingArea.Left;
            this.Top = workingArea.Top;
            this.Width = workingArea.Width;
            this.Height = workingArea.Height;
            this.WindowState = System.Windows.WindowState.Maximized;

            //create finger points
            fingerPoints = new List<Ellipse>();
            for (int i = 0; i < NativeConstants.MAX_FINGER_NUM; i++)
            {
                Ellipse ellipse = new Ellipse()
                {
                    Width = FINGER_RADIUS * 2,
                    Height = FINGER_RADIUS * 2,
                    Fill = Brushes.Yellow,
                    Opacity = 0,
                };
                fingerPoints.Add(ellipse);
                fingerCanvas.Children.Add(ellipse);
            }

            //create hand points
            handPoints = new List<Ellipse>();
            for (int i = 0; i < NativeConstants.MAX_HAND_NUM; i++)
            {
                Ellipse ellipse = new Ellipse()
                {
                    Width = HAND_RADIUS * 2,
                    Height = HAND_RADIUS * 2,
                    Opacity = 0,
                };

                handPoints.Add(ellipse);
                handCanvas.Children.Add(ellipse);
            }

            refreshWorker = new BackgroundWorker();
            refreshWorker.DoWork += new DoWorkEventHandler(refreshWorker_DoWork);
            CompositionTarget.Rendering += new EventHandler(CompositionTarget_Rendering);
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
                //draw fingers
                unsafe
                {
                    int fingerNum;
                    ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFingers(&fingerNum);
                    Finger* fingers = (Finger*)ptr.IntPtr;

                    FloatPoint3D[] kinectPoints = new FloatPoint3D[fingerNum];
                    FloatPoint3D[] rgbPoints = new FloatPoint3D[fingerNum];

                    for (int i = 0; i < fingerNum; i++)
                    {
                        kinectPoints[i] = new FloatPoint3D()
                        {
                            x = fingers[i].PositionInKinectPersp.x,
                            y = fingers[i].PositionInKinectPersp.y,
                            z = 0
                        };
                    }

                    fixed (FloatPoint3D* kinectPointsPtr = kinectPoints, rgbPointsPtr = rgbPoints)
                    {
                        CommandDllWrapper.transformPoints(kinectPointsPtr, rgbPointsPtr, fingerNum, CalibratedCoordinateSystem.Depth2D, CalibratedCoordinateSystem.Table2D);
                    }

                    for (int i = 0; i < fingerNum; i++)
                    {
                        Canvas.SetLeft(fingerPoints[i], rgbPoints[i].x - FINGER_RADIUS);
                        Canvas.SetTop(fingerPoints[i], rgbPoints[i].y - FINGER_RADIUS);
                        fingerPoints[i].Opacity = 1.0;
                        fingerPoints[i].Fill = fingers[i].FingerType == FingerType.OmniFinger ? Brushes.Orange : Brushes.Purple;
                    }


                    for (int i = fingerNum; i < fingerPoints.Count; i++)
                    {
                        fingerPoints[i].Opacity = 0;
                    }

                    ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                }


                //draw hands
                unsafe
                {
                    int handNum;
                    ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockHands(&handNum);
                    Hand* hands = (Hand*)ptr.IntPtr;

                    FloatPoint3D[] kinectPoints = new FloatPoint3D[handNum];
                    FloatPoint3D[] projPoints = new FloatPoint3D[handNum];

                    for (int i = 0; i < handNum; i++)
                    {
                        kinectPoints[i] = new FloatPoint3D()
                        {
                            x = hands[i].PositionInKinectProj.x,
                            y = hands[i].PositionInKinectProj.y,
                            z = 0
                        };
                    }

                    fixed (FloatPoint3D* kinectPointsPtr = kinectPoints, projPointsPtr = projPoints)
                    {
                        CommandDllWrapper.transformPoints(kinectPointsPtr, projPointsPtr, handNum, CalibratedCoordinateSystem.Depth2D, CalibratedCoordinateSystem.Table2D);
                    }

                    for (int i = 0; i < handNum; i++)
                    {
                        Canvas.SetLeft(handPoints[i], projPoints[i].x - HAND_RADIUS);
                        Canvas.SetTop(handPoints[i], projPoints[i].y - HAND_RADIUS);
                        handPoints[i].Opacity = hands[i].HandType == HandType.TrackingHand ? 1.0 : 0.5;
                        handPoints[i].Fill = hands[i].HandType == HandType.NewHandHint ? Brushes.White : new SolidColorBrush(IntColorConverter.ToColor((int)hands[i].ID));
                    }

                    for (int i = handNum; i < handPoints.Count; i++)
                    {
                        handPoints[i].Opacity = 0;
                    }

                    ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                }
            }, null);
        }

        public FloatPoint3D[] ShowChessboard()
        {
            FloatPoint3D[] refCorners = new FloatPoint3D[(CHESSBOARD_ROWS - 1) * (CHESSBOARD_COLS - 1)];
            double width = chessboardCanvas.Width / CHESSBOARD_COLS;
            double height = chessboardCanvas.Height / CHESSBOARD_ROWS;

            for (int i = 0; i < CHESSBOARD_ROWS; i++)
            {
                for (int j = 0; j < CHESSBOARD_COLS; j++)
                {
                    if (i >= 1 && j >= 1)
                    {
                        Point innerCornerInCanvas = new Point(width * j, height * i);
                        Point innerCorner = chessboardCanvas.TranslatePoint(innerCornerInCanvas, globalCanvas);
                        refCorners[(i - 1) * (CHESSBOARD_COLS - 1) + (j - 1)] = new FloatPoint3D()
                        {
                            x = (float)innerCorner.X,
                            y = (float)innerCorner.Y,
                            z = 0
                        };
                    }

                    if ((i + j) % 2 != 0)
                    {
                        Rectangle rect = new Rectangle()
                        {
                            Width = width,
                            Height = height,
                            Fill = Brushes.White
                        };
                        Canvas.SetTop(rect, height * i);
                        Canvas.SetLeft(rect, width * j);
                        chessboardCanvas.Children.Add(rect);
                    }
                }
            }

            chessboardBorderCanvas.Visibility = Visibility.Visible;
            return refCorners;
        }

        public void HideChecssboard()
        {
            chessboardBorderCanvas.Visibility = Visibility.Hidden;
            chessboardCanvas.Children.Clear();
            foreach (var e in checkPointEllipses)
            {
                globalCanvas.Children.Remove(e);
            }
            checkPointEllipses.Clear();
        }

        public unsafe void DrawCheckpoints(FloatPoint3D* checkPoints, int checkPointNum)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                checkPointEllipses = new List<Ellipse>(checkPointNum);
                for (int i = 0; i < checkPointNum; i++)
                {
                    Ellipse e = new Ellipse()
                    {
                        Width = 10,
                        Height = 10,
                        Stroke = Brushes.Red,
                        StrokeThickness = 2
                    };
                    checkPointEllipses.Add(e);

                    globalCanvas.Children.Add(e);
                    Canvas.SetLeft(e, checkPoints[i].x);
                    Canvas.SetTop(e, checkPoints[i].y);
                }
            }, null);
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            CompositionTarget.Rendering -= CompositionTarget_Rendering;
        }
    }
}
