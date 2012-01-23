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
using InteractiveSpaceSDK.GUI;
using System.IO;
using System.Diagnostics;
using everspaces;

namespace DocSenseApp
{
    /// <summary>
    /// Interaction logic for CaptureGalleryControl.xaml
    /// </summary>
    public partial class CaptureGalleryControl : UserControl, ISpacePanel
    {
        private everspaces.everspaces everSpaces;
        private string currentEvernoteId = null;

        public CaptureGalleryControl()
        {
            InitializeComponent();
            try
            {
                everSpaces = new everspaces.everspaces();
                everSpaces.createLinkCompleted += new everspaces.createLinkHandler(everSpaces_createLinkCompleted);
            }
            catch (Exception)
            {
                everSpaces = null;
            }
        }

        public IEnumerable<ISpaceControl> SpaceChildren
        {
            get 
            {
                foreach (var element in galleryStack.Children)
                {
                    if (element is ISpaceControl)
                    {
                        yield return element as ISpaceControl;
                    }
                }
            }
        }

        public event EventHandler<InteractiveSpaceSDK.FingerEventArgs> FingerMove;
        public event EventHandler<InteractiveSpaceSDK.FingerEventArgs> FingerDown;
        public event EventHandler<InteractiveSpaceSDK.FingerEventArgs> FingerUp;
        public event EventHandler UploadingBegin;

        public void RaiseFingerMove(InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (FingerMove != null)
            {
                FingerMove(this, e);
            }
        }

        public void RaiseFingerDown(InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (FingerDown != null)
            {
                FingerDown(this, e);
            }
        }

        public void RaiseFingerUp(InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (FingerUp != null)
            {
                FingerUp(this, e);
            }
        }

        public void AddImage(byte[] data, string mime)
        {
            MemoryStream ms = new MemoryStream(data);
            ms.Seek(0, SeekOrigin.Begin);

            SpaceImageButton newImg = new SpaceImageButton();
            newImg.ImageSource = BitmapFrame.Create(ms, BitmapCreateOptions.None, BitmapCacheOption.OnLoad);
            newImg.Width = 100;
            newImg.Height = 100;
            newImg.ImageStretch = Stretch.UniformToFill;    //FIXME: if uniform, FingerDown is only raised outside of the image

            ms.Seek(0, SeekOrigin.Begin);
            newImg.Tag = new Tuple<byte[], string>(ms.ToArray(), mime);
            newImg.FingerDown += new EventHandler<InteractiveSpaceSDK.FingerEventArgs>(newImg_FingerDown);

            galleryStack.Children.Add(newImg);

            ms.Close();

            evernoteButton.Visibility = Visibility.Visible;
        }

        public void ClearAllImages()
        {
            evernoteButton.Visibility = Visibility.Hidden;
            galleryStack.Children.Clear();
            currentEvernoteId = null;
            linkButton.Visibility = Visibility.Hidden;
        }

        void newImg_FingerDown(object sender, InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (!(sender is SpaceImageButton))
            {
                return;
            }

            SpaceImageButton img = sender as SpaceImageButton;
            galleryStack.Children.Remove(img);

            if (galleryStack.Children.Count == 0)
            {
                evernoteButton.Visibility = Visibility.Hidden;
            }
        }

        private void SpaceImageButton_FingerDown(object sender, InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (UploadingBegin != null)
            {
                UploadingBegin(this, EventArgs.Empty);
            }

            slink link = new slink(new metadata());
            link.setTitle("Screen Capture");
            List<string> tags = new List<string>();
            tags.Add("Demo");
           
            evernoteButton.Visibility = Visibility.Hidden;

            List<Tuple<byte[], string>> resources = new List<Tuple<byte[], string>>();

            foreach (var child in galleryStack.Children)
            {
                SpaceImageButton img = child as SpaceImageButton;
                if (img == null)
                {
                    continue;
                }

                var data = img.Tag as Tuple<byte[], string>;
                if (data == null)
                {
                    continue;
                }

                resources.Add(data);
            }

            link.setResources(resources);

            if (everSpaces != null)
            {
                everSpaces.createLink(link);
            }

            Trace.WriteLine("Uploading to evernote...");
        }

        void everSpaces_createLinkCompleted(string data)
        {
            Dispatcher.BeginInvoke((Action)delegate()
            {
                currentEvernoteId = data;
                linkButton.Visibility = Visibility.Visible;
            }, null);
        }

        private void linkButton_FingerDown(object sender, InteractiveSpaceSDK.FingerEventArgs e)
        {
            if (currentEvernoteId != null && everSpaces != null)
            {
                everSpaces.openLink(currentEvernoteId);
                System.Diagnostics.Process.Start("https://sandbox.evernote.com/view/" + currentEvernoteId);
                Trace.WriteLine("Opening link...");
            }
        }

    }
}
