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
using System.ComponentModel;
using ControlPanel.NativeWrappers;

namespace ControlPanel
{
    /// <summary>
    /// Interaction logic for VideoWindow.xaml
    /// </summary>
    public partial class VideoWindow : Window
    {
        public const int CHESSBOARD_ROWS = 5;
        public const int CHESSBOARD_COLS = 8;

        private const double HAND_RADIUS = 25;
        private const double FINGER_RADIUS = 5;

        //private BackgroundWorker refreshWorker;
        private VideoSourceType videoSourceType;
        private List<Ellipse> fingerPoints;
        private List<Ellipse> handPoints;

        private MainWindow mainWindow;

        public VideoWindow()
        {
            InitializeComponent();

            fingerPoints = new List<Ellipse>();
            handPoints = new List<Ellipse>();
        }

        public MainWindow MainWindow
        {
            get { return mainWindow;}
            set
            {
                mainWindow = value;
                mainWindow.EngineUpdate += new EventHandler(mainWindow_EngineUpdate);
            }
        }

        public void SetVideo(VideoSourceType videoSourceType)
        {
            this.videoSourceType = videoSourceType;

            Title = videoSourceType.ToString();

            //create finger points
            for (int i = 0; i < NativeConstants.MAX_FINGER_NUM; i++)
            {
                Ellipse ellipse = new Ellipse()
                {
                    Width = FINGER_RADIUS * 2,
                    Height = FINGER_RADIUS * 2,
                    Fill = Brushes.Orange,
                    Stroke = Brushes.White,
                    StrokeThickness = 2,
                    Opacity = 0,
                };
                fingerPoints.Add(ellipse);
                canvas.Children.Add(ellipse);
            }

            //create hand points
            for (int i = 0; i < NativeConstants.MAX_HAND_NUM; i++)
            {
                Ellipse ellipse = new Ellipse()
                {
                    Width = HAND_RADIUS * 2,
                    Height = HAND_RADIUS * 2,
                    Stroke = Brushes.White,
                    StrokeThickness = 2,
                    Opacity = 0,
                };
                handPoints.Add(ellipse);
                canvas.Children.Add(ellipse);
            }



            /*refreshWorker = new BackgroundWorker();
            refreshWorker.DoWork += new DoWorkEventHandler(refreshWorker_DoWork);
            CompositionTarget.Rendering += new EventHandler(CompositionTarget_Rendering);*/
        }


        void mainWindow_EngineUpdate(object sender, EventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate
            {
                videoImage.Source = VideoSources.SharedVideoSources.GetSource(videoSourceType);

                if (videoSourceType != VideoSourceType.MotionCamera)
                {
                    //draw fingers
                    unsafe
                    {
                        int fingerNum;
                        ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFingers(&fingerNum);
                        Finger* fingers = (Finger*)ptr.IntPtr;

                        if (videoSourceType == VideoSourceType.RGB) //transform points
                        {
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
                                CommandDllWrapper.transformPoints(kinectPointsPtr, rgbPointsPtr, fingerNum, CalibratedCoordinateSystem.Depth2D, CalibratedCoordinateSystem.RGB2D);
                            }

                            for (int i = 0; i < fingerNum; i++)
                            {
                                Canvas.SetLeft(fingerPoints[i], rgbPoints[i].x - FINGER_RADIUS);
                                Canvas.SetTop(fingerPoints[i], rgbPoints[i].y - FINGER_RADIUS);
                            }
                        }
                        else
                        {
                            for (int i = 0; i < fingerNum; i++)
                            {
                                Canvas.SetLeft(fingerPoints[i], fingers[i].PositionInKinectPersp.x);
                                Canvas.SetTop(fingerPoints[i], fingers[i].PositionInKinectPersp.y);
                            }
                        }

                        for (int i = 0; i < fingerNum; i++)
                        {
                            fingerPoints[i].Opacity = 1.0;
                            if (fingers[i].ID == 0)
                            {
                                fingerPoints[i].Fill = Brushes.Orange;
                            }
                            else
                            {
                                fingerPoints[i].Fill = new SolidColorBrush(IntColorConverter.ToColor(fingers[i].ID));
                            }

                            if (fingers[i].FingerState == FingerState.FingerOnSurface)
                            {
                                fingerPoints[i].Stroke = Brushes.White;
                            }
                            else
                            {
                                fingerPoints[i].Stroke = Brushes.Transparent;
                            }
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

                        for (int i = 0; i < handNum; i++)
                        {
                            Canvas.SetLeft(handPoints[i], hands[i].PositionInKinectProj.x - HAND_RADIUS);
                            Canvas.SetTop(handPoints[i], hands[i].PositionInKinectProj.y - HAND_RADIUS);
                            //handPoints[i].Opacity = hands[i].HandType == HandType.TrackingHand ? 1.0 : 0.5;
                            handPoints[i].Opacity = hands[i].Captured > 0 ? 1.0 : 0.0;
                            handPoints[i].Fill = hands[i].HandType == HandType.NewHandHint ? Brushes.Black : new SolidColorBrush(IntColorConverter.ToColor((int)hands[i].ID));
                        }

                        for (int i = handNum; i < handPoints.Count; i++)
                        {
                            handPoints[i].Opacity = 0;
                        }

                        ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    }
                }
            });
        }

        /*void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            if (!refreshWorker.IsBusy)
            {
                refreshWorker.RunWorkerAsync();
            }
        }*/

        /*void refreshWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate
            {
                videoImage.Source = VideoSources.SharedVideoSources.GetSource(videoSourceType);

                if (videoSourceType != VideoSourceType.MotionCamera)
                {
                    //draw fingers
                    unsafe
                    {
                        int fingerNum;
                        ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFingers(&fingerNum);
                        Finger* fingers = (Finger*)ptr.IntPtr;

                        if (videoSourceType == VideoSourceType.RGB) //transform points
                        {
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
                                CommandDllWrapper.transformPoints(kinectPointsPtr, rgbPointsPtr, fingerNum, CalibratedCoordinateSystem.Depth2D, CalibratedCoordinateSystem.RGB2D);
                            }

                            for (int i = 0; i < fingerNum; i++)
                            {
                                Canvas.SetLeft(fingerPoints[i], rgbPoints[i].x - FINGER_RADIUS);
                                Canvas.SetTop(fingerPoints[i], rgbPoints[i].y - FINGER_RADIUS);
                            }
                        }
                        else
                        {
                            for (int i = 0; i < fingerNum; i++)
                            {
                                Canvas.SetLeft(fingerPoints[i], fingers[i].PositionInKinectPersp.x);
                                Canvas.SetTop(fingerPoints[i], fingers[i].PositionInKinectPersp.y);
                            }
                        }

                        for (int i = 0; i < fingerNum; i++)
                        {
                            fingerPoints[i].Opacity = 1.0;
                            if (fingers[i].ID == 0)
                            {
                                fingerPoints[i].Fill = Brushes.Orange;
                            }
                            else
                            {
                                fingerPoints[i].Fill = new SolidColorBrush(IntColorConverter.ToColor(fingers[i].ID));
                            }
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

                        for (int i = 0; i < handNum; i++)
                        {
                            Canvas.SetLeft(handPoints[i], hands[i].PositionInKinectProj.x - HAND_RADIUS);
                            Canvas.SetTop(handPoints[i], hands[i].PositionInKinectProj.y - HAND_RADIUS);
                            //handPoints[i].Opacity = hands[i].HandType == HandType.TrackingHand ? 1.0 : 0.5;
                            handPoints[i].Opacity = hands[i].Captured > 0 ? 1.0 : 0.0;
                            handPoints[i].Fill = hands[i].HandType == HandType.NewHandHint ? Brushes.Black : new SolidColorBrush(IntColorConverter.ToColor((int)hands[i].ID));
                        }

                        for (int i = handNum; i < handPoints.Count; i++)
                        {
                            handPoints[i].Opacity = 0;
                        }

                        ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                    }
                }
            });

        }*/

        private void videoImage_MouseDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void videoImage_MouseMove(object sender, MouseEventArgs e)
        {

        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            //CompositionTarget.Rendering -= CompositionTarget_Rendering;
        }
    }
}
