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
using System.Windows.Media.Animation;
using System.Threading;

namespace InteractiveSpace.EngineController
{
    /// <summary>
    /// Interaction logic for Toast.xaml
    /// </summary>
    public partial class ToastWindow : Window
    {
        static System.Windows.Forms.Timer myTimer = new System.Windows.Forms.Timer();
        public ToastWindow(string msg)
        {
            InitializeComponent();
            Message.Content = msg;
           myTimer.Tick += new EventHandler(FadeOutTimerCallback);
           myTimer.Interval = 2000;
           myTimer.Start();
            
        }
        public void FadeOutTimerCallback(Object myObject,
                                            EventArgs myEventArgs)
        {
            myTimer.Stop();

            Storyboard storyboard = new Storyboard();
            TimeSpan duration = new TimeSpan(0, 0, 1);

            // Create a DoubleAnimation to fade the not selected option control
            DoubleAnimation animation = new DoubleAnimation();

            animation.From = 0.8;
            animation.To = 0.0;
            animation.Duration = new Duration(duration);
            // Configure the animation to target de property Opacity
            Storyboard.SetTargetName(animation, this.Name);
            Storyboard.SetTargetProperty(animation, new PropertyPath(Control.OpacityProperty));
            
            // Add the animation to the storyboard
            storyboard.Children.Add(animation);
            storyboard.Completed += new EventHandler(storyboard_Completed);

            // Begin the storyboard
            storyboard.Begin(this);
        }

        void storyboard_Completed(object sender, EventArgs e)
        {
            this.Close();
        }

    }
}
