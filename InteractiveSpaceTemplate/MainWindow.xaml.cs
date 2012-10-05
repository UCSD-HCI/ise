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

namespace InteractiveSpaceTemplate
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Multitouch.Framework.WPF.Controls.Window
    {
        public MainWindow()
        {
            InitializeComponent();

            InteractiveSpace.SDK.InteractiveSpaceProvider provider = new InteractiveSpace.SDK.DLL.InteractiveSpaceProviderDLL();
            provider.Connect();
        }

        private void okButton_ContactRemoved(object sender, Multitouch.Framework.WPF.Input.ContactEventArgs e)
        {
            MessageBox.Show("Success");
        }
    }
}
