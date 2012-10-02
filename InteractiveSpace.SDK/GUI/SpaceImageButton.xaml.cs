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

namespace InteractiveSpace.SDK.GUI
{
    /// <summary>
    /// Interaction logic for SpaceImageButton.xaml
    /// </summary>
    public partial class SpaceImageButton : UserControl, ISpaceControl
    {
        public SpaceImageButton()
        {
            InitializeComponent();
        }

        public ImageSource ImageSource
        {
            get { return imageButton.Source; }
            set { imageButton.Source = value; }
        }

        public Stretch ImageStretch
        {
            get { return imageButton.Stretch; }
            set { imageButton.Stretch = value; }
        }

        public event EventHandler<FingerEventArgs> FingerMove;
        public event EventHandler<FingerEventArgs> FingerDown;
        public event EventHandler<FingerEventArgs> FingerUp;

        public void RaiseFingerMove(FingerEventArgs e)
        {
            if (FingerMove != null)
            {
                FingerMove(this, e);
            }
        }

        public void RaiseFingerDown(FingerEventArgs e)
        {
            if (FingerDown != null)
            {
                FingerDown(this, e);
            }
        }

        public void RaiseFingerUp(FingerEventArgs e)
        {
            if (FingerUp != null)
            {
                FingerUp(this, e);
            }
        }
    }
}
