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

namespace InteractiveSpaceTemplate
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Multitouch.Framework.WPF.Controls.Window
    {
        InteractiveSpace.SDK.InteractiveSpaceProvider spaceProvider;

        public MainWindow()
        {
            InitializeComponent();

            MultitouchScreen.AllowNonContactEvents = true;

            spaceProvider = new InteractiveSpace.SDK.DLL.InteractiveSpaceProviderDLL();
            spaceProvider.Connect();
        }

        private void okButton_ContactRemoved(object sender, Multitouch.Framework.WPF.Input.ContactEventArgs e)
        {
            MessageBox.Show("Success");
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            spaceProvider.Close();
        }
    }
}
