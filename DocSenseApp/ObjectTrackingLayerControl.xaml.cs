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
using InteractiveSpaceSDK;
using InteractiveSpaceSDK.GUI;
using System.Windows.Media.Media3D;
using System.Windows.Media.Effects;

namespace DocSenseApp
{
    /// <summary>
    /// Interaction logic for ObjectTrackingLayerControl.xaml
    /// </summary>
    public partial class ObjectTrackingLayerControl : UserControl, ISpacePanel
    {
        private InteractiveSpaceProvider spaceProvider;

        public ObjectTrackingLayerControl()
        {
            InitializeComponent();
        }

        public IEnumerable<ISpaceControl> SpaceChildren
        {
            get
            {
                foreach (var child in canvas.Children)
                {
                    if (child is ISpaceControl)
                    {
                        yield return child as ISpaceControl;
                    }
                }
            }
        }

        public event EventHandler<FingerEventArgs> FingerMove;
        public event EventHandler<FingerEventArgs> FingerDown;
        public event EventHandler<FingerEventArgs> FingerUp;

        public void RaiseFingerMove(FingerEventArgs e)
        {
            if (FingerMove != null)
            {
                FingerMove(this, e);
            }
        }

        public void RaiseFingerDown(FingerEventArgs e)
        {
            if (FingerDown != null)
            {
                FingerDown(this, e);
            }
        }

        public void RaiseFingerUp(FingerEventArgs e)
        {
            if (FingerUp != null)
            {
                FingerUp(this, e);
            }
        }

        public InteractiveSpaceProvider SpaceProvider
        {
            get { return spaceProvider; }
            set
            {
                spaceProvider = value;
                spaceProvider.EngineUpdate += new EventHandler(spaceProvider_EngineUpdate);
            }
        }

        void spaceProvider_EngineUpdate(object sender, EventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                canvas.Children.Clear();
                foreach (var docVertices in SpaceProvider.ObjectTracker.TrackingDocumentBounds)
                {
                    //Polygon docPolygon = new Polygon();
                    //docPolygon.docPolygon.Stroke = Brushes.White;
                    //docPolygon.StrokeThickness = 4;
                    //docPolygon.Fill = new SolidColorBrush(Color.FromArgb(100, 255, 255, 255));
                    //docPolygon.Effect = docPolygonEffect;

                    //docPolygon.Points = new PointCollection();
                    //convert 3D to 2D
                    double minX = double.MaxValue, minY = double.MaxValue, maxX = double.MinValue, maxY = double.MinValue;
                    foreach (Point3D p in docVertices)
                    {
                        if (p.X < minX)
                        {
                            minX = p.X;
                        }

                        if (p.X > maxX)
                        {
                            maxX = p.X;
                        }

                        if (p.Y < minY)
                        {
                            minY = p.Y;
                        }

                        if (p.Y > maxY)
                        {
                            maxY = p.Y;
                        }
                        //docPolygon.Points.Add(new Point(p.X, p.Y));
                    }

                    Ellipse ellipse = new Ellipse();
                    ellipse.Fill = new RadialGradientBrush(Color.FromArgb(255,255,255,255), Color.FromArgb(0,255,255,255));
                    ellipse.Width = maxX - minX;
                    ellipse.Height = maxY - minY;

                    Canvas.SetTop(ellipse, minY);
                    Canvas.SetLeft(ellipse, minX);
                    //docPolygon.RenderedGeometry.Bounds

                    canvas.Children.Add(ellipse);
                }
            }, null);
        }
    }
}
