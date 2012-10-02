using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Diagnostics;
using System.Windows.Media;

namespace InteractiveSpace.SDK.GUI
{
    public class ProjectorFeedbackWindow : Window
    {
        private InteractiveSpaceProvider provider;

        public event EventHandler SpaceProviderReady;

        public ProjectorFeedbackWindow()
        {
            WindowStyle = WindowStyle.None;
            Background = Brushes.Purple;
            BorderBrush = Brushes.Gray;
            BorderThickness = new Thickness(10);
            Loaded += new RoutedEventHandler(ProjectFeedbackWindow_Loaded);
        }

        public InteractiveSpaceProvider SpaceProvider
        {
            get { return provider; }
            set
            {
                if (provider != null)
                {
                    FingerTracker fingerTracker = provider.FingerTracker;
                    if (fingerTracker != null)
                    {
                        fingerTracker.FingerMove -= fingerTracker_FingerMove;
                        fingerTracker.FingerDown -= fingerTracker_FingerDown;
                        fingerTracker.FingerUp -= fingerTracker_FingerUp;
                    }
                }

                provider = value;

                FingerTracker newFingerTracker = provider.FingerTracker;
                if (newFingerTracker != null)
                {
                    newFingerTracker.FingerMove += new EventHandler<FingerEventArgs>(fingerTracker_FingerMove);
                    newFingerTracker.FingerDown += new EventHandler<FingerEventArgs>(fingerTracker_FingerDown);
                    newFingerTracker.FingerUp += new EventHandler<FingerEventArgs>(fingerTracker_FingerUp);
                }

                if (SpaceProviderReady != null)
                {
                    SpaceProviderReady(this, EventArgs.Empty);
                }
            }
        }

        void fingerTracker_FingerMove(object sender, FingerEventArgs e)
        {
            handleFingerEvent(FingerEventType.FingerMove, e);
        }

        void fingerTracker_FingerUp(object sender, FingerEventArgs e)
        {
            handleFingerEvent(FingerEventType.FingerUp, e);
        }

        void fingerTracker_FingerDown(object sender, FingerEventArgs e)
        {
            handleFingerEvent(FingerEventType.FingerDown, e);
        }

        private void handleFingerEvent(FingerEventType type, FingerEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                VisualTreeHelper.HitTest(this,
                    null,
                    (HitTestResultCallback)delegate(HitTestResult result)
                    {
                        //find ISpaceControl parent
                        DependencyObject current = result.VisualHit;
                        while(current != null && !(current is ISpaceControl))
                        {
                            current = VisualTreeHelper.GetParent(current);
                        }

                        if (current != null)
                        {
                            ISpaceControl spaceControl = current as ISpaceControl;
                            switch(type)
                            {
                                case FingerEventType.FingerDown:
                                    spaceControl.RaiseFingerDown(e);
                                    break;

                                case FingerEventType.FingerUp:
                                    spaceControl.RaiseFingerUp(e);
                                    break;

                                case FingerEventType.FingerMove:
                                    spaceControl.RaiseFingerMove(e);
                                    break;
                            }

                            return HitTestResultBehavior.Stop;
                        }
                        else
                        {
                            return HitTestResultBehavior.Continue;
                        }
                    }, 
                    new PointHitTestParameters(new Point(e.Position.X, e.Position.Y)));
            }, null);
        }

        private void ProjectFeedbackWindow_Loaded(object sender, RoutedEventArgs e)
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

            //Debug.Assert(secondaryScreen != null);

            System.Drawing.Rectangle workingArea = secondaryScreen != null ? secondaryScreen.WorkingArea : System.Windows.Forms.Screen.PrimaryScreen.WorkingArea;
            this.Left = workingArea.Left;
            this.Top = workingArea.Top;
            this.Width = workingArea.Width;
            this.Height = workingArea.Height;
            this.WindowState = System.Windows.WindowState.Maximized;
        }

        private enum FingerEventType
        {
            FingerMove,
            FingerUp,
            FingerDown,
        }
    }
}
