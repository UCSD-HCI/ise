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
using System.Diagnostics;

namespace ControlPanel
{
    /// <summary>
    /// Interaction logic for ZoomableVideoUserControl.xaml
    /// </summary>
    public partial class ZoomableVideoUserControl : UserControl
    {
        public ZoomableVideoUserControl()
        {
            InitializeComponent();
        }

        public ImageSource ImageSource
        {
            get
            {
                return videoImage.Source;
            }

            set
            {
                videoImage.Source = value;
            }
        }

        private void videoImage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            double scale = Math.Min(videoImage.RenderSize.Width / videoImage.Source.Width, videoImage.RenderSize.Height / videoImage.Source.Height);
            TransformGroup trans = new TransformGroup();
            trans.Children.Add(new ScaleTransform(scale, scale));
            trans.Children.Add(new TranslateTransform((UiCanvas.RenderSize.Width - videoImage.Source.Width * scale) / 2, (UiCanvas.RenderSize.Height - videoImage.Source.Height * scale) / 2));
            UiCanvas.RenderTransform = trans;
        }
    }
}
