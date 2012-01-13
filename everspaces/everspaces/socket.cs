using System;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text;

namespace everspaces
{
	public class socket
	{
		private Socket s;
		private StreamReader r;
		private StreamWriter w;
		
		public socket(string host="localhost", int port=9999)
		{
			IPAddress[] ipAddress = Dns.GetHostAddresses(host);
			IPEndPoint ipEnd = new IPEndPoint(ipAddress[0], port);
			s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			s.Connect(ipEnd);
			
			NetworkStream ns = new NetworkStream(s);
			r = new StreamReader(ns);
			w = new StreamWriter(ns);
		}
		
		public void write(string msg)
		{
			
			//byte[] send = Encoding.ASCII.GetBytes(msg);
			//s.Send(send, send.Length, 0);
			
			w.WriteLine(msg);
			w.Flush();
		}
		
		public string read()
		{	
			/*
			int bytes = 0;
			string msg = "";
			byte[] receive = new byte[256];
			do
			{	
				bytes = s.Receive(receive, receive.Length, 0);
				msg += Encoding.ASCII.GetString(receive, 0, bytes);
			} while(bytes > 0);
			
			return msg;
			*/
			return r.ReadLine();
		}
		
	}
}

