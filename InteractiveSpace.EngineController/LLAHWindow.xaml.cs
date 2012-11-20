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
using InteractiveSpace.EngineController.NativeWrappers;

namespace InteractiveSpace.EngineController
{
    /// <summary>
    /// Interaction logic for LLAHWindow.xaml
    /// </summary>
    public partial class LLAHWindow : Window
    {
        private bool isCropDragging;
        private Point cropCorner1, cropCorner2;

        private MainWindow mainWindow;

        public LLAHWindow()
        {
            InitializeComponent();
        }
        public MainWindow MainWindow
        {
            get { return mainWindow; }
            set
            {
                mainWindow = value;
                mainWindow.EngineUpdate += new EventHandler(mainWindow_EngineUpdate);
            }
        }
        void mainWindow_EngineUpdate(object sender, EventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate
            {
                if (mainWindow.IsStopRequested)
                {
                    return;
                }

                webcamImage.Source = VideoSources.SharedVideoSources.GetSource(VideoSourceType.Webcam);
            });

        }

        private void videoImage_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (llahCropButton.IsChecked == true)
            {
                cropCorner1 = e.GetPosition(webcamImage);
                isCropDragging = true;
                webcamImage.CaptureMouse();
            }
        }

        private void videoImage_MouseMove(object sender, MouseEventArgs e)
        {
            if (isCropDragging)
            {
                cropCorner2 = e.GetPosition(webcamImage);

                cropCorner2.X = Math.Max(0, Math.Min(webcamImage.Width, cropCorner2.X));
                cropCorner2.Y = Math.Max(0, Math.Min(webcamImage.Height, cropCorner2.Y));

                Canvas.SetLeft(llahCropRect, Math.Min(cropCorner1.X, cropCorner2.X));
                Canvas.SetTop(llahCropRect, Math.Min(cropCorner1.Y, cropCorner2.Y));
                llahCropRect.Width = Math.Abs(cropCorner1.X - cropCorner2.X);
                llahCropRect.Height = Math.Abs(cropCorner1.Y - cropCorner2.Y);
            }
        }

        private void videoImage_MouseUp(object sender, MouseButtonEventArgs e)
        {
            isCropDragging = false;
            webcamImage.ReleaseMouseCapture();

            int left = (int)Canvas.GetLeft(llahCropRect);
            int top = (int)Canvas.GetTop(llahCropRect);
            int width = (int)(llahCropRect.Width);
            int height = (int)(llahCropRect.Height);

            unsafe
            {
                statusLabel.Content = CommandDllWrapper.getLLAHInfo(left, top, width, height);
            }

            llahCropButton.IsChecked = false;
        }
        private void llahCropButton_Checked(object sender, RoutedEventArgs e)
        {
            if (llahCropButton.IsChecked == true)
            {
                llahCropRect.Width = 0;
                llahCropRect.Height = 0;
                isCropDragging = false;
            }
        }

    }
}
