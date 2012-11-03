using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using InteractiveSpace.EngineController;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using System.Windows;
using System.IO;

namespace InteractiveSpace.SDK.DLL
{
    public class InteractiveSpaceProviderDLL : InteractiveSpaceProvider
    {
        private FingerTrackerDLL fingerTracker;
        private HandTrackerDLL handTracker;
        private RawVideoStreamingDLL rawVideoStreaming;

        private MainWindow mainWindow;
        private Thread workingThread;
        private volatile bool stopRequested = false;
        private Int64 lastFrameCount;

        public event EventHandler EngineUpdate;

        public void Connect()
        {
            mainWindow = new InteractiveSpace.EngineController.MainWindow();
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

            if (rawVideoStreaming != null)
            {
                rawVideoStreaming.Refresh(mainWindow.Dispatcher);
            }

            lastFrameCount = newFrameCount;

            if (EngineUpdate != null)
            {
                EngineUpdate(this, EventArgs.Empty);
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



        public void CreateRawVideoStreaming()
        {
            rawVideoStreaming = new RawVideoStreamingDLL();
        }

        public RawVideoStreaming RawVideoStreaming
        {
            get { return rawVideoStreaming; }
        }


        public double[,] GetRgbToTabletopHomography()
        {
            Matrix33 m = ResultsDllWrapper.getRgbSurfHomography();

            return new double[3, 3]
            {
                {m.M11, m.M12, m.M13},
                {m.M21, m.M22, m.M23},
                {m.M31, m.M32, m.M33}
            };
        }

        public double[,] GetDepthToTabletopHomography()
        {
            Matrix33 m = ResultsDllWrapper.getDepthSurfHomography();

            return new double[3, 3]
            {
                {m.M11, m.M12, m.M13},
                {m.M21, m.M22, m.M23},
                {m.M31, m.M32, m.M33}
            };
        }
    }
}
