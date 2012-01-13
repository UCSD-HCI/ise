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
using System.Windows.Media.Media3D;
using everspaces;

namespace DocSenseApp
{
    /// <summary>
    /// Interaction logic for ProjectorFeedbackWindow.xaml
    /// </summary>
    public partial class ProjectorFeedbackWindow : InteractiveSpaceSDK.GUI.ProjectorFeedbackWindow
    {
        private Point3D? lastFingerUpPosition;

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
                if (e.FingerTracker.FingerCount == 0)
                {
                    if (lastFingerUpPosition.HasValue)
                    {
                        Point3D center = new Point3D((lastFingerUpPosition.Value.X + e.Position.X) / 2, (lastFingerUpPosition.Value.Y + e.Position.Y) / 2, 0);

                        Canvas.SetLeft(highlightRect, Canvas.GetLeft(selectionRect));
                        Canvas.SetTop(highlightRect, Canvas.GetTop(selectionRect));
                        highlightRect.Width = selectionRect.Width;
                        highlightRect.Height = selectionRect.Height;
                        highlightRect.Opacity = 1.0;

                        SpaceProvider.GrabAt(center, (Action)delegate()
                        {
                            Trace.WriteLine("Grabbed image at " + center.ToString());
                            //move to somewhere else!!!
                            byte[] data;
                            string mime;
                            SpaceProvider.GetLastGrabbedImageData(out data, out mime);

                            Dispatcher.BeginInvoke((Action)delegate()
                            {
                                highlightRect.Opacity = 0;
                                gallery.AddImage(data, mime);
                            }, null);
                        });
                        //grabButton.IsChecked = false;
                    }


                    selectionRect.Opacity = 0;
                    lastFingerUpPosition = null;
                }
                else
                {
                    lastFingerUpPosition = e.Position;
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
                else if (e.FingerTracker.FingerCount == 1 && lastFingerUpPosition.HasValue)
                {
                    Finger f = e.FingerTracker.Fingers.First();
                    Canvas.SetLeft(selectionRect, Math.Min(lastFingerUpPosition.Value.X, f.Position.X));
                    Canvas.SetTop(selectionRect, Math.Min(lastFingerUpPosition.Value.Y, f.Position.Y));
                    selectionRect.Width = Math.Abs(lastFingerUpPosition.Value.X - f.Position.X);
                    selectionRect.Height = Math.Abs(lastFingerUpPosition.Value.Y - f.Position.Y);
                }
            }

            //Trace.WriteLine("FingerMove: " + e.ID.ToString() + " @ " + e.Position.ToString());
        }

        private void ProjectorFeedbackWindow_Loaded(object sender, RoutedEventArgs e)
        {
        }

        private void grabButton_FingerDown(object sender, FingerEventArgs e)
        {
            gallery.ClearAllImages();
        }

        private void gallery_UploadingBegin(object sender, EventArgs e)
        {
            grabButton.IsChecked = false;
        }
    }
}
