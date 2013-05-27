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

namespace InteractiveSpaceTemplate
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
            
            //spaceProvider.CreateFingerTracker();
            //vizLayer.SpaceProvider = spaceProvider;
            
        }

        private void button_NewContact(object sender, NewContactEventArgs e)
        {
            Button b = (Button)sender; 
            b.Background = Brushes.OrangeRed;
            b.Content = "Touching";
            e.Contact.Capture(b);
        }

        private void button_ContactRemoved(object sender, ContactEventArgs e)
        {
            Button b = (Button)sender;
            b.Background = Brushes.White;
            b.Content = "Touch Me!";
            e.Contact.ReleaseCapture();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            spaceProvider.Close();
        }

    }
}
