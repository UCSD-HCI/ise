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
        private BackgroundWorker refreshWorker;
        private VideoSourceType videoSourceType;
        private List<Ellipse> fingerPoints;
        private List<Ellipse> handPoints;

        public VideoWindow()
        {
            InitializeComponent();

            fingerPoints = new List<Ellipse>();
            handPoints = new List<Ellipse>();
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
                    Width = 10,
                    Height = 10,
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
                    Width = 50,
                    Height = 50,
                    Stroke = Brushes.White,
                    StrokeThickness = 2,
                    Opacity = 0,
                };
                handPoints.Add(ellipse);
                canvas.Children.Add(ellipse);
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
                videoImage.Source = VideoSources.SharedVideoSources.GetSource(videoSourceType);

                //draw fingers
                unsafe
                {
                    int fingerNum;
                    ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockFingers(&fingerNum);
                    Finger* fingers = (Finger*)ptr.IntPtr;

                    for (int i = 0; i < fingerNum; i++)
                    {
                        Canvas.SetLeft(fingerPoints[i], fingers[i].PositionInKinectPersp.x);
                        Canvas.SetTop(fingerPoints[i], fingers[i].PositionInKinectPersp.y);
                        fingerPoints[i].Opacity = 1.0;
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
                        Canvas.SetLeft(handPoints[i], hands[i].PositionInKinectProj.x);
                        Canvas.SetTop(handPoints[i], hands[i].PositionInKinectProj.y);
                        handPoints[i].Opacity = hands[i].HandType == HandType.TrackingHand ? 1.0 : 0.5;
                        handPoints[i].Fill = new SolidColorBrush(IntColorConverter.ToColor((int)hands[i].ID));
                    }

                    for (int i = handNum; i < handPoints.Count; i++)
                    {
                        handPoints[i].Opacity = 0;
                    }

                    ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);
                }
            });
        }

        private void videoImage_MouseDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void videoImage_MouseMove(object sender, MouseEventArgs e)
        {

        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            CompositionTarget.Rendering -= CompositionTarget_Rendering;
        }
    }
}
