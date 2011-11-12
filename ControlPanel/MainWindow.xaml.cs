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
using ControlPanel.NativeWrappers;

namespace ControlPanel
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private VideoWindow rawVideoWindow, depthVideoWindow, multiTouchVideoWindow;
        private bool isSlidersValueLoaded;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            threshold1Slider.Value = Properties.Settings.Default.Threshold1;
            threshold2Slider.Value = Properties.Settings.Default.Threshold2;
            threshold3Slider.Value = Properties.Settings.Default.Threshold3;
            threshold4Slider.Value = Properties.Settings.Default.Threshold4;
            isSlidersValueLoaded = true;

            CommandDllWrapper.engineRun();
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

            CommandDllWrapper.engineStop();
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

        private void thresholdSliders_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            e.Handled = true;

            if (sender == threshold1Slider)
            {
                if (threshold2Slider.Value < e.NewValue - double.Epsilon)
                {
                    threshold2Slider.Value = e.NewValue;
                }

                threshold1TextBox.Text = e.NewValue.ToString("0.00");
            }
            else if (sender == threshold2Slider)
            {
                if (e.NewValue < threshold1Slider.Value - double.Epsilon)
                {
                    e.Handled = false;
                    threshold2Slider.Value = threshold1Slider.Value;
                }
                else
                {
                    threshold2TextBox.Text = e.NewValue.ToString("0.00");
                }
            }
            else if (sender == threshold3Slider)
            {
                if (threshold4Slider.Value < e.NewValue - double.Epsilon)
                {
                    threshold4Slider.Value = e.NewValue;
                }

                threshold3TextBox.Text = e.NewValue.ToString("0.00");
            }
            else if (sender == threshold4Slider)
            {
                if (e.NewValue < threshold3Slider.Value - double.Epsilon)
                {
                    e.Handled = false;
                    threshold4Slider.Value = threshold3Slider.Value;
                }
                else
                {
                    threshold4TextBox.Text = e.NewValue.ToString("0.00");
                }
            }

            if (e.Handled && isSlidersValueLoaded)
            {
                Properties.Settings.Default.Threshold1 = threshold1Slider.Value;
                Properties.Settings.Default.Threshold2 = threshold2Slider.Value;
                Properties.Settings.Default.Threshold3 = threshold3Slider.Value;
                Properties.Settings.Default.Threshold4 = threshold4Slider.Value;
                Properties.Settings.Default.Save();

                NativeWrappers.CommandDllWrapper.setOmniTouchParameters(threshold1Slider.Value, threshold2Slider.Value, threshold3Slider.Value, threshold4Slider.Value);
            }
        }
    }
}
