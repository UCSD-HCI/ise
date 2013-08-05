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

			//Initialize the Interactive Space Engine
            spaceProvider = new InteractiveSpaceProviderDLL();
            spaceProvider.Connect();

			//Enable the finger tracker. 
			spaceProvider.CreateFingerTracker();
			spaceProvider.FingerTracker.FingerMove += new EventHandler<FingerEventArgs>(FingerTracker_FingerMove);
            

			//Uncomment this line to enable raw video streaming.
            /*
            spaceProvider.CreateRawVideoStreaming();
            */

            //Uncomment this line to draw fingers on the projected screen            
            //vizLayer.SpaceProvider = spaceProvider;

        }

		private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			//Stop the Interactive Space Engine. 
			spaceProvider.Close();
		}

        private void button_NewContact(object sender, NewContactEventArgs e)
        {
			//Here we start a dragging operation. The initial position of the finger touched on the button is stored in the button's Tag. 

            Button b = (Button)sender;
            if (b.Tag != null)
            {
                return;
            }

            b.Background = Brushes.OrangeRed;
            b.Content = "Touching";

			//"Capture(b)" means the event "ContactMoved" of this Contact is trigerred on button "b" even if the finger gets out of the button. This is commonly used for dragging. 
            e.Contact.Capture(b);
            
			b.Tag = e.Contact.GetPosition(mainGrid);
        }

        private void button_ContactRemoved(object sender, ContactEventArgs e)
        {
            Button b = (Button)sender;
            if (e.Contact.Captured != b)
            {
                return;
            }

            b.Background = Brushes.White;
            b.Content = "Touch Me!";
            e.Contact.ReleaseCapture();
            b.Tag = null;
        }

        private void button_ContactMoved(object sender, ContactEventArgs e)
        {
            
            Button b = (Button)sender;
            if (e.Contact.Captured != b)
            {
                return;
            }

            if (b.Tag != null)
            {
				//Here we move the button according to the offset of the finger that is dragging this button. 

                Point prevP = (Point)b.Tag;
                Point p = e.Contact.GetPosition(mainGrid);

                b.Margin = new Thickness(b.Margin.Left + p.X - prevP.X, b.Margin.Top + p.Y - prevP.Y, 0, 0);
                b.Tag = p;
            }
            
        }

		//This function listens to 3D finger events. 
		private void FingerTracker_FingerMove(object sender, FingerEventArgs e)
		{
			//use e.ID to get finger ID
			int fingerId = e.ID;

			//use e.FingerState to get the state, can be "Hovering" or "OnSurface". 
			FingerState state = e.FingerState;

			//use e.Position to get finger position. 
			Point3D pos = e.Position;

			//(pos.X, pos.Y) is the finger's 2D absolute coordinate, in pixels.
			Point pos2d = new Point(pos.X, pos.Y);

			//Use hit testing to determine if the finger is above a WPF control. 
			HitTestResult result = VisualTreeHelper.HitTest(mainGrid, pos2d);
			if (result != null) {
				Trace.WriteLine ("Finger above " + result.VisualHit.ToString());
			}

			//pos.Z is the finger's height, i.e. distance from the finger to the tabletop, in centimeters. 
			Trace.WriteLine ("Finger height: " + pos.Z.ToString ());
		}


    }
}
