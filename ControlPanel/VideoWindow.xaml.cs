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
using System.Windows.Shapes;
using System.ComponentModel;

namespace ControlPanel
{
    /// <summary>
    /// Interaction logic for VideoWindow.xaml
    /// </summary>
    public partial class VideoWindow : Window
    {
        private BackgroundWorker refreshWorker;
        private VideoSourceType videoSourceType;

        public VideoWindow()
        {
            InitializeComponent();
        }

        public void SetVideo(VideoSourceType videoSourceType)
        {
            this.videoSourceType = videoSourceType;

            Title = videoSourceType.ToString();

            refreshWorker = new BackgroundWorker();
            refreshWorker.DoWork += new DoWorkEventHandler(refreshWorker_DoWork);
            CompositionTarget.Rendering += new EventHandler(CompositionTarget_Rendering);
        }

        void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            if (!refreshWorker.IsBusy)
            {
                refreshWorker.RunWorkerAsync();
            }
        }

        void refreshWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            Dispatcher.BeginInvoke((Action)delegate
            {
                videoImage.Source = VideoSources.SharedVideoSources.GetSource(videoSourceType);
            });
        }

        private void videoImage_MouseDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void videoImage_MouseMove(object sender, MouseEventArgs e)
        {

        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            CompositionTarget.Rendering -= CompositionTarget_Rendering;
        }
    }
}
