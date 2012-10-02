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

namespace RiskSpace
{
    /// <summary>
    /// Interaction logic for ControlButton.xaml
    /// </summary>
    public partial class ControlButton : UserControl
    {
        public event EventHandler OkButtonClick;
        public event EventHandler CancelButtonClick;
        public event EventHandler NextButtonClick;

        public ControlButton()
        {
            InitializeComponent();
        }

        public bool OkButtonVisible
        {
            get
            {
                return okButton.Visibility == Visibility.Visible;
            }
            set
            {
                okButton.Visibility = value ? Visibility.Visible : Visibility.Collapsed;
                okButton.IsEnabled = value;
            }
        }

        public bool CancelButtonVisible
        {
            get
            {
                return cancelButton.Visibility == Visibility.Visible;
            }
            set
            {
                cancelButton.Visibility = value ? Visibility.Visible : Visibility.Collapsed;
                cancelButton.IsEnabled = value;
            }
        }

        public bool NextButtonVisible
        {
            get
            {
                return nextButton.Visibility == Visibility.Visible;
            }
            set
            {
                nextButton.Visibility = value ? Visibility.Visible : Visibility.Collapsed;
                nextButton.IsEnabled = value;
            }
        }

        public void HideAllButtons()
        {
            OkButtonVisible = false;
            CancelButtonVisible = false;
            NextButtonVisible = false;
        }

        private void okButton_ContactRemoved(object sender, Multitouch.Framework.WPF.Input.ContactEventArgs e)
        {
            if (OkButtonClick != null)
            {
                OkButtonClick(this, EventArgs.Empty);
            }
        }

        private void cancelButton_ContactRemoved(object sender, Multitouch.Framework.WPF.Input.ContactEventArgs e)
        {
            if (CancelButtonClick != null)
            {
                CancelButtonClick(this, EventArgs.Empty);
            }
        }

        private void nextButton_ContactRemoved(object sender, Multitouch.Framework.WPF.Input.ContactEventArgs e)
        {
            if (NextButtonClick != null)
            {
                NextButtonClick(this, EventArgs.Empty);
            }
        }
    }
}
