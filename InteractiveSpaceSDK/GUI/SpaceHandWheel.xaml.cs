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
using System.Diagnostics;
using System.Windows.Media.Media3D;
using System.Windows.Media.Animation;

namespace InteractiveSpaceSDK.GUI
{
    /// <summary>
    /// Interaction logic for SpaceHandWheel.xaml
    /// </summary>
    public partial class SpaceHandWheel : UserControl
    {
        private const double BUTTON_SIZE = 100;
        private const double BUTTON_RADIUS = 300;
        private const double CIRCLE_RADIUS = 350;
        private const int HAND_UPDATE_COUNT = 20;
        private const double MOVING_THRESHOLD = 10;
        private const double BOTTOM_CROP = 1200;    //FIXME: 

        private InteractiveSpaceProvider spaceProvider;
        private List<Image> buttons;

        private bool isActive;
        private int capturedHandId;
        private int currentIndex;
        private Point3D wheelCenter;

        private Dictionary<int, Tuple<int, Point3D>> handUpdateRecord;

        //animation
        private ScaleTransform scaleTransform;

        
        public event EventHandler<HandWheelButtonEventArgs> ButtonSelected;

        public InteractiveSpaceProvider SpaceProvider
        {
            get { return spaceProvider; }
            set
            {
                spaceProvider = value;
                spaceProvider.HandTracker.HandCaptured += new EventHandler<HandEventArgs>(handTracker_HandCaptured);
                spaceProvider.HandTracker.HandMove += new EventHandler<HandEventArgs>(handTracker_HandMove);
                spaceProvider.HandTracker.HandLost += new EventHandler<HandEventArgs>(handTracker_HandLost);
            }
        }

        public SpaceHandWheel()
        {
            buttons = new List<Image>();
            handUpdateRecord = new Dictionary<int, Tuple<int, Point3D>>();

            InitializeComponent();

            deactivate();

            scaleTransform = new ScaleTransform(1, 1);
            RenderTransformOrigin = new Point(0.5, 0.5);
            RenderTransform = scaleTransform;
        }

        public void AddButton(ImageSource imgSrc)
        {
            Image img = new Image();
            img.BeginInit();
            img.Width = BUTTON_SIZE;
            img.Height = BUTTON_SIZE;
            img.Source = imgSrc;
            img.EndInit();

            buttons.Add(img);
            wheelCanvas.Children.Add(img);

            if (IsInitialized)
            {
                refreshButtons();
            }
        }

        public override void EndInit()
        {
            refreshButtons();
            base.EndInit();
        }

        void handTracker_HandCaptured(object sender, HandEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                if (!IsEnabled || isActive || spaceProvider.FingerTracker.FingerCount > 0)
                {
                    return;
                }

                if (e.Position.Y > BOTTOM_CROP)
                {
                    return;
                }

                handUpdateRecord.Add(e.ID, new Tuple<int, Point3D>(0, e.Position));
                //Trace.WriteLine("Hand " + e.ID.ToString() + " captured");
            }, null);
        }

        void handTracker_HandMove(object sender, HandEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                if (!IsEnabled)
                {
                    return;
                }

                if (!isActive)
                {
                    if (!handUpdateRecord.ContainsKey(e.ID))
                    {
                        return;
                    }
                    var rec = handUpdateRecord[e.ID];
                    if ((e.Position - rec.Item2).LengthSquared <= MOVING_THRESHOLD * MOVING_THRESHOLD)
                    {
                        //Trace.WriteLine("Hand " + e.ID + ": " + rec.Item1.ToString());
                        handUpdateRecord[e.ID] = new Tuple<int, Point3D>(rec.Item1 + 1, e.Position);
                        if (handUpdateRecord[e.ID].Item1 >= HAND_UPDATE_COUNT)
                        {
                            activate(e.ID, e.Position);
                        }
                    }
                    else
                    {
                        handUpdateRecord[e.ID] = new Tuple<int, Point3D>(0, e.Position);
                        //Trace.WriteLine("Hand " + e.ID + " reset. ");
                    }
                }
                else if (spaceProvider.FingerTracker.FingerCount > 0)
                {
                    deactivate();
                }
                else if (e.ID == capturedHandId)
                {
                    double halfSec = 180.0 / buttons.Count;
                    double angle = Math.Atan2(wheelCenter.Y - e.Position.Y, e.Position.X - wheelCenter.X) * 180.0 / Math.PI;
                    if (angle < 0)
                    {
                        angle += 360;
                    }
                    //Trace.WriteLine("Angle = " + angle);

                    int index = (int)((angle + halfSec) / (2 * halfSec)) % buttons.Count;

                    if (index != currentIndex)
                    {
                        double selectedAngle = -index * 2 * halfSec - halfSec;
                        selectionSector.RenderTransform = new RotateTransform(selectedAngle);
                        currentIndex = index;
                        //Trace.WriteLine("Index = " + index);
                    }

                    double ratio = (e.Position - wheelCenter).Length / CIRCLE_RADIUS;
                    selectionSector.Opacity = ratio;

                    if (ratio > 1.0)
                    {
                        //Trace.WriteLine("Index " + index + " activated!");
                        deactivate(e);
                        if (ButtonSelected != null)
                        {
                            ButtonSelected(this, new HandWheelButtonEventArgs(index));
                        }
                    }
                }
            }, null);
        }

        void handTracker_HandLost(object sender, HandEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                if (!IsEnabled)
                {
                    return;
                }

                if (isActive && e.ID == capturedHandId)
                {
                    deactivate();
                }
                else
                {
                    handUpdateRecord.Remove(e.ID);
                }
            }, null);
        }

        private void refreshButtons()
        {
            for (int i = 0; i < buttons.Count; i++)
            {
                double angle = 2 * Math.PI / buttons.Count * i;
                double x = BUTTON_RADIUS * Math.Cos(angle);
                double y = BUTTON_RADIUS * Math.Sin(angle);

                Canvas.SetLeft(buttons[i], x - (BUTTON_SIZE / 2) + wheelCanvas.RenderSize.Width / 2);
                Canvas.SetTop(buttons[i], -y - (BUTTON_SIZE / 2) + wheelCanvas.RenderSize.Height / 2);
            }

            //draw arc
            if (buttons.Count > 1)
            {
                PathGeometry pg = new PathGeometry();
                pg.FillRule = FillRule.Nonzero;

                PathFigure pf = new PathFigure();
                pf.StartPoint = new Point(0, 0);
                pf.IsClosed = true;
                pg.Figures.Add(pf);

                LineSegment ls1 = new LineSegment(new Point(CIRCLE_RADIUS, 0), true);
                pf.Segments.Add(ls1);
                 
                double rad = 2 * Math.PI / buttons.Count;
                double arcX = CIRCLE_RADIUS * Math.Cos(rad);
                double arcY = CIRCLE_RADIUS * Math.Sin(rad);
                ArcSegment arc = new ArcSegment(new Point(arcX, arcY), new Size(CIRCLE_RADIUS, CIRCLE_RADIUS), 0, false, SweepDirection.Clockwise, true);
                pf.Segments.Add(arc);

                selectionSector.Data = pg;
                selectionSector.Width = CIRCLE_RADIUS;
                selectionSector.Height = arcY;

                selectionSector.RenderTransform = new RotateTransform(-180 / buttons.Count);
            }
            else
            {
            }

        }

        private void activate(int id, Point3D initPos)
        {   
            handUpdateRecord.Clear();
            isActive = true;
            capturedHandId = id;
            Visibility = Visibility.Visible;

            Canvas.SetLeft(wheelCanvas, initPos.X - wheelCanvas.RenderSize.Width / 2);
            Canvas.SetTop(wheelCanvas, initPos.Y - wheelCanvas.RenderSize.Height / 2);
            wheelCenter = initPos;

            DoubleAnimation growAnim = new DoubleAnimation(0, 1, new Duration(TimeSpan.FromSeconds(0.25)), FillBehavior.Stop);
            scaleTransform.BeginAnimation(ScaleTransform.ScaleXProperty, growAnim);
            scaleTransform.BeginAnimation(ScaleTransform.ScaleYProperty, growAnim);
           
        }

        private void deactivate()
        {
            isActive = false;
            Visibility = Visibility.Hidden;
        }

        private void deactivate(HandEventArgs e)
        {
            deactivate();
            handUpdateRecord.Add(e.ID, new Tuple<int, Point3D>(0, e.Position));
        }
    }

    public class HandWheelButtonEventArgs : EventArgs
    {
        private int id;

        public HandWheelButtonEventArgs(int id)
        {
            this.id = id;
        }

        public int ID { get { return id; } }
    }
}
