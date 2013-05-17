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
using System.Runtime.InteropServices;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Diagnostics;
using System.IO.MemoryMappedFiles;
using System.IO;

namespace InteractiveSpace.EngineController
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private VideoWindow rawVideoWindow, depthVideoWindow, multiTouchVideoWindow, thresholdTouchVideoWindow, motionCameraVideoWindow, objTrackingVideoWindow, rectifiedTabletopVideoWindow;
        //private ProjectorFeedbackWindow projectorFeedbackWindow;
        private bool isSlidersValueLoaded;
        private System.Threading.Timer fpsTimer;
        private Action engineUpdateDelegate;
        private Action engineStoppedDelegate;
        private bool isRecording;
        private SharedMemoryExporter smExporter;
        private MultiTouchVistaController multiTouchVistaController;

        public MainWindow()
        {
            InitializeComponent();
            smExporter = new SharedMemoryExporter();
        }

        public void Exit()
        {
            if (rawVideoWindow != null)
            {
                rawVideoWindow.Close();
            }

            if (depthVideoWindow != null)
            {
                depthVideoWindow.Close();
            }

            if (multiTouchVideoWindow != null)
            {
                multiTouchVideoWindow.Close();
            }

            if (thresholdTouchVideoWindow != null)
            {
                thresholdTouchVideoWindow.Close();
            }

            /*if (projectorFeedbackWindow != null)
            {
                projectorFeedbackWindow.Close();
            }*/

            if (motionCameraVideoWindow != null)
            {
                motionCameraVideoWindow.Close();
            }

            multiTouchVistaController.StopServer();
            EngineBackgroundWorker.Instance.Stop();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var settings = Properties.Settings.Default;

            fingerMinWidthSlider.Value = settings.FingerMinWidth;
            fingerMaxWidthSlider.Value = settings.FingerMaxWidth;
            fingerMinLengthSlider.Value = settings.FingerMinLength;
            fingerMaxLengthSlider.Value = settings.FingerMaxLength;
            fingerRisingThresholdSlider.Value = settings.FingerRisingThreshold;
            fingerFallingThresholdSlider.Value = settings.FingerFallingThreshold;
            clickFloodMaxGradSlider.Value = settings.ClickFloodMaxGrad;

            isSlidersValueLoaded = true;

            //projectorFeedbackWindow = new ProjectorFeedbackWindow();

            multiTouchVistaController = new MultiTouchVistaController();

            string multiTouchVistaPath = @"..\..\MultiTouchVista\Output";
            if (!Directory.Exists(multiTouchVistaPath))
            {
                multiTouchVistaPath = @".\";
            }

            try
            {
                multiTouchVistaController.StartServer(multiTouchVistaPath);
            }
            catch (FileNotFoundException ex)
            {
                MessageBox.Show(ex.Message);
            }

            EngineBackgroundWorker.Instance.EngineUpdate += new EventHandler(engineUpdate);
            EngineBackgroundWorker.Instance.Run();

            EngineBackgroundWorker.Instance.WaitForInit();

            NativeWrappers.CommandDllWrapper.setOmniTouchParameters(fingerMinWidthSlider.Value, fingerMaxWidthSlider.Value, fingerMinLengthSlider.Value, fingerMaxLengthSlider.Value,
                fingerRisingThresholdSlider.Value, fingerFallingThresholdSlider.Value, clickFloodMaxGradSlider.Value);
           
            //engineUpdateDelegate = new Action(engineUpdateCallback);
            //NativeWrappers.CommandDllWrapper.registerEngineUpdateCallback(Marshal.GetFunctionPointerForDelegate(engineUpdateDelegate));

            //omni cropping
            NativeWrappers.CommandDllWrapper.setOmniTouchCropping(settings.OmniTouchCroppingLeft, settings.OmniTouchCroppingTop, settings.OmniTouchCroppingRight, settings.OmniTouchCroppingBottom);

            fpsTimer = new System.Threading.Timer(new System.Threading.TimerCallback(fpsTimer_Tick), null, 0, 1000);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Exit();
        }

        void engineUpdate(object sender, EventArgs e)
        {
            smExporter.Update();
        }

        private void rawVideoToggleButton_Click(object sender, RoutedEventArgs e)
        {
            handleVideoToggleButtonClick(rawVideoToggleButton, ref rawVideoWindow, VideoSourceType.RGB);
        }

        private void depthVideoToggleButton_Click(object sender, RoutedEventArgs e)
        {
            handleVideoToggleButtonClick(depthVideoToggleButton, ref depthVideoWindow, VideoSourceType.DepthHistogramed);
        }

        private void multiTouchVideoToggleButton_Click(object sender, RoutedEventArgs e)
        {
            handleVideoToggleButtonClick(multiTouchVideoToggleButton, ref multiTouchVideoWindow, VideoSourceType.OmniTouch);
        }

         private void rectifiedTabletopToggleButton_Click(object sender, RoutedEventArgs e)
        {
            handleVideoToggleButtonClick(rectifiedTabletopToggleButton, ref rectifiedTabletopVideoWindow, VideoSourceType.RectifiedTabletopProduct);
        }

        private void handleVideoToggleButtonClick(System.Windows.Controls.Primitives.ToggleButton button, ref VideoWindow window, VideoSourceType videoType)
        {
            if (button.IsChecked.GetValueOrDefault(false))
            {
                if (window != null)
                {
                    window.Close();
                }

                window = new VideoWindow();
                window.SetVideo(videoType);
                window.Closed += delegate(object cSender, EventArgs cArgs)
                {
                    button.IsChecked = false;
                };
                window.Show();
            }
            else
            {
                if (window != null)
                {
                    window.Close();
                    window = null;
                }
            }
        }

        private void omniSliders_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            e.Handled = true;

            if (sender == fingerMinWidthSlider)
            {
                if (fingerMaxWidthSlider.Value < e.NewValue - double.Epsilon)
                {
                    fingerMaxWidthSlider.Value = e.NewValue;
                }

                fingerMinWidthTextBox.Text = e.NewValue.ToString("0.00");
            }
            else if (sender == fingerMaxWidthSlider)
            {
                if (e.NewValue < fingerMinWidthSlider.Value - double.Epsilon)
                {
                    e.Handled = false;
                    fingerMaxWidthSlider.Value = fingerMinWidthSlider.Value;
                }
                else
                {
                    fingerMaxWidthTextBox.Text = e.NewValue.ToString("0.00");
                }
            }
            else if (sender == fingerMinLengthSlider)
            {
                if (fingerMaxLengthSlider.Value < e.NewValue - double.Epsilon)
                {
                    fingerMaxLengthSlider.Value = e.NewValue;
                }

                fingerMinLengthTextBox.Text = e.NewValue.ToString("0.00");
            }
            else if (sender == fingerMaxLengthSlider)
            {
                if (e.NewValue < fingerMinLengthSlider.Value - double.Epsilon)
                {
                    e.Handled = false;
                    fingerMaxLengthSlider.Value = fingerMinLengthSlider.Value;
                }
                else
                {
                    fingerMaxLengthTextBox.Text = e.NewValue.ToString("0.00");
                }
            }
            else if (sender == fingerRisingThresholdSlider)
            {
                if (fingerRisingThresholdTextBox != null)
                {
                    fingerRisingThresholdTextBox.Text = e.NewValue.ToString("0.00");
                }
            }
            else if (sender == fingerFallingThresholdSlider)
            {
                if (fingerFallingThresholdTextBox != null)
                {
                    fingerFallingThresholdTextBox.Text = e.NewValue.ToString("0.00");
                }
            }
            else if (sender == clickFloodMaxGradSlider)
            {
                clickFloodMaxGradTextBox.Text = e.NewValue.ToString("0.00");
            }

            if (e.Handled && isSlidersValueLoaded)
            {
                Properties.Settings.Default.FingerMinWidth = fingerMinWidthSlider.Value;
                Properties.Settings.Default.FingerMaxWidth = fingerMaxWidthSlider.Value;
                Properties.Settings.Default.FingerMinLength = fingerMinLengthSlider.Value;
                Properties.Settings.Default.FingerMaxLength = fingerMaxLengthSlider.Value;
                Properties.Settings.Default.FingerRisingThreshold = fingerRisingThresholdSlider.Value;
                Properties.Settings.Default.FingerFallingThreshold = fingerFallingThresholdSlider.Value;
                Properties.Settings.Default.ClickFloodMaxGrad = clickFloodMaxGradSlider.Value;
                
                Properties.Settings.Default.Save();

                NativeWrappers.CommandDllWrapper.setOmniTouchParameters(fingerMinWidthSlider.Value, fingerMaxWidthSlider.Value, fingerMinLengthSlider.Value, fingerMaxLengthSlider.Value,
                    fingerRisingThresholdSlider.Value, fingerFallingThresholdSlider.Value, clickFloodMaxGradSlider.Value);
            }
        }

        private void systemCalibrateButton_Click(object sender, RoutedEventArgs e)
        {
            CalibrationWindow caliWin = new CalibrationWindow();
            //caliWin.ProjectorFeedbackWindow = projectorFeedbackWindow;
            caliWin.Show();
        }

        private void fpsTimer_Tick(object state)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                fpsLabel.Content = NativeWrappers.ResultsDllWrapper.getFPS().ToString("0.00");
            }, null);
        }

        private void recordButton_Click(object sender, RoutedEventArgs e)
        {
            if (!isRecording)
            {
                Microsoft.Win32.SaveFileDialog dialog = new Microsoft.Win32.SaveFileDialog();
                dialog.FileName = "rec" + String.Format("{0:yyMMdd-HHmm}", DateTime.Now);
                dialog.DefaultExt = ".avi";
                dialog.Filter = "Video (.avi)|*.avi";

                bool? result = dialog.ShowDialog();

                if (result == true)
                {
                    string path = dialog.FileName;
                    string pathNoExt = System.IO.Path.GetDirectoryName(path) + "\\" + System.IO.Path.GetFileNameWithoutExtension(path);
                    byte[] pathBytes = Encoding.UTF8.GetBytes(pathNoExt);
                    byte[] cstr = new byte[pathBytes.Length + 1];
                    for (int i = 0; i < pathBytes.Length; i++)
                    {
                        cstr[i] = pathBytes[i];
                    }
                    cstr[cstr.Length - 1] = (byte)0;

                    unsafe
                    {
                        fixed (byte* cstrPtr = cstr)
                        {
                            CommandDllWrapper.startRecording(cstrPtr);
                        }
                    }

                    isRecording = true;
                    recordButton.Content = "Stop";
                }
            }
            else
            {
                CommandDllWrapper.stopRecording();
                isRecording = false;
                recordButton.Content = "Record";
            }
        }

        private void captureButton_Click(object sender, RoutedEventArgs e)
        {
            FrameCapture fc = new FrameCapture();
            fc.Save();
        }

        private void omniTouchCheckBox_Click(object sender, RoutedEventArgs e)
        {
            CommandDllWrapper.setOmniTouchEnabled(omniTouchCheckBox.IsChecked.Value ? 1 : 0);
        }

        private void rectifiedTabletopCheckBox_Click(object sender, RoutedEventArgs e)
        {
            CommandDllWrapper.setTabletopRectifiedEnabled(rectifiedTabletopCheckBox.IsChecked.Value ? 1 : 0);
        }

        
    }
}
