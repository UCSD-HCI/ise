using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace PLinkWin
{
    class server
    {
        private TcpListener tcpListener;
        private Thread listenThread;

        public server()
        {
            this.tcpListener = new TcpListener(IPAddress.Any, 9999);
            this.listenThread = new Thread(new ThreadStart(ListenForClients));
            this.listenThread.Start();
        }

        private void ListenForClients()
        {
            this.tcpListener.Start();

            while (true)
            {
                //blocks until a client has connected to the server
                TcpClient client = this.tcpListener.AcceptTcpClient();

                //create a thread to handle communication 
                //with connected client
                Thread clientThread = new Thread(new ParameterizedThreadStart(HandleClientComm));
                clientThread.Start(client);
            }
        }

        private void HandleClientComm(object client)
        {
            TcpClient tcpClient = (TcpClient)client;
            NetworkStream clientStream = tcpClient.GetStream();

            StreamReader r = new StreamReader(clientStream);
            StreamWriter w = new StreamWriter(clientStream);

            Console.WriteLine("Client connected");

            while (true)
            {
                string jsonTxt;
                JObject request;

                try
                {
                    jsonTxt = r.ReadLine();
                    request = JObject.Parse(jsonTxt);
                }
                catch
                {
                    break;
                }

                string type = (string)request["TYPE"];

                if (type == constants.REQUEST_APPFOCUS)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_APPFOCUS);

                    handleAppFocusRequest(w);

                    Console.WriteLine("SOCKET: {0} sent.", constants.RESPONSE_APPFOCUS);
                }
                else if (type == constants.REQUEST_FIREFOXURL)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_FIREFOXURL);

                    handleFirefoxURLRequest(w);

                    Console.WriteLine("SOCKET: {0} sent.", constants.RESPONSE_FIREFOXURL);
                }
                else if (type == constants.REQUEST_IEXPLOREURL)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_IEXPLOREURL);

                    handleIExploreURLRequest(w);

                    Console.WriteLine("SOCKET: {0} sent.", constants.RESPONSE_IEXPLOREURL);
                }
                else if (type == constants.REQUEST_CHROMEURL)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_CHROMEURL);

                    handleChromeURLRequest(w);

                    Console.WriteLine("SOCKET: {0} sent.", constants.RESPONSE_CHROMEURL);
                }
                else if (type == constants.REQUEST_FIREFOXOPENURL)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_FIREFOXOPENURL);

                    JObject id = (JObject)request["DATA"];
                    string url = (string)id[constants.IDENTIFIER_FIREFOXOPENURL];

                    handleFirefoxOpenRequest(url);
                }
                else if (type == constants.REQUEST_IEXPLOREOPENURL)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_IEXPLOREOPENURL);

                    JObject id = (JObject)request["DATA"];
                    string url = (string)id[constants.IDENTIFIER_IEXPLOREOPENURL];

                    handleIExploreOpenRequest(url);
                }
                else if (type == constants.REQUEST_CHROMEOPENURL)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_CHROMEOPENURL);

                    JObject id = (JObject)request["DATA"];
                    string url = (string)id[constants.IDENTIFIER_CHROMEOPENURL];

                    handleChromeOpenRequest(url);
                }
                else if (type == constants.REQUEST_EXPLORERFILE)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_EXPLORERFILE);

                    handleExplorerFileRequest(w);

                    Console.WriteLine("SOCKET: {0} sent.", constants.RESPONSE_EXPLORERFILE);
                }
                else if (type == constants.REQUEST_EXPLOREROPENFILE)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_EXPLOREROPENFILE);

                    JObject id = (JObject)request["DATA"];
                    string path = (string)id[constants.IDENTIFIER_EXPLOREROPENFILE];

                    handleExplorerOpenRequest(path);
                }
                else if (type == constants.REQUEST_EXPLORERUPLOAD)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_EXPLORERUPLOAD);

                    JObject id = (JObject)request["DATA"];
                    string path = (string)id[constants.IDENTIFIER_EXPLORERUPLOAD];

                    handleExplorerUploadRequest(clientStream, path);

                    Console.WriteLine("SOCKET: data sent.");
                }
                else if (type == constants.REQUEST_ADOBEINFO)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_ADOBEINFO);

                    handleAdobeInfoRequest(w);

                    Console.WriteLine("SOCKET: {0} sent.", constants.RESPONSE_ADOBEINFO);
                }
                else if (type == constants.REQUEST_ADOBEOPENINFO)
                {
                    Console.WriteLine("SOCKET: {0} received.", constants.REQUEST_ADOBEOPENINFO);

                    JObject id = (JObject)request["DATA"];
                    string info = (string)id[constants.IDENTIFIER_ADOBEOPENINFO];

                    handleAdobeOpenRequest(info);
                }
                else
                {
                    Console.WriteLine("SOCKET: received unknown operation.");
                }
            }

            Console.WriteLine("Connection closed");
            tcpClient.Close();
        }

        private void handleAppFocusRequest(StreamWriter w)
        {
            string appType = AppController.getAppFocus();
            Console.WriteLine("APPTYPE: {0}", appType);
            Dictionary<string, object> map = new Dictionary<string, object>();
            Dictionary<string, string> dataMap = new Dictionary<string, string>();

            dataMap.Add(constants.IDENTIFIER_APPFOCUS, appType);
            map.Add("TYPE", constants.RESPONSE_APPFOCUS);
            map.Add("DATA", dataMap);

            string jsonObject = JsonConvert.SerializeObject(map);
            w.WriteLine(jsonObject);
            w.Flush();
        }

        private void handleFirefoxURLRequest(StreamWriter w)
        {
            string url = AppController.getURI(constants.APP_WIN_FIREFOX);
            Dictionary<string, object> map = new Dictionary<string, object>();
            Dictionary<string, string> dataMap = new Dictionary<string, string>();

            dataMap.Add(constants.IDENTIFIER_FIREFOXURL, url);
            map.Add("TYPE", constants.RESPONSE_FIREFOXURL);
            map.Add("DATA", dataMap);

            string jsonObject = JsonConvert.SerializeObject(map);
            w.WriteLine(jsonObject);
            w.Flush();
        }

        private void handleIExploreURLRequest(StreamWriter w)
        {
            string url = AppController.getURI(constants.APP_WIN_IEXPLORE);
            Dictionary<string, object> map = new Dictionary<string, object>();
            Dictionary<string, string> dataMap = new Dictionary<string, string>();

            dataMap.Add(constants.IDENTIFIER_IEXPLOREURL, url);
            map.Add("TYPE", constants.RESPONSE_IEXPLOREURL);
            map.Add("DATA", dataMap);

            string jsonObject = JsonConvert.SerializeObject(map);
            w.WriteLine(jsonObject);
            w.Flush();
        }

        private void handleChromeURLRequest(StreamWriter w)
        {
            string url = AppController.getURI(constants.APP_WIN_CHROME);
            Dictionary<string, object> map = new Dictionary<string, object>();
            Dictionary<string, string> dataMap = new Dictionary<string, string>();

            dataMap.Add(constants.IDENTIFIER_CHROMEURL, url);
            map.Add("DATA", dataMap);
            map.Add("TYPE", constants.RESPONSE_CHROMEURL);

            string jsonObject = JsonConvert.SerializeObject(map);
            w.WriteLine(jsonObject);
            w.Flush();
        }

        private void handleExplorerFileRequest(StreamWriter w)
        {
            string path = AppController.getURI(constants.APP_WIN_EXPLORER);
            Dictionary<string, object> map = new Dictionary<string, object>();
            Dictionary<string, string> dataMap = new Dictionary<string, string>();

            dataMap.Add(constants.IDENTIFIER_EXPLORERFILE, path);
            map.Add("TYPE", constants.RESPONSE_EXPLORERFILE);
            map.Add("DATA", dataMap);

            string jsonObject = JsonConvert.SerializeObject(map);
            w.WriteLine(jsonObject);
            w.Flush();
        }

        private void handleAdobeInfoRequest(StreamWriter w)
        {
            string info = AppController.getURI(constants.APP_WIN_ADOBE);
            Dictionary<string, object> map = new Dictionary<string, object>();
            Dictionary<string, string> dataMap = new Dictionary<string, string>();

            dataMap.Add(constants.IDENTIFIER_ADOBEINFO, info);
            map.Add("TYPE", constants.RESPONSE_ADOBEINFO);
            map.Add("DATA", dataMap);

            string jsonObject = JsonConvert.SerializeObject(map);
            w.WriteLine(jsonObject);
            w.Flush();
        }

        private void handleFirefoxOpenRequest(string url)
        {
            AppController.openURL("firefox", url);
        }

        private void handleIExploreOpenRequest(string url)
        {
            AppController.openURL("iexplore", url);
        }

        private void handleChromeOpenRequest(string url)
        {
            AppController.openURL("chrome", url);
        }

        private void handleExplorerOpenRequest(string path)
        {
            AppController.openFile(path);
        }

        private void handleAdobeOpenRequest(string info)
        {
            AppController.openFile("AcroRd32", "/A " + info);
        }

        private void handleExplorerUploadRequest(NetworkStream ns, string path)
        {
            byte[] data = ReadFully(File.OpenRead(path));

            ns.Write(data, 0, data.Length);
            ns.Flush();
        }

        private byte[] ReadFully(Stream stream)
        {
            byte[] buffer = new byte[32768];
            using (MemoryStream ms = new MemoryStream())
            {
                while (true)
                {
                    int read = stream.Read(buffer, 0, buffer.Length);
                    if (read <= 0)
                    {
                        return ms.ToArray();
                    }
                    ms.Write(buffer, 0, read);
                }
            }
        }
    }
}
