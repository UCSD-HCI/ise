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
using Multitouch.Framework.WPF.Input;
using InteractiveSpace.SDK;
using InteractiveSpace.SDK.DLL;
using System.Windows.Media.Effects;

namespace InteractiveSpace.DocTrackApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Multitouch.Framework.WPF.Controls.Window
    {
        InteractiveSpaceProvider spaceProvider;

        public MainWindow()
        {
            InitializeComponent();

            MultitouchScreen.AllowNonContactEvents = true;

            spaceProvider = new InteractiveSpaceProviderDLL();
            spaceProvider.Connect();

            //Uncomment this line to enable raw video streaming.
            /*
            spaceProvider.CreateRawVideoStreaming();
            */

            //Uncomment these lines to draw fingers on the projected screen
            /*
            spaceProvider.CreateFingerTracker();
            vizLayer.SpaceProvider = spaceProvider;
            */
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            spaceProvider.Close();
        }

        private void grabButton_FingerDown(object sender, FingerEventArgs e)
        {
            (grabButton.Effect as DropShadowEffect).Opacity = 1.0;
        }

        private void grabButton_FingerUp(object sender, FingerEventArgs e)
        {
            (grabButton.Effect as DropShadowEffect).Opacity = 0;
        }

        private void grabButton_NewContact(object sender, NewContactEventArgs e)
        {
            (grabButton.Effect as DropShadowEffect).Opacity = 1.0;
            e.Contact.Capture(grabButton);
        }

        private void grabButton_ContactRemoved(object sender, ContactEventArgs e)
        {
            (grabButton.Effect as DropShadowEffect).Opacity = 0;
            e.Contact.ReleaseCapture();
        }

    }
}
