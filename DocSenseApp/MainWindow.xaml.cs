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
using InteractiveSpaceSDK.DLL;
using System.Diagnostics;

namespace DocSenseApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private InteractiveSpaceProviderDLL sdk;
        private ProjectorFeedbackWindow feedbackWindow;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            sdk = new InteractiveSpaceProviderDLL();
            sdk.Connect();

            
            sdk.CreateFingerTracker();
            sdk.FingerTracker.FingerDown += new EventHandler<InteractiveSpaceSDK.FingerEventArgs>(FingerTracker_FingerDown);
            sdk.FingerTracker.FingerUp += new EventHandler<InteractiveSpaceSDK.FingerEventArgs>(FingerTracker_FingerUp);
            sdk.FingerTracker.FingerMove += new EventHandler<InteractiveSpaceSDK.FingerEventArgs>(FingerTracker_FingerMove);

            sdk.CreateHandTracker();

            feedbackWindow = new ProjectorFeedbackWindow();
            feedbackWindow.Show();

            feedbackWindow.SpaceProvider = sdk;
        }

        void FingerTracker_FingerMove(object sender, InteractiveSpaceSDK.FingerEventArgs e)
        {
            //Trace.WriteLine("FingerMove: " + e.ID.ToString() + " @ " + e.Position.ToString());
        }

        void FingerTracker_FingerUp(object sender, InteractiveSpaceSDK.FingerEventArgs e)
        {
            //Trace.WriteLine("FingerUp: " + e.ID.ToString() + " @ " + e.Position.ToString());
        }

        void FingerTracker_FingerDown(object sender, InteractiveSpaceSDK.FingerEventArgs e)
        {
            //Trace.WriteLine("FingerDown: " + e.ID.ToString() + " @ " + e.Position.ToString());
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            feedbackWindow.Close();
            sdk.Close();
        }
    }
}
