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
using ControlPanel.NativeWrappers;
using System.Diagnostics;

namespace ControlPanel
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private VideoWindow rawVideoWindow, depthVideoWindow, multiTouchVideoWindow, thresholdTouchVideoWindow, motionCameraVideoWindow;
        //private ProjectorFeedbackWindow projectorFeedbackWindow;
        private bool isSlidersValueLoaded;
        private Action thresholdCalibrationFinishedCallback;
        private System.Threading.Timer fpsTimer;
        private Action engineUpdateDelegate;
        private Action engineStoppedDelegate;
        public bool IsStopRequested { get; private set; }
        private bool isRecording;

        public event EventHandler EngineUpdate;

        public MainWindow()
        {
            InitializeComponent();
        }

        public void EngineStop()
        {
            IsStopRequested = true;
            engineStoppedDelegate = new Action(onEngineStopped);
            CommandDllWrapper.engineStop(Marshal.GetFunctionPointerForDelegate(engineStoppedDelegate));
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            fingerMinWidthSlider.Value = Properties.Settings.Default.FingerMinWidth;
            fingerMaxWidthSlider.Value = Properties.Settings.Default.FingerMaxWidth;
            fingerMinLengthSlider.Value = Properties.Settings.Default.FingerMinLength;
            fingerMaxLengthSlider.Value = Properties.Settings.Default.FingerMaxLength;

            noiseThresholdSlider.Value = Properties.Settings.Default.NoiseThreshold;
            fingerThresholdSlider.Value = Properties.Settings.Default.FingerThreshold;
            blindThresholdSlider.Value = Properties.Settings.Default.BlindThreshold;

            isSlidersValueLoaded = true;

            //projectorFeedbackWindow = new ProjectorFeedbackWindow();

            CommandDllWrapper.engineRun();
            NativeWrappers.CommandDllWrapper.setOmniTouchParameters(fingerMinWidthSlider.Value, fingerMaxWidthSlider.Value, fingerMinLengthSlider.Value, fingerMaxLengthSlider.Value);
            NativeWrappers.CommandDllWrapper.setThresholdTouchParameters(noiseThresholdSlider.Value, fingerThresholdSlider.Value, blindThresholdSlider.Value);

            engineUpdateDelegate = new Action(engineUpdateCallback);
            NativeWrappers.CommandDllWrapper.registerEngineUpdateCallback(Marshal.GetFunctionPointerForDelegate(engineUpdateDelegate));

            //projectorFeedbackWindow.Show();

            fpsTimer = new System.Threading.Timer(new System.Threading.TimerCallback(fpsTimer_Tick), null, 0, 1000);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
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
        }

        private void onEngineStopped()
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                this.Close();
            }, null);
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

        private void thresholdTouchVideoToggleButton_Click(object sender, RoutedEventArgs e)
        {
            handleVideoToggleButtonClick(thresholdTouchVideoToggleButton, ref thresholdTouchVideoWindow, VideoSourceType.ThresholdTouch);
        }

        private void motionCameraVideoToggleButton_Click(object sender, RoutedEventArgs e)
        {
            handleVideoToggleButtonClick(motionCameraVideoToggleButton, ref motionCameraVideoWindow, VideoSourceType.MotionCamera);
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
                window.MainWindow = this;
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

            if (e.Handled && isSlidersValueLoaded)
            {
                Properties.Settings.Default.FingerMinWidth = fingerMinWidthSlider.Value;
                Properties.Settings.Default.FingerMaxWidth = fingerMaxWidthSlider.Value;
                Properties.Settings.Default.FingerMinLength = fingerMinLengthSlider.Value;
                Properties.Settings.Default.FingerMaxLength = fingerMaxLengthSlider.Value;
                Properties.Settings.Default.Save();

                NativeWrappers.CommandDllWrapper.setOmniTouchParameters(fingerMinWidthSlider.Value, fingerMaxWidthSlider.Value, fingerMinLengthSlider.Value, fingerMaxLengthSlider.Value);
            }
        }

        private void thresholdSliders_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            e.Handled = true;

            if (sender == noiseThresholdSlider)
            {
                if (fingerThresholdSlider.Value < e.NewValue - double.Epsilon)
                {
                    fingerThresholdSlider.Value = e.NewValue;
                }

                if (blindThresholdSlider.Value < e.NewValue - double.Epsilon)
                {
                    blindThresholdSlider.Value = e.NewValue;
                }

                noiseThresholdTextBox.Text = e.NewValue.ToString("0.00");
            }
            else if (sender == fingerThresholdSlider)
            {
                if (e.NewValue < noiseThresholdSlider.Value - double.Epsilon)
                {
                    e.Handled = false;
                    fingerThresholdSlider.Value = noiseThresholdSlider.Value;
                }
                else
                {
                    if (blindThresholdSlider.Value < e.NewValue - double.Epsilon)
                    {
                        blindThresholdSlider.Value = e.NewValue;
                    }

                    fingerThresholdTextBox.Text = e.NewValue.ToString("0.00");
                }
            }
            else if (sender == blindThresholdSlider)
            {
                if (e.NewValue < fingerThresholdSlider.Value - double.Epsilon)
                {
                    e.Handled = false;
                    blindThresholdSlider.Value = fingerThresholdSlider.Value;
                }
                else
                {
                    blindThresholdTextBox.Text = e.NewValue.ToString("0.00");
                }
            }

            if (e.Handled && isSlidersValueLoaded)
            {
                Properties.Settings.Default.NoiseThreshold = noiseThresholdSlider.Value;
                Properties.Settings.Default.FingerThreshold = fingerThresholdSlider.Value;
                Properties.Settings.Default.BlindThreshold = blindThresholdSlider.Value;
                Properties.Settings.Default.Save();

                NativeWrappers.CommandDllWrapper.setThresholdTouchParameters(noiseThresholdSlider.Value, fingerThresholdSlider.Value, blindThresholdSlider.Value);
            }
        }

        private void thresholdCalibrationButton_Click(object sender, RoutedEventArgs e)
        {
            calibrationButton.IsEnabled = false;
            calibrationButton.Content = "Calibrating...";

            thresholdCalibrationFinishedCallback = new Action(thresholdCalibration_Finihsed);
            IntPtr callbackPtr = Marshal.GetFunctionPointerForDelegate(thresholdCalibrationFinishedCallback);
            NativeWrappers.CommandDllWrapper.thresholdTouchCalibrate(callbackPtr);
        }

        private void thresholdCalibration_Finihsed()
        {
            Dispatcher.BeginInvoke((Action)delegate
            {
                calibrationButton.IsEnabled = true;
                calibrationButton.Content = "Calibrate";
            }, null);
        }

        private void systemCalibrateButton_Click(object sender, RoutedEventArgs e)
        {
            CalibrationWindow caliWin = new CalibrationWindow();
            caliWin.MainWindow = this;
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

        private void engineUpdateCallback()
        {
            if (EngineUpdate != null && !IsStopRequested)
            {
                EngineUpdate(this, EventArgs.Empty);
            }
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
    }
}
