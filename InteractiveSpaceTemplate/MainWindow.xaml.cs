﻿using System;
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

            spaceProvider.EngineUpdate += new EventHandler(spaceProvider_EngineUpdate);

            //Uncomment these lines to draw fingers on the projected screen
            
            spaceProvider.CreateFingerTracker();
            //vizLayer.SpaceProvider = spaceProvider;

            spaceProvider.FingerTracker.FingerCaptured += new EventHandler<FingerEventArgs>(FingerTracker_FingerCaptured);
        }

        void FingerTracker_FingerCaptured(object sender, FingerEventArgs e)
        {
            //use e.ID to get finger ID
            //e.Position
        }

        void spaceProvider_EngineUpdate(object sender, EventArgs e)
        {
            //Get called every frame
            //throw new NotImplementedException();
        }

        private void button_NewContact(object sender, NewContactEventArgs e)
        {
            Button b = (Button)sender;
            if (b.Tag != null)
            {
                return;
            }

            b.Background = Brushes.OrangeRed;
            b.Content = "Touching";
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
                Point prevP = (Point)b.Tag;
                Point p = e.Contact.GetPosition(mainGrid);

                b.Margin = new Thickness(b.Margin.Left + p.X - prevP.X, b.Margin.Top + p.Y - prevP.Y, 0, 0);
                b.Tag = p;
            }
            
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            spaceProvider.Close();
        }

    }
}
