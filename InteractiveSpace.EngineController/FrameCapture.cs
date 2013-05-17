using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Runtime.InteropServices;
using System.IO;

namespace InteractiveSpace.EngineController
{
    public class FrameCapture
    {
        private ImageSource rgbSource;
        private ImageSource depthSource;
        private ImageSource omniSource;
        private ushort[] depthData;
        private int stride;

        public FrameCapture()
        {
            rgbSource = VideoSources.SharedVideoSources.GetSource(VideoSourceType.RGB).Clone();
            depthSource = VideoSources.SharedVideoSources.GetSource(VideoSourceType.DepthHistogramed).Clone();
            omniSource = VideoSources.SharedVideoSources.GetSource(VideoSourceType.OmniTouch).Clone();

            stride = CommandDllWrapper.getDepthWidth();
            int depthPixelSize = stride * CommandDllWrapper.getDepthHeight();
            short[] depthInShort = new short[depthPixelSize];

            IntPtr depthPtr = ResultsDllWrapper.getFactoryImage(ImageProductType.DepthSourceProduct);
            Marshal.Copy(depthPtr, depthInShort, 0, depthPixelSize);

            depthData = new ushort[depthPixelSize];
            Buffer.BlockCopy(depthInShort, 0, depthData, 0, depthPixelSize * 2);
        }

        public void Save()
        {
            var dialog = new System.Windows.Forms.SaveFileDialog();
            dialog.FileName = DateTime.Now.ToString("yyyyMMdd_HHmmss");
            dialog.Filter = "Snapshot|*.*";
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.Cancel)
            {
                return;
            }

            string filePrefix = dialog.FileName;

            saveBmp(rgbSource, filePrefix + "_rgb.bmp");
            saveBmp(depthSource, filePrefix + "_depth.bmp");
            saveBmp(omniSource, filePrefix + "_omni.bmp");
            saveCsv(depthData, filePrefix + "_depth.csv");
        }

        private void saveBmp(ImageSource source, string fileName)
        {
            using (FileStream fs = new FileStream(fileName, FileMode.Create))
            {
                BmpBitmapEncoder bmp = new BmpBitmapEncoder();
                bmp.Frames.Add(BitmapFrame.Create((BitmapSource)source));
                bmp.Save(fs);
            }
        }

        private void saveCsv<T>(T[] data, string filename)
        {
            using (StreamWriter sw = new StreamWriter(filename))
            {
                for (int i = 0; i < data.Length; i++)
                {
                    sw.Write(data[i]);
                    sw.Write(i % stride == stride - 1 ? sw.NewLine : ",");
                }
            }
        }
    }
}
