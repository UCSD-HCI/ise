using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using InteractiveSpace.EngineController.NativeWrappers;
using System.Threading;

namespace InteractiveSpace.EngineController
{
    public class EngineBackgroundWorker
    {
        private static EngineBackgroundWorker instance;
        public static EngineBackgroundWorker Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new EngineBackgroundWorker();
                }

                return instance;
            }
        }

        private BackgroundWorker worker;
        private volatile bool isInitialized;

        public event EventHandler EngineUpdateOnMainUI;
        public event EventHandler EngineUpdate;
        public event EventHandler CompletedOnMainUI;

        private EngineBackgroundWorker()
        {
            worker = new BackgroundWorker();
            worker.WorkerSupportsCancellation = true;
            worker.DoWork += new DoWorkEventHandler(worker_DoWork);
            worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(worker_RunWorkerCompleted);
        }


        public void Run()
        {
            worker.RunWorkerAsync();
        }

        public void Stop()
        {
            worker.CancelAsync();
        }

        public void WaitForInit()
        {
            while (!isInitialized)
            {
                Thread.Yield();
            }
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            App.Current.Dispatcher.Invoke((Action)delegate()
            {
                if (CompletedOnMainUI != null)
                {
                    CompletedOnMainUI(this, EventArgs.Empty);
                }
            }, null);
        }

        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            CommandDllWrapper.engineInit();

            isInitialized = true;

            while (!worker.CancellationPending)
            {
                CommandDllWrapper.engineMainLoopUpdate();

                if (EngineUpdate != null)
                {
                    EngineUpdate(this, EventArgs.Empty);
                }

                App.Current.Dispatcher.Invoke((Action)delegate()
                {
                    if (EngineUpdateOnMainUI != null)
                    {
                        EngineUpdateOnMainUI(this, EventArgs.Empty);
                    }
                }, null);
            }

            CommandDllWrapper.engineExit();
            e.Cancel = true;
        }
    }
}
