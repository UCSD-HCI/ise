using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using InteractiveSpace.EngineController.NativeWrappers;

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

        public event EventHandler EngineUpdateOnMainUI;
        public event EventHandler EngineUpdate;

        private EngineBackgroundWorker()
        {
            worker = new BackgroundWorker();
            worker.WorkerSupportsCancellation = true;
            worker.DoWork += new DoWorkEventHandler(worker_DoWork);
        }


        public void Run()
        {
            worker.RunWorkerAsync();
        }

        public void Stop()
        {
            worker.CancelAsync();
        }

        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
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

            e.Cancel = true;
        }
    }
}
