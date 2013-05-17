using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;

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
            string file = folder + @"\Multitouch.Service.Console.exe";

            if (!File.Exists(file))
            {
                throw new FileNotFoundException("Cannot find " + file);
            }

            ProcessStartInfo startInfo = new ProcessStartInfo(folder + @"\Multitouch.Service.Console.exe");
            startInfo.WorkingDirectory = folder;

            //startInfo.RedirectStandardInput = true;
            //startInfo.UseShellExecute = false;
            //startInfo.CreateNoWindow = true;
            
            process = Process.Start(startInfo);
        }

        public void StopServer()
        {
            if (process != null && !process.HasExited)
            {
                if (process.StartInfo.RedirectStandardInput)
                {
                    process.StandardInput.WriteLine();
                }
                else
                {
                    try
                    {
                        process.CloseMainWindow();
                    }
                    catch (Exception)
                    {
                        process.Kill();
                    }
                }
            }
        }
    }
}
 