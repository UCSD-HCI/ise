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
        private Point roiCorner1, roiCorner2;

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
            if (llahRoiBtn.IsChecked == true)
            {
                roiCorner1 = e.GetPosition(webcamImage);
                isCropDragging = true;
                webcamImage.CaptureMouse();
            }
        }

        private void videoImage_MouseMove(object sender, MouseEventArgs e)
        {
            if (isCropDragging)
            {
                roiCorner2 = e.GetPosition(webcamImage);

                roiCorner2.X = Math.Max(0, Math.Min(webcamImage.Width, roiCorner2.X));
                roiCorner2.Y = Math.Max(0, Math.Min(webcamImage.Height, roiCorner2.Y));

                Canvas.SetLeft(LLAHRoi, Math.Min(roiCorner1.X, roiCorner2.X));
                Canvas.SetTop(LLAHRoi, Math.Min(roiCorner1.Y, roiCorner2.Y));
                LLAHRoi.Width = Math.Abs(roiCorner1.X - roiCorner2.X);
                LLAHRoi.Height = Math.Abs(roiCorner1.Y - roiCorner2.Y);
                updateDimensionLabel();
                
            }
        }

        private void videoImage_MouseUp(object sender, MouseButtonEventArgs e)
        {
            isCropDragging = false;
            webcamImage.ReleaseMouseCapture();

            int left = (int)Canvas.GetLeft(LLAHRoi);
            int top = (int)Canvas.GetTop(LLAHRoi);
            int width = (int)(LLAHRoi.Width);
            int height = (int)(LLAHRoi.Height);

            unsafe
            {
                statusLabel.Content = CommandDllWrapper.setLLAHRoi(left, top, width, height);
            }

            llahRoiBtn.IsChecked = false;
        }
        private void setROIButton_Checked(object sender, RoutedEventArgs e)
        {
            if (llahRoiBtn.IsChecked == true)
            {
                LLAHRoi.Width = 0;
                LLAHRoi.Height = 0;
                LLAHDimensionTbx.Text = "";
                isCropDragging = false;
            }
        }

        private void updateDimensionLabel()
        {
            Canvas.SetLeft(LLAHDimensionTbx,  Canvas.GetLeft(LLAHRoi) + 2);
            Canvas.SetTop(LLAHDimensionTbx,  Canvas.GetTop(LLAHRoi) + 2);
            LLAHDimensionTbx.Text = Canvas.GetLeft(LLAHRoi) + ", " + Canvas.GetTop(LLAHRoi) + ", " + LLAHRoi.Width + ", " + LLAHRoi.Height;
        }

    }
}
