using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.ComponentModel;
using System.Runtime.Remoting.Messaging;
using System.Net;
using System.Net.Sockets;
using Thrift;
using Thrift.Protocol;
using Thrift.Transport;
using Evernote.EDAM.Type;
using Evernote.EDAM.UserStore;
using Evernote.EDAM.NoteStore;
using Evernote.EDAM.Error;
using Evernote.EDAM.Limits;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace everspaces
{
	class MainClass
	{ 	
		public static List<string> ids = new List<string>();
		public static slink l = null;
		
		public static void Main (string[] args)
		{
			//test
			everspaces e = new everspaces();
			
			e.createLinkCompleted += new createLinkHandler(getCreateLinkResult);
			e.getLinkCompleted += new getLinkHandler(getLinkResult);
	
			metadata coord;
			coord.x1 = 0;
			coord.x2 = 5;
			coord.y1 = 0;
			coord.y2 = 5;
			
			slink link = new slink(coord);
			link.setTitle("My first link!");
			link.setComment("This is the coolest!");
			List<String> tags = new List<String>();
			tags.Add("ryan");
			link.setTags(tags);
			//List<String> files = new List<String>();
			//files.Add("qrcode.jpg");
			//link.setResources(files);
			
			Console.WriteLine("Waiting for link...");
			Thread.Sleep(5000);
			
			IAsyncResult result;
			
			e.createLink(link, true);
			
			while(ids.Count == 0){}
			
			e.getLink(ids[0]);
			
			while(l == null){}
			
			metadata m = l.getMetadata();
			Console.WriteLine(m.x1);
			Console.WriteLine(m.y1);
			Console.WriteLine(m.x2);
			Console.WriteLine(m.y2);
			Console.WriteLine(l.getComment());
			Console.WriteLine(l.getAppType());
			Console.WriteLine(l.getUri());
			
			Console.WriteLine("Waiting to open...");
			Thread.Sleep(5000);
			result = e.openLink(l.getNoteGuid());
			
			result.AsyncWaitHandle.WaitOne();
	    }
		
		
		private static void getCreateLinkResult(string data)
		{
			ids.Add(data);
		}
		
		private static void getLinkResult(slink link)
		{
			l = link;
		}
	}
}
