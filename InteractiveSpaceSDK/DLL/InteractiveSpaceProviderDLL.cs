using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using ControlPanel;
using ControlPanel.NativeWrappers;

namespace InteractiveSpaceSDK.DLL
{
    public class InteractiveSpaceProviderDLL : InteractiveSpaceProvider
    {
        private FingerTrackerDLL fingerTracker;
        private MainWindow mainWindow;
        private Thread workingThread;
        private volatile bool stopRequested = false;
        private Int64 lastFrameCount;

        public void Connect()
        {
            mainWindow = new ControlPanel.MainWindow();
            mainWindow.Show();
            mainWindow.Top = 0;
            mainWindow.Left = 0;

            workingThread = new Thread(threadWorker);
            workingThread.Start();
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

        private void threadWorker()
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
            
        }
    }
}
