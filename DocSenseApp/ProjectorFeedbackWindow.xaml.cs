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
using InteractiveSpaceSDK;
using System.Diagnostics;

namespace DocSenseApp
{
    /// <summary>
    /// Interaction logic for ProjectorFeedbackWindow.xaml
    /// </summary>
    public partial class ProjectorFeedbackWindow : InteractiveSpaceSDK.GUI.ProjectorFeedbackWindow
    {
        public ProjectorFeedbackWindow()
        {
            InitializeComponent();
        }

        private void SpaceCanvas_FingerDown(object sender, FingerEventArgs e)
        {
            if (grabButton.IsChecked)
            {
                if (e.FingerTracker.FingerCount >= 2)
                {
                    selectionRect.Opacity = 1.0;
                }
            }
            //Trace.WriteLine("FingerDown: " + e.ID.ToString() + " @ " + e.Position.ToString());
        }

        private void SpaceCanvas_FingerUp(object sender, FingerEventArgs e)
        {
            if (grabButton.IsChecked)
            {
                if (e.FingerTracker.FingerCount < 2)
                {
                    selectionRect.Opacity = 0;
                }
            }
            //Trace.WriteLine("FingerUp: " + e.ID.ToString() + " @ " + e.Position.ToString());
        }

        private void SpaceCanvas_FingerMove(object sender, FingerEventArgs e)
        {
            if (grabButton.IsChecked)
            {
                //TODO FIXME: This thread is not the same as FingerTracker's thread. How to synchronize? 
                if (e.FingerTracker.FingerCount >= 2)
                {
                    Finger[] fingers = new Finger[2];

                    int count = 0;
                    foreach (Finger f in e.FingerTracker.Fingers)
                    {
                        fingers[count] = f;
                        count++;
                        if (count >= 2)
                        {
                            break;
                        }
                    }

                    Canvas.SetLeft(selectionRect, Math.Min(fingers[0].Position.X, fingers[1].Position.X));
                    Canvas.SetTop(selectionRect, Math.Min(fingers[0].Position.Y, fingers[1].Position.Y));
                    selectionRect.Width = Math.Abs(fingers[0].Position.X - fingers[1].Position.X);
                    selectionRect.Height = Math.Abs(fingers[0].Position.Y - fingers[1].Position.Y);
                }
            }

            //Trace.WriteLine("FingerMove: " + e.ID.ToString() + " @ " + e.Position.ToString());
        }

        private void ProjectorFeedbackWindow_Loaded(object sender, RoutedEventArgs e)
        {
        }
    }
}
