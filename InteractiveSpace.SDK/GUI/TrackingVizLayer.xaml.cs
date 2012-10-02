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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Media.Media3D;
using InteractiveSpace.EngineController.NativeWrappers;
using InteractiveSpace.EngineController;

namespace InteractiveSpace.SDK.GUI
{
    /// <summary>
    /// Interaction logic for TrackingVizLayer.xaml
    /// </summary>
    public partial class TrackingVizLayer : UserControl
    {

        private const double HAND_RADIUS = 50;
        private const double FINGER_RADIUS = 10;

        private List<Ellipse> handPoints;
        private List<Ellipse> fingerPoints;

        private InteractiveSpaceProvider spaceProvider;

        public bool IsDocVisible { get; set; }
        public bool IsFingerVisible { get; set; }
        public bool IsHandVisible { get; set; }

        public TrackingVizLayer()
        {
            InitializeComponent();

            IsDocVisible = true;
            IsFingerVisible = true;
            IsHandVisible = true;
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
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
                fingerLayer.Children.Add(ellipse);
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
                handLayer.Children.Add(ellipse);
            }
        }

        public InteractiveSpaceProvider SpaceProvider
        {
            get
            {
                return spaceProvider;
            }

            set
            {
                spaceProvider = value;
                spaceProvider.EngineUpdate += new EventHandler(spaceProvider_EngineUpdate);
            }
        }

        private void spaceProvider_EngineUpdate(object sender, EventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                drawHands();
                drawFingers();
            }, null);
        }

        private void drawHands()
        {
            if (SpaceProvider.HandTracker == null || handPoints == null || !IsHandVisible)
            {
                foreach (var handPoint in handPoints)
                {
                    handPoint.Opacity = 0;
                }
                return;
            }

            int i = 0;
            foreach (var hand in SpaceProvider.HandTracker.Hands)
            {
                Canvas.SetLeft(handPoints[i], hand.Position.X - HAND_RADIUS);
                Canvas.SetTop(handPoints[i], hand.Position.Y - HAND_RADIUS);
                //handPoints[i].Opacity = hands[i].HandType == HandType.TrackingHand ? 1.0 : 0.5;
                handPoints[i].Opacity = 1.0;
                handPoints[i].Fill = new SolidColorBrush(IntColorConverter.ToColor(hand.ID));
                i++;

                if (i >= NativeConstants.MAX_HAND_NUM)
                {
                    break;  //due to some synchronization issues
                }
            }

            for (; i < handPoints.Count; i++)
            {
                handPoints[i].Opacity = 0;
            }

        }

        private void drawFingers()
        {
            if (SpaceProvider.FingerTracker == null || fingerPoints == null || !IsFingerVisible)
            {
                foreach (var fingerPoint in fingerPoints)
                {
                    fingerPoint.Opacity = 0;
                }
                return;
            }

            int i = 0;
            foreach (var finger in SpaceProvider.FingerTracker.Fingers)
            {
                Canvas.SetLeft(fingerPoints[i], finger.Position.X - FINGER_RADIUS);
                Canvas.SetTop(fingerPoints[i], finger.Position.Y - FINGER_RADIUS);
                fingerPoints[i].Opacity = 1.0;

                if (finger.ID == 0)
                {
                    fingerPoints[i].Fill = Brushes.Orange;
                }
                else
                {
                    fingerPoints[i].Fill = new SolidColorBrush(IntColorConverter.ToColor(finger.ID));
                }

                if (finger.FingerState == FingerState.OnSurface)
                {
                    fingerPoints[i].Stroke = Brushes.White;
                }
                else
                {
                    fingerPoints[i].Stroke = Brushes.Transparent;
                }

                i++;

                if (i >= NativeConstants.MAX_FINGER_NUM)
                {
                    break;  //due to some synchronization issues
                }
            }

            for (; i < fingerPoints.Count; i++)
            {
                fingerPoints[i].Opacity = 0;
            }
        }


    }
}
