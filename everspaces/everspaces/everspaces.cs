using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.ComponentModel;
using System.Runtime.Remoting.Messaging;
using System.Text.RegularExpressions;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace everspaces
{	
	public delegate void createLinkHandler(string data);
	public delegate void getAllLinksHandler(List<slink> links);
	public delegate void searchLinksHandler(List<slink> links);
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
		private object _searchLinksSync = new object();
		private bool _searchLinksBusy = false;
		private object _deleteLinkSync = new object();
		private bool _deleteLinkBusy = false;
		private object _getLinkSync = new object();
		private bool _getLinkBusy = false;
		private object _openLinkSync = new object();
		private bool _openLinkBusy = false;
		
		private bool _connected = true;
		
		public event createLinkHandler createLinkCompleted;
		public event getAllLinksHandler getAllLinksCompleted;
		public event getLinkHandler getLinkCompleted;
		public event searchLinksHandler searchLinksCompleted;
		
		public everspaces(string host="localhost", int port=9999)
		{
			try
			{
				s = new socket(host, port);  
			}
			catch
			{
				_connected = false;
			}
			
			spaces = new evernote(username, password, consumerKey, consumerSecret);
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

		protected virtual void OnSearchLinksCompleted(List<slink> data) 
		{
			if(searchLinksCompleted != null)
    			searchLinksCompleted(data);
		}
		
		protected virtual void OnGetLinkCompleted(slink data) 
		{
			if(getLinkCompleted != null)
    			getLinkCompleted(data);
		}
		
		public IAsyncResult createLink(slink link, bool upload=false)
		{
			IAsyncResult result;
	  		createLinkDelegate worker = new createLinkDelegate(createLinkWorker);
  			AsyncCallback completedCallback = new AsyncCallback(createLinkCallback);
			
			lock(_createLinkSync)
			{		
				if(_createLinkBusy)
					throw new Exception("Create Link is busy");
				
				AsyncOperation async = AsyncOperationManager.CreateOperation(null);
	    		result = worker.BeginInvoke(link, upload, completedCallback, async);
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
		
		private delegate string createLinkDelegate(slink link, bool upload=false);
		
		private string createLinkWorker(slink link, bool upload=false)
		{
			string appType = constants.UNKNOWN;
			string uri = constants.UNKNOWN;
			if(_connected)
			{
				appType = appFocusRequest();
				uri = appUriRequest(appType);
			}
			
			link.setAppType(appType);
			link.setUri(uri);
			
            //modified by Wander
            if (appType == constants.APP_APPLE_SAFARI && uri != constants.UNKNOWN)
            {
                Trace.WriteLine("Linked to " + uri);
            }


			if((appType == constants.APP_APPLE_FINDER || appType == constants.APP_WIN_EXPLORER) 
			   	&& uri != constants.UNKNOWN && upload)
			{
				string path;
				string request;
				string identifier;
				
				if(appType == constants.APP_APPLE_FINDER)
				{
					path = Regex.Replace(uri, "file://.*?/", "/");
					request = constants.REQUEST_FINDERUPLOAD;
					identifier = constants.IDENTIFIER_FINDERUPLOAD;
				}
				else
				{
					path = uri;
					request = constants.REQUEST_EXPLORERUPLOAD;
					identifier = constants.IDENTIFIER_EXPLORERUPLOAD;
				}
				
				Dictionary<string, object> dataMap = new Dictionary<string, object>();
				Dictionary<string, object> map = new Dictionary<string, object>();
				string jsonObject;
				
				dataMap.Add(identifier, path);
				map.Add("TYPE", request);
				map.Add("DATA", dataMap);
			
				Console.WriteLine ("Requesting: {0}", request);
				jsonObject = JsonConvert.SerializeObject(map);
				s.write(jsonObject + "\r");
				
				string textData = s.readData();
				byte[] data = System.Text.Encoding.Unicode.GetBytes(textData);
				string extension = Path.GetExtension(path);
				string mime = slink.getMimeType(extension);
				link.addResource(data, mime);
			}
			
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
		
		public IAsyncResult searchLinks(string searchStr)
		{
			IAsyncResult result;
	  		searchLinksDelegate worker = new searchLinksDelegate(searchLinksWorker);
  			AsyncCallback completedCallback = new AsyncCallback(searchLinksCallback);
			
			lock(_searchLinksSync)
			{		
				if(_searchLinksBusy)
					throw new Exception("Get All Link is busy");
				
				AsyncOperation async = AsyncOperationManager.CreateOperation(null);
	    		result = worker.BeginInvoke(searchStr, completedCallback, async);
				_searchLinksBusy = true;
			}
			
			return result;
		}
		
		private void searchLinksCallback(IAsyncResult ar)
		{
			searchLinksDelegate worker = (searchLinksDelegate)((AsyncResult)ar).AsyncDelegate;
			
			List<slink> links = worker.EndInvoke(ar);
			
			lock(_searchLinksSync)
			{
				_searchLinksBusy = false;
			}
			
			OnSearchLinksCompleted(links);
		}
		
		private delegate List<slink> searchLinksDelegate(string searchStr);
		
		private List<slink> searchLinksWorker(string searchStr)
		{
			return spaces.searchNotes(searchStr);
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
			if(!_connected)
			{
				Console.WriteLine("Cannot open application: not connected to Plink");
				return;
			}
			
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
			else if(appType == constants.APP_WIN_FIREFOX)
			{
				openRequest = constants.REQUEST_FIREFOXOPENURL;
				openIdentifier = constants.IDENTIFIER_FIREFOXOPENURL;
			}
			else if(appType == constants.APP_WIN_IEXPLORE)
			{
				openRequest = constants.REQUEST_IEXPLOREOPENURL;
				openIdentifier = constants.IDENTIFIER_IEXPLOREOPENURL;
			}
			else if(appType == constants.APP_WIN_CHROME)
			{
				openRequest = constants.REQUEST_CHROMEOPENURL;
				openIdentifier = constants.IDENTIFIER_CHROMEOPENURL;
			}
			else if(appType == constants.APP_WIN_EXPLORER)
			{
				openRequest = constants.REQUEST_EXPLOREROPENFILE;
				openIdentifier = constants.IDENTIFIER_EXPLOREROPENFILE;
			}
			else if(appType == constants.APP_WIN_ADOBE)
			{
				openRequest = constants.REQUEST_ADOBEOPENINFO;
				openIdentifier = constants.IDENTIFIER_ADOBEOPENINFO;
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
			else if(appType == constants.APP_WIN_FIREFOX)
			{
				request = constants.REQUEST_FIREFOXURL;
				response = constants.RESPONSE_FIREFOXURL;
				identifier = constants.IDENTIFIER_FIREFOXURL;
			}
			else if(appType == constants.APP_WIN_IEXPLORE)
			{
				request = constants.REQUEST_IEXPLOREURL;
				response = constants.RESPONSE_IEXPLOREURL;
				identifier = constants.IDENTIFIER_IEXPLOREURL;
			}
			else if(appType == constants.APP_WIN_CHROME)
			{
				request = constants.REQUEST_CHROMEURL;
				response = constants.RESPONSE_CHROMEURL;
				identifier = constants.IDENTIFIER_CHROMEURL;
			}
			else if(appType == constants.APP_WIN_EXPLORER)
			{
				request = constants.REQUEST_EXPLORERFILE;
				response = constants.RESPONSE_EXPLORERFILE;
				identifier = constants.IDENTIFIER_EXPLORERFILE;
			}
			else if(appType == constants.APP_WIN_ADOBE)
			{
				request = constants.REQUEST_ADOBEINFO;
				response = constants.RESPONSE_ADOBEINFO;
				identifier = constants.IDENTIFIER_ADOBEINFO;
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
			
			Console.WriteLine("Requesting: {0}", constants.REQUEST_APPFOCUS);
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

