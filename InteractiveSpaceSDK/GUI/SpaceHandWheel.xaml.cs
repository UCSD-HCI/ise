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
        private const double MOVING_THRESHOLD = 20;

        private HandTracker handTracker;
        private List<Image> buttons;

        private bool isActive;
        private int capturedHandId;

        private Dictionary<int, Tuple<int, Point3D>> handUpdateRecord;
        
        public event EventHandler<HandWheelButtonEventArgs> ButtonSelected;

        public HandTracker HandTracker
        {
            get { return handTracker; }
            set
            {
                handTracker = value;
                handTracker.HandCaptured += new EventHandler<HandEventArgs>(handTracker_HandCaptured);
                handTracker.HandMove += new EventHandler<HandEventArgs>(handTracker_HandMove);
                handTracker.HandLost += new EventHandler<HandEventArgs>(handTracker_HandLost);
            }
        }

        public SpaceHandWheel()
        {
            buttons = new List<Image>();
            handUpdateRecord = new Dictionary<int, Tuple<int, Point3D>>();

            InitializeComponent();

            deactivate();
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
                if (!IsEnabled || isActive)
                {
                    return;
                }

                handUpdateRecord.Add(e.ID, new Tuple<int, Point3D>(0, e.Position));
                Trace.WriteLine("Hand " + e.ID.ToString() + " captured");
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
                    var rec = handUpdateRecord[e.ID];
                    if ((e.Position - rec.Item2).LengthSquared <= MOVING_THRESHOLD * MOVING_THRESHOLD)
                    {
                        Trace.WriteLine("Hand " + e.ID + ": " + rec.Item1.ToString());
                        handUpdateRecord[e.ID] = new Tuple<int, Point3D>(rec.Item1 + 1, e.Position);
                        if (handUpdateRecord[e.ID].Item1 >= HAND_UPDATE_COUNT)
                        {
                            activate(e.ID, e.Position);
                        }
                    }
                    else
                    {
                        handUpdateRecord[e.ID] = new Tuple<int, Point3D>(0, e.Position);
                        Trace.WriteLine("Hand " + e.ID + " reset. ");
                    }
                }
                else
                {
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

                if (isActive)
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
        }

        private void deactivate()
        {
            isActive = false;
            Visibility = Visibility.Hidden;
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
