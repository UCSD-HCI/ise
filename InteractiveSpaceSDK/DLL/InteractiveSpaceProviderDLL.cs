using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using ControlPanel;
using ControlPanel.NativeWrappers;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using System.Windows;
using System.IO;

namespace InteractiveSpaceSDK.DLL
{
    public class InteractiveSpaceProviderDLL : InteractiveSpaceProvider
    {
        private FingerTrackerDLL fingerTracker;
        private HandTrackerDLL handTracker;
        private ObjectTracker objectTracker;

        private MainWindow mainWindow;
        private Thread workingThread;
        private volatile bool stopRequested = false;
        private Int64 lastFrameCount;

        private Action grabCallback;

        public void Connect()
        {
            mainWindow = new ControlPanel.MainWindow();
            mainWindow.Show();
            mainWindow.Top = 0;
            mainWindow.Left = 0;

            //workingThread = new Thread(threadWorker);
            //workingThread.Start();
            mainWindow.EngineUpdate += new EventHandler(mainWindow_EngineUpdate);
        }

        public void Close()
        {
            stopRequested = true;
        }

        public void CreateFingerTracker()
        {
            fingerTracker = new FingerTrackerDLL();
        }

        public FingerTracker FingerTracker
        {
            get { return fingerTracker; }
        }

        /*private void threadWorker()
        {
            while (true)
            {
                if (stopRequested)
                {
                    mainWindow.Dispatcher.BeginInvoke((Action)delegate()
                    {
                        mainWindow.Close();
                    }, null);
                    break;
                }

                long newFrameCount = ResultsDllWrapper.getEngineFrameCount();
                if (newFrameCount > lastFrameCount)
                {
                    if (fingerTracker != null)
                    {
                        fingerTracker.Refresh();
                    }

                    lastFrameCount = newFrameCount;
                }
                else
                {
                    Thread.Yield();
                }
            }
            
        }*/


        void mainWindow_EngineUpdate(object sender, EventArgs e)
        {
            if (stopRequested)
            {
                mainWindow.EngineStop();
                return;
            }

            long newFrameCount = ResultsDllWrapper.getEngineFrameCount();

            if (fingerTracker != null)
            {
                fingerTracker.Refresh();
            }

            lastFrameCount = newFrameCount;
        }


        public void GrabAt(System.Windows.Media.Media3D.Point3D center, Action onFinished)
        {
            grabCallback = onFinished;  //save it so that it won't be disposed by GC
            CommandDllWrapper.motionCameraGrabAndSave(new FloatPoint3D(center.X, center.Y, center.Z), Marshal.GetFunctionPointerForDelegate(grabCallback));
        }

        public void GetLastGrabbedImageData(out byte[] data, out string mime)
        {
            unsafe
            {
                ReadLockedWrapperPtr ptr = ResultsDllWrapper.lockMotionCameraLastGrabbedImage();
                int width = CommandDllWrapper.getMotionCameraWidth();
                int height = CommandDllWrapper.getMotionCameraHeight();
                WriteableBitmap wb = new WriteableBitmap(height, width, 96, 96, PixelFormats.Bgr24, null);
                wb.Lock();
                wb.WritePixels(new Int32Rect(0, 0, height, width), ptr.IntPtr, width * height * 3, height * 3);
                wb.Unlock();
                ResultsDllWrapper.releaseReadLockedWrapperPtr(ptr);

                MemoryStream ms = new MemoryStream();
                JpegBitmapEncoder encoder = new JpegBitmapEncoder();
                encoder.Frames.Add(BitmapFrame.Create(wb));
                encoder.Save(ms);

                ms.Flush();
                ms.Seek(0, SeekOrigin.Begin);
                data = ms.ToArray();
                mime = "image/jpeg";
                ms.Close();
            }
        }


        public void CreateHandTracker()
        {
            handTracker = new HandTrackerDLL();
        }

        public HandTracker HandTracker
        {
            get { return handTracker; }
        }

        public void CreateObjectTracker()
        {
            objectTracker = new ObjectTrackerDLL();
        }

        public ObjectTracker ObjectTracker
        {
            get { return objectTracker; }
        }
    }
}
