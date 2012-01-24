using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Automation;
using System.Diagnostics;
using System.IO;

namespace PLinkWin
{
    class AppController
    {
        [DllImport("user32.dll")]
        private static extern Int32 GetWindowThreadProcessId(
            IntPtr hWnd, out uint lpdwProcessId);

        [DllImport("user32.dll")]
        private static extern IntPtr GetForegroundWindow();

        public static string getAppFocus()
        {
            IntPtr hwnd = GetForegroundWindow();
            Process process = null;
            string appType;

            try
            {
                uint processID;
                GetWindowThreadProcessId(hwnd, out processID);
                process = Process.GetProcessById((int)processID);
            }
            catch { }


            if(process.ProcessName == "explorer")
            {
                appType = constants.APP_WIN_EXPLORER;
            }
            else if(process.ProcessName == "iexplore")
            {
                appType = constants.APP_WIN_IEXPLORE;
            }
            else if(process.ProcessName == "firefox")
            {
                appType = constants.APP_WIN_FIREFOX;
            }
            else if (process.ProcessName == "chrome")
            {
                appType = constants.APP_WIN_CHROME;
            }
            else
            {
                appType = constants.UNKNOWN;
            }

            return appType;
        }

        public static string getURI(string appType)
        {
            IntPtr hwnd = GetForegroundWindow();
            Process process = null;
            string uri;

            try
            {
                uint processID;
                GetWindowThreadProcessId(hwnd, out processID);
                process = Process.GetProcessById((int)processID);
            }
            catch { }

            if(appType == constants.APP_WIN_EXPLORER)
            {
                uri = GetExplorerPath(process);
                if (uri == null)
                    uri = constants.UNKNOWN;
            }
            else if(appType == constants.APP_WIN_IEXPLORE)
            {
                uri = GetInternetExplorerUrl(process);
                if (uri == null)
                    uri = constants.UNKNOWN;
            }
            else if(appType == constants.APP_WIN_FIREFOX)
            {
                uri = GetFirefoxUrl(process);
                if (uri == null)
                    uri = constants.UNKNOWN;
            }
            else if (appType == constants.APP_WIN_CHROME)
            {
                uri = GetChromeUrl(process);
                if (uri == null)
                    uri = constants.UNKNOWN;
            }
            else
            {
                uri = constants.UNKNOWN;
            }
            
            return uri;
        }

        private static string GetExplorerPath(Process process)
        {
            if (process == null)
                throw new ArgumentNullException("process");

            if (process.MainWindowHandle == IntPtr.Zero)
                return null;

            Process p = new Process();
            
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.FileName = "PLinkWin.getPathExplorer.exe";
            p.Start();
            StreamReader reader = p.StandardOutput;
            string path = reader.ReadToEnd();

            if (path == "" | path == null)
                return null;

            return path;
        }

        private static string GetInternetExplorerUrl(Process process)
        {
            if (process == null)
                throw new ArgumentNullException("process");

            if (process.MainWindowHandle == IntPtr.Zero)
                return null;

            AutomationElement element = AutomationElement.FromHandle(process.MainWindowHandle);
            if (element == null)
                return null;

            AutomationElement rebar = element.FindFirst(TreeScope.Children, new PropertyCondition(AutomationElement.ClassNameProperty, "ReBarWindow32"));
            if (rebar == null)
                return null;

            AutomationElement edit = rebar.FindFirst(TreeScope.Subtree, new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Edit));

            return ((ValuePattern)edit.GetCurrentPattern(ValuePattern.Pattern)).Current.Value as string;
        }

        private static string GetFirefoxUrl(Process process)
        {
            if (process == null)
                throw new ArgumentNullException("process");

            if (process.MainWindowHandle == IntPtr.Zero)
                return null;

            AutomationElement element = AutomationElement.FromHandle(process.MainWindowHandle);
            if (element == null)
                return null;

            AutomationElement doc = element.FindFirst(TreeScope.Subtree, new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Document));
            if (doc == null)
                return null;

            return ((ValuePattern)doc.GetCurrentPattern(ValuePattern.Pattern)).Current.Value as string;
        }

        public static string GetChromeUrl(Process process)
        {
            if (process == null)
                throw new ArgumentNullException("process");

            if (process.MainWindowHandle == IntPtr.Zero)
                return null;

            AutomationElement element = AutomationElement.FromHandle(process.MainWindowHandle);
            if (element == null)
                return null;

            AutomationElement edit = element.FindFirst(TreeScope.Children, new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Edit));
            return ((ValuePattern)edit.GetCurrentPattern(ValuePattern.Pattern)).Current.Value as string;
        }

        public static void openURL(string browser, string url)
        {
            Process.Start(browser, url);
        }

        public static void openFile(string path)
        {
            Process.Start(path);
        }
    }
}
