using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace InteractiveSpace.EngineController
{
    public class MultiTouchVistaController
    {
        private Process process;

        public MultiTouchVistaController()
        {
        }

        public void StartServer(string folder)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo(folder + @"\Multitouch.Service.Console.exe");
            startInfo.WorkingDirectory = folder;

            startInfo.RedirectStandardInput = true;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;

            process = Process.Start(startInfo);
        }

        public void StopServer()
        {
            process.StandardInput.WriteLine();
        }
    }
}
