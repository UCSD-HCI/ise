using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Diagnostics;
using System.Windows.Media;
using InteractiveSpaceSDK;

namespace InteractiveSpaceWPFControls
{
    public class ProjectorFeedbackWindow : Window
    {
        private InteractiveSpaceProvider provider;

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
            }
        }

        void fingerTracker_FingerMove(object sender, FingerEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                VisualTreeHelper.HitTest(this, null, (HitTestResultCallback)delegate(HitTestResult result)
                {
                    if (result.VisualHit is ISpaceControl)
                    {
                        ISpaceControl spaceControl = result.VisualHit as ISpaceControl;
                        spaceControl.RaiseFingerMove(e);
                        return HitTestResultBehavior.Stop;
                    }
                    else
                    {
                        return HitTestResultBehavior.Continue;
                    }
                }, new PointHitTestParameters(new Point(e.Position.X, e.Position.Y)));
            }, null);
        }

        void fingerTracker_FingerUp(object sender, FingerEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                VisualTreeHelper.HitTest(this, null, (HitTestResultCallback)delegate(HitTestResult result)
                {
                    if (result.VisualHit is ISpaceControl)
                    {
                        ISpaceControl spaceControl = result.VisualHit as ISpaceControl;
                        spaceControl.RaiseFingerUp(e);
                        return HitTestResultBehavior.Stop;
                    }
                    else
                    {
                        return HitTestResultBehavior.Continue;
                    }
                }, new PointHitTestParameters(new Point(e.Position.X, e.Position.Y)));
            }, null);
        }

        void fingerTracker_FingerDown(object sender, FingerEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                VisualTreeHelper.HitTest(this, null, (HitTestResultCallback)delegate(HitTestResult result)
                {
                    if (result.VisualHit is ISpaceControl)
                    {
                        ISpaceControl spaceControl = result.VisualHit as ISpaceControl;
                        spaceControl.RaiseFingerDown(e);
                        return HitTestResultBehavior.Stop;
                    }
                    else
                    {
                        return HitTestResultBehavior.Continue;
                    }
                }, new PointHitTestParameters(new Point(e.Position.X, e.Position.Y)));
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

            Debug.Assert(secondaryScreen != null);

            System.Drawing.Rectangle workingArea = secondaryScreen.WorkingArea;
            this.Left = workingArea.Left;
            this.Top = workingArea.Top;
            this.Width = workingArea.Width;
            this.Height = workingArea.Height;
            this.WindowState = System.Windows.WindowState.Maximized;
        }
    }
}
