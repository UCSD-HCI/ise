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

using AnnotoPen;
using System.Windows.Ink;
using System.Diagnostics;

namespace DocSenseApp
{
    /// <summary>
    /// Interaction logic for AnnotoPenViewer.xaml
    /// </summary>
    public partial class AnnotoPenViewer : UserControl
    {
        private AnnotoPenReceiver annotoPenReceiver;
        private Stroke currentStroke;
        //private StylusPointCollection currentPts = new StylusPointCollection();

        public AnnotoPenViewer()
        {
            InitializeComponent();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (!System.ComponentModel.DesignerProperties.GetIsInDesignMode(this))
            {
                annotoPenReceiver = AnnotoPenReceiver.NewInstance();
                if (annotoPenReceiver != null)
                {
                    annotoPenReceiver.PenDown += new EventHandler<AnnotoPenMotionEventArgs>(annotoPenReceiver_PenDown);
                    annotoPenReceiver.PenUp += new EventHandler<AnnotoPenMotionEventArgs>(annotoPenReceiver_PenUp);
                    annotoPenReceiver.PenMove += new EventHandler<AnnotoPenMotionEventArgs>(annotoPenReceiver_PenMove);
                }
            }
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            if (annotoPenReceiver != null)
            {
                annotoPenReceiver.Close();
            }
        }

        void annotoPenReceiver_PenMove(object sender, AnnotoPenMotionEventArgs e)
        {

        }

        void annotoPenReceiver_PenUp(object sender, AnnotoPenMotionEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                /*if (currentPts == null || currentPts.Count == 0)
                {
                    return;
                }

                inkCanvas.Strokes.Add(new Stroke(currentPts));
                currentPts = new StylusPointCollection();*/
                currentStroke = null;
                Trace.WriteLine("Pen Up");
            }, null);
        }

        void annotoPenReceiver_PenDown(object sender, AnnotoPenMotionEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                StylusPoint pt = new StylusPoint((e.X - 5.0) / 204.0 * 363.0, (e.Y - 5.0) / 173.0 * 309.0, e.Force / 256.0f);
                //currentPts.Add(pt);

                if (currentStroke == null)
                {
                    StylusPointCollection pts = new StylusPointCollection();
                    pts.Add(pt);

                    DrawingAttributes attr = new DrawingAttributes()
                    {
                        Color = Colors.White, 
                    };

                    currentStroke = new Stroke(pts, attr);
                    inkCanvas.Strokes.Add(currentStroke);
                }
                else
                {
                    currentStroke.StylusPoints.Add(pt);
                }
                //Trace.WriteLine("Pen Down");
            }, null);
        }
    }
}
