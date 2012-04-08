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
using InteractiveSpaceSDK.GUI;
using InteractiveSpaceSDK;
using System.Windows.Media.Media3D;

namespace DocSenseApp
{
    /// <summary>
    /// Interaction logic for ObjectRegControl.xaml
    /// </summary>
    public partial class ObjectRegControl : UserControl, ISpacePanel
    {
        public event EventHandler<FingerEventArgs> FingerMove;
        public event EventHandler<FingerEventArgs> FingerDown;
        public event EventHandler<FingerEventArgs> FingerUp;

        public InteractiveSpaceProvider SpaceProvider { get; set; }

        public Action onRegisterFinishedDelegate;

        public ObjectRegControl()
        {
            InitializeComponent();
        }

        private void grabButton_FingerDown(object sender, InteractiveSpaceSDK.FingerEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                registerAreaRect.Fill = Brushes.White;
                onRegisterFinishedDelegate = onRegisterFinished;

                Rect bounds = registerAreaRect.RenderedGeometry.Bounds;
                Point center = registerAreaRect.TranslatePoint(new Point(registerAreaRect.RenderSize.Width / 2, registerAreaRect.RenderSize.Height / 2), Window.GetWindow(this));

                SpaceProvider.ObjectTracker.Register(new Point3D(center.X, center.Y, 0), onRegisterFinishedDelegate);
            }, null);
        }

        private void onRegisterFinished()
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                registerAreaRect.Fill = null;
            }, null);
        }

        public IEnumerable<ISpaceControl> SpaceChildren
        {
            get { return null; }
        }



        public void RaiseFingerMove(InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (FingerMove != null)
            {
                FingerMove(this, e);
            }
        }

        public void RaiseFingerDown(InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (FingerDown != null)
            {
                FingerDown(this, e);
            }
        }

        public void RaiseFingerUp(InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (FingerUp != null)
            {
                FingerUp(this, e);
            }
        }
    }
}
