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
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;

namespace InteractiveSpaceSDK.GUI
{
    /// <summary>
    /// Interaction logic for SpaceToggleButton.xaml
    /// </summary>
    public partial class SpaceToggleButton : UserControl, ISpaceControl
    {
        private bool isChecked = false;

        public bool IsChecked 
        { 
            get 
            { 
                return isChecked; 
            }
            set
            {
                if (isChecked != value)
                {
                    isChecked = value;
                }

                isChecked = value;

                if (isChecked)
                {
                    DoubleAnimation animationOpacity = new DoubleAnimation(0, 1, new Duration(TimeSpan.FromSeconds(0.5)));
                    glow.BeginAnimation(Ellipse.OpacityProperty, animationOpacity);
                }
                else
                {
                    DoubleAnimation animationOpacity = new DoubleAnimation(1, 0, new Duration(TimeSpan.FromSeconds(1)));
                    glow.BeginAnimation(Ellipse.OpacityProperty, animationOpacity);
                }
            }
        }


        public event EventHandler<FingerEventArgs> FingerMove;
        public event EventHandler<FingerEventArgs> FingerDown;
        public event EventHandler<FingerEventArgs> FingerUp;

        public SpaceToggleButton()
        {
            InitializeComponent();
        }

        public string Text
        {
            get { return textBlock.Text; }
            set { textBlock.Text = value; }
        }

        public void RaiseFingerMove(FingerEventArgs e)
        {
            if (FingerMove != null)
            {
                FingerMove(this, e);
            }
        }

        public void RaiseFingerDown(FingerEventArgs e)
        {
            IsChecked = !IsChecked;
            
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
