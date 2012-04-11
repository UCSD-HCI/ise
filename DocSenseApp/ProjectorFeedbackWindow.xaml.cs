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
            SpaceProviderReady += new EventHandler(ProjectorFeedbackWindow_SpaceProviderReady);
        }

        private void SpaceCanvas_FingerDown(object sender, FingerEventArgs e)
        {
            if (grabButton.IsChecked)
            {
                if (e.FingerTracker.OnSurfaceFingers.Count() >= 2)
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
                if (e.FingerTracker.OnSurfaceFingers.Count() == 0)
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
                if (e.FingerTracker.OnSurfaceFingers.Count() >= 2)
                {
                    Finger[] fingers = new Finger[2];

                    int count = 0;
                    foreach (Finger f in e.FingerTracker.OnSurfaceFingers)
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
                else if (e.FingerTracker.OnSurfaceFingers.Count() == 1 && lastFingerUpPosition.HasValue)
                {
                    Finger f = e.FingerTracker.OnSurfaceFingers.First();
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
            handWheel.BeginInit();
            handWheel.AddButton(new BitmapImage(new Uri(@"pack://application:,,,/Images/plink.png", UriKind.RelativeOrAbsolute)));
            handWheel.AddButton(new BitmapImage(new Uri(@"pack://application:,,,/Images/evernote.png", UriKind.RelativeOrAbsolute)));
            handWheel.AddButton(new BitmapImage(new Uri(@"pack://application:,,,/Images/undo.png", UriKind.RelativeOrAbsolute)));
            handWheel.AddButton(new BitmapImage(new Uri(@"pack://application:,,,/Images/grab.png", UriKind.RelativeOrAbsolute)));
            handWheel.ButtonSelected += new EventHandler<InteractiveSpaceSDK.GUI.HandWheelButtonEventArgs>(handWheel_ButtonSelected);
            //handWheel.IsEnabled = false;
            handWheel.EndInit();
        }

        void handWheel_ButtonSelected(object sender, InteractiveSpaceSDK.GUI.HandWheelButtonEventArgs e)
        {
            switch (e.ID)
            {
                case 0:
                    gallery.OpenLink();
                    break;

                case 1:
                    gallery.UploadToEvernote();
                    break;

                case 2:
                    gallery.UndoCapture();
                    break;

                case 3:
                    grabButton.IsChecked = !grabButton.IsChecked;
                    gallery.ClearAllImages();
                    break;

            }
        }

        void ProjectorFeedbackWindow_SpaceProviderReady(object sender, EventArgs e)
        {
            handWheel.SpaceProvider = SpaceProvider;
            objectReg.SpaceProvider = SpaceProvider;
            objectTrackingLayer.SpaceProvider = SpaceProvider;
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
