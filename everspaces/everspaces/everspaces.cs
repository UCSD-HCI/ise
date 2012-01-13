using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.ComponentModel;
using System.Runtime.Remoting.Messaging;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace everspaces
{	
	public delegate void createLinkHandler(string data);
	public delegate void getAllLinksHandler(List<slink> links);
	public delegate void getLinkHandler(slink link);
	
	public class everspaces
	{
		private socket s;
		private evernote spaces; 
    	private string username = "myinteractivespace";
        private string password = "thisspaces";	
        private string consumerKey = "myinteractivespace";
        private string consumerSecret = "e1ec9b1b89983c94"; 
		
		private object _createLinkSync = new object();
		private bool _createLinkBusy = false;
		private object _getAllLinksSync = new object();
		private bool _getAllLinksBusy = false;
		private object _deleteLinkSync = new object();
		private bool _deleteLinkBusy = false;
		private object _getLinkSync = new object();
		private bool _getLinkBusy = false;
		private object _openLinkSync = new object();
		private bool _openLinkBusy = false;
		
		public event createLinkHandler createLinkCompleted;
		public event getAllLinksHandler getAllLinksCompleted;
		public event getLinkHandler getLinkCompleted;
		
		public everspaces(string host="128.54.38.41", int port=9999)
		{
			s = new socket(host, port);  
			spaces = new evernote(username, password, consumerKey, consumerSecret);
			/*
			s = new socket("www.xkcd.com", 80);
			string msg = "GET / HTTP/1.1\r\nHost: www.xkcd.com\r\n\r\n";
			s.write(msg);
			Console.WriteLine(s.read());
			*/
		}
		
		protected virtual void OnCreateLinkCompleted(string data) 
		{
			if(createLinkCompleted != null)
    			createLinkCompleted(data);
		}
		
		protected virtual void OnGetAllLinksCompleted(List<slink> data) 
		{
			if(getAllLinksCompleted != null)
    			getAllLinksCompleted(data);
		}
		
		protected virtual void OnGetLinkCompleted(slink data) 
		{
			if(getLinkCompleted != null)
    			getLinkCompleted(data);
		}
		
		public IAsyncResult createLink(slink link, bool appRequest=true)
		{
			IAsyncResult result;
	  		createLinkDelegate worker = new createLinkDelegate(createLinkWorker);
  			AsyncCallback completedCallback = new AsyncCallback(createLinkCallback);
			
			lock(_createLinkSync)
			{		
				if(_createLinkBusy)
					throw new Exception("Create Link is busy");
				
				AsyncOperation async = AsyncOperationManager.CreateOperation(null);
	    		result = worker.BeginInvoke(link, appRequest, completedCallback, async);
				_createLinkBusy = true;
			}
			
			return result;
		}
		
		private void createLinkCallback(IAsyncResult ar)
		{
			createLinkDelegate worker = (createLinkDelegate)((AsyncResult)ar).AsyncDelegate;
			
			string id = worker.EndInvoke(ar);
			
			lock(_createLinkSync)
			{
				_createLinkBusy = false;
			}
			
			OnCreateLinkCompleted(id);
		}
		
		private delegate string createLinkDelegate(slink link, bool appRequest=true);
		
		private string createLinkWorker(slink link, bool appRequest=true)
		{
			string appType = "";
			string uri = "";
			if(appRequest)
			{
				appType = appFocusRequest();
				uri = appUriRequest(appType);
				link.setAppType(appType);
				link.setUri(uri);
			}
			
            //modified by Wander
            if (appType == constants.APP_APPLE_SAFARI && uri != constants.UNKNOWN)
            {
                Trace.WriteLine("Linked to " + uri);
            }


			/*if(appType == constants.APP_APPLE_FINDER && uri != constants.UNKNOWN)
			{
				string path = uri;
				path = path.Replace("file://localhost", "");
				link.addResource(path);
			}*/
			
			return spaces.createNote(link);
		}
		
		public IAsyncResult getAllLinks()
		{
			IAsyncResult result;
	  		getAllLinksDelegate worker = new getAllLinksDelegate(getAllLinksWorker);
  			AsyncCallback completedCallback = new AsyncCallback(getAllLinksCallback);
			
			lock(_getAllLinksSync)
			{		
				if(_getAllLinksBusy)
					throw new Exception("Get All Link is busy");
				
				AsyncOperation async = AsyncOperationManager.CreateOperation(null);
	    		result = worker.BeginInvoke(completedCallback, async);
				_getAllLinksBusy = true;
			}
			
			return result;
		}
		
		private void getAllLinksCallback(IAsyncResult ar)
		{
			getAllLinksDelegate worker = (getAllLinksDelegate)((AsyncResult)ar).AsyncDelegate;
			
			List<slink> links = worker.EndInvoke(ar);
			
			lock(_getAllLinksSync)
			{
				_getAllLinksBusy = false;
			}
			
			OnGetAllLinksCompleted(links);
		}
		
		private delegate List<slink> getAllLinksDelegate();
		
		private List<slink> getAllLinksWorker()
		{
			return spaces.getAllNotes();
		}
		
		public IAsyncResult deleteLink(string id)
		{
			IAsyncResult result;	
	  		deleteLinkDelegate worker = new deleteLinkDelegate(deleteLinkWorker);
  			AsyncCallback completedCallback = new AsyncCallback(deleteLinkCallback);
			
			lock(_deleteLinkSync)
			{		
				if(_deleteLinkBusy)
					throw new Exception("Delete Link is busy");
				
				AsyncOperation async = AsyncOperationManager.CreateOperation(null);
	    		result = worker.BeginInvoke(id, completedCallback, async);
				_deleteLinkBusy = true;
			}
			
			return result;
		}
		
		private void deleteLinkCallback(IAsyncResult ar)
		{
			deleteLinkDelegate worker = (deleteLinkDelegate)((AsyncResult)ar).AsyncDelegate;
			
			worker.EndInvoke(ar);
			
			lock(_deleteLinkSync)
			{
				_deleteLinkBusy = false;
			}
		}
		
		private delegate void deleteLinkDelegate(string id);
		
		private void deleteLinkWorker(string id)
		{
			spaces.deleteNote(id);
		}
		
		public IAsyncResult getLink(string id)
		{
			IAsyncResult result;
	  		getLinkDelegate worker = new getLinkDelegate(getLinkWorker);
  			AsyncCallback completedCallback = new AsyncCallback(getLinkCallback);
			
			lock(_getLinkSync)
			{		
				if(_getLinkBusy)
					throw new Exception("Get Link is busy");
				
				AsyncOperation async = AsyncOperationManager.CreateOperation(null);
	    		result = worker.BeginInvoke(id, completedCallback, async);
				_getLinkBusy = true;
			}
			
			return result;
		}
		
		private void getLinkCallback(IAsyncResult ar)
		{
			getLinkDelegate worker = (getLinkDelegate)((AsyncResult)ar).AsyncDelegate;
			
			slink link = worker.EndInvoke(ar);
			
			lock(_getLinkSync)
			{
				_getLinkBusy = false;
			}
			
			OnGetLinkCompleted(link);
		}
		
		private delegate slink getLinkDelegate(string id);
		
		private slink getLinkWorker(string id)
		{
			return spaces.getNote(id);
		}
		
		public IAsyncResult openLink(string id)
		{
			IAsyncResult result;
	  		openLinkDelegate worker = new openLinkDelegate(openLinkWorker);
  			AsyncCallback completedCallback = new AsyncCallback(openLinkCallback);
			
			lock(_openLinkSync)
			{		
				if(_openLinkBusy)
					throw new Exception("Open Link is busy");
				
				AsyncOperation async = AsyncOperationManager.CreateOperation(null);
	    		result = worker.BeginInvoke(id, completedCallback, async);
				_openLinkBusy = true;
			}
			
			return result;
		}
		
		private void openLinkCallback(IAsyncResult ar)
		{
			openLinkDelegate worker = (openLinkDelegate)((AsyncResult)ar).AsyncDelegate;
			
			worker.EndInvoke(ar);
			
			lock(_openLinkSync)
			{
				_openLinkBusy = false;
			}
		}
		
		private delegate void openLinkDelegate(string id);
		
		private void openLinkWorker(string id)
		{
			slink link = getLinkWorker(id);
			string appType = link.getAppType();
			string uri = link.getUri();
			string openRequest;
			string openIdentifier;
			Dictionary<string, object> dataMap = new Dictionary<string, object>();
			Dictionary<string, object> map = new Dictionary<string, object>();
			string jsonObject;
			
			if(appType == constants.APP_APPLE_SAFARI)
			{
				openRequest = constants.REQUEST_SAFARIOPENURL;	
				openIdentifier = constants.IDENTIFIER_SAFARIOPENURL;
			}
			else if(appType == constants.APP_APPLE_FINDER)
			{
				openRequest = constants.REQUEST_FINDEROPENFILE;
				openIdentifier = constants.IDENTIFIER_FINDEROPENFILE;
			}
			else if(appType == constants.APP_APPLE_MAIL)
			{
				openRequest = constants.REQUEST_MAILOPENEMAIL;
				openIdentifier = constants.IDENTIFIER_MAILOPENEMAIL;
			}
			else if(appType == constants.APP_APPLE_SKIM)
			{
				openRequest = constants.REQUEST_SKIMOPENPDF;
				openIdentifier = constants.IDENTIFIER_SKIMOPENPDF;
			}
			else
			{
				Console.WriteLine("Cannot open application: {0}", appType);
				return;
			}
			
			dataMap.Add(openIdentifier, uri);
			map.Add("TYPE", openRequest);
			map.Add("DATA", dataMap);
			
			jsonObject = JsonConvert.SerializeObject(map);
			s.write(jsonObject + "\r");
		}
		
		private string appUriRequest(string appType)
		{
			string uri = constants.UNKNOWN;
			string request;
			string response;
			string identifier;
			Dictionary<string, object> map = new Dictionary<string, object>();
			string jsonObject;
			string jsonTxt;
			JObject reply;
			
			if(appType == constants.APP_APPLE_SAFARI)
			{
				request = constants.REQUEST_SAFARIURL;
				response = constants.RESPONSE_SAFARIURL;
				identifier = constants.IDENTIFIER_SAFARIURL;
			}
			else if(appType == constants.APP_APPLE_FINDER)
			{
				request = constants.REQUEST_FINDERFILE;
				response = constants.RESPONSE_FINDERFILE;
				identifier = constants.IDENTIFIER_FINDERFILE;
			}
			else if(appType == constants.APP_APPLE_MAIL)
			{
				request = constants.REQUEST_MAILEMAIL;
				response = constants.RESPONSE_MAILEMAIL;
				identifier = constants.IDENTIFIER_MAILEMAIL;
			}
			else if(appType == constants.APP_APPLE_SKIM)
			{
				request = constants.REQUEST_SKIMPDF;
				response = constants.RESPONSE_SKIMPDF;
				identifier = constants.IDENTIFIER_SKIMPDF;
			}
			else
			{
				Console.WriteLine("Unknown application type");
				return uri;
			}
		
			Console.WriteLine("Requesting: {0}", request);
			map.Add("TYPE", request);
			jsonObject = JsonConvert.SerializeObject(map);
			s.write(jsonObject + "\r");
			
			jsonTxt = s.read();
			reply = JObject.Parse(jsonTxt);
			string type = (string)reply["TYPE"];
			Console.WriteLine("Received: {0}", type);
			
			if(type == response)
			{
				JObject focus = (JObject)reply["DATA"];
				string data = (string)focus[identifier];
				uri = data;
			}
			else
			{
				Console.WriteLine("Unexpected response: {0}", type);
			}
			
			return uri;
		}
		
		private string appFocusRequest()
		{
			string appType = constants.UNKNOWN;
			Dictionary<string, object> map = new Dictionary<string, object>();
			string jsonObject;
			string jsonTxt;
			JObject reply;
			
			Console.WriteLine("Application focus request");
			map.Add("TYPE", constants.REQUEST_APPFOCUS);
			jsonObject = JsonConvert.SerializeObject(map);
			s.write(jsonObject + "\r");
			
			jsonTxt = s.read();
			reply = JObject.Parse(jsonTxt);
			string type = (string)reply["TYPE"];
			Console.WriteLine("Received: {0}", type);
			
			if(type == constants.RESPONSE_APPFOCUS)
			{
				JObject focus = (JObject)reply["DATA"];
				string data = (string)focus[constants.IDENTIFIER_APPFOCUS];
				appType = data;
			}
			else
			{
				Console.WriteLine("Unexpected response: {0}", type);
			}
			
			return appType;
		}
	}
}

