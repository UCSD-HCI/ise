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
            //Trace.WriteLine("FingerDown: " + e.ID.ToString() + " @ " + e.RealWorldPosition.ToString());
        }

        private void SpaceCanvas_FingerUp(object sender, FingerEventArgs e)
        {
            //Trace.WriteLine("FingerUp: " + e.ID.ToString() + " @ " + e.RealWorldPosition.ToString());
        }

        private void SpaceCanvas_FingerMove(object sender, FingerEventArgs e)
        {
           //Trace.WriteLine("FingerMove: " + e.ID.ToString() + " @ " + e.RealWorldPosition.ToString());
        }
    }
}
