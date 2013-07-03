using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Security.Cryptography;
using Thrift;
using Thrift.Protocol;
using Thrift.Transport;
using Evernote.EDAM.Type;
using Evernote.EDAM.UserStore;
using Evernote.EDAM.NoteStore;
using Evernote.EDAM.Error;
using Evernote.EDAM.Limits;

namespace everspaces
{
	public class evernote
	{	
		//private String username;
		//private String password;
		//private String consumerKey;
		//private String consumerSecret;
		private String authToken;
		private NoteStore.Client noteStore;
		
		public evernote(String username, String password, String consumerKey, String consumerSecret)
		{
			//this.username = username;
			//this.password = password;
			//this.consumerKey = consumerKey;
			//this.consumerSecret = consumerSecret;
			
			String evernoteHost = "sandbox.evernote.com";
	        String edamBaseUrl = "https://" + evernoteHost;
	        
	        Uri userStoreUrl = new Uri(edamBaseUrl + "/edam/user");
	        TTransport userStoreTransport = new THttpClient(userStoreUrl);
	        TProtocol userStoreProtocol = new TBinaryProtocol(userStoreTransport);
	        UserStore.Client userStore = new UserStore.Client(userStoreProtocol);
	        
	        bool versionOK =
	            userStore.checkVersion("C# EDAMTest",
	        	   Evernote.EDAM.UserStore.Constants.EDAM_VERSION_MAJOR,
	        	   Evernote.EDAM.UserStore.Constants.EDAM_VERSION_MINOR);
	        if (!versionOK) 
			{
				throw new Exception("EDAM protocol out-of-date");
	        }
	        
	        AuthenticationResult authResult = null;
	        try 
			{
	            authResult = userStore.authenticate(username, password,
	                                                consumerKey, consumerSecret);
	        } 
			catch (EDAMUserException ex) 
			{
	            String parameter = ex.Parameter;
	            EDAMErrorCode errorCode = ex.ErrorCode;
	            
	            if (errorCode == EDAMErrorCode.INVALID_AUTH) 
				{
	                if (parameter == "consumerKey") 
					{
                        throw new Exception("Your consumer key was not accepted by " + evernoteHost);
	                } 
					else if (parameter == "username") 
					{
	                    throw new Exception("You must authenticate using a username and password from " + evernoteHost);
	                } 
					else if (parameter == "password") 
					{
	                    throw new Exception("The password that you entered is incorrect");
	                }
	            }
	        }
			
	        User user = authResult.User;
	        authToken = authResult.AuthenticationToken;
			
			Uri noteStoreUrl = new Uri(edamBaseUrl + "/edam/note/" + user.ShardId);
	        TTransport noteStoreTransport = new THttpClient(noteStoreUrl);
	        TProtocol noteStoreProtocol = new TBinaryProtocol(noteStoreTransport);
	        noteStore = new NoteStore.Client(noteStoreProtocol);

		}
		
		public List<Tuple<string, string>> listNotebooks()
		{
			List<Notebook> notebooks = noteStore.listNotebooks(authToken);
			List<Tuple<string, string>> nb = new List<Tuple<string, string>>();
			foreach (Notebook notebook in notebooks)
			{
				nb.Add(new Tuple<string, string>(notebook.Guid, notebook.Name));
			}
			return nb;
		}
		
		public slink getNote(String guid)
		{
			Note note = noteStore.getNote(authToken, guid, true, true, true, true);
			metadata meta = new metadata();
			
			meta.x1 = Convert.ToInt32(noteStore.getNoteApplicationDataEntry(authToken, guid, "xone"));
			meta.x2 = Convert.ToInt32(noteStore.getNoteApplicationDataEntry(authToken, guid, "xtwo"));
			meta.y1 = Convert.ToInt32(noteStore.getNoteApplicationDataEntry(authToken, guid, "yone"));
			meta.y2 = Convert.ToInt32(noteStore.getNoteApplicationDataEntry(authToken, guid, "ytwo"));
			
			
			slink link = new slink(meta);
		
			link.setNotebookGuid(note.NotebookGuid);
			link.setNoteGuid(note.Guid);
			link.setTitle(note.Title);
			link.setComment(note.Attributes.Source);
			link.setTags(note.TagNames);
			link.setAppType(note.Attributes.SourceApplication);
			link.setUri(note.Attributes.SourceURL);
			
			List<Tuple<byte[], string>> res = null;
			List<Resource> resources = note.Resources;
			if(resources != null)
			{
				res = new List<Tuple<byte[], string>>();
				foreach (Resource resource in resources)
				{
					res.Add(new Tuple<byte[], string>(resource.Data.Body, resource.Mime));
				}
			}
			link.setResources(res);
			
			return link;
		}
		
		public String createNote(slink link)
		{
			Note note = new Note();
			String content = "";
			
			note.Title = link.getTitle();
			
			if(link.getNotebookGuid() != "")
			{
				note.NotebookGuid = link.getNotebookGuid();
			}
			
			content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
            "<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">" +
            "<en-note>" + link.getComment();
			
			if(link.getResources() != null)
			{
				content += "<br/>";
				note.Resources = new List<Resource>();
				foreach (Tuple<byte[], string> resource in link.getResources())
				{
					byte[] file = resource.Item1;
		        	byte[] hash = new MD5CryptoServiceProvider().ComputeHash(file);
		        	string hashHex = BitConverter.ToString(hash).Replace("-", "").ToLower();
		        
		        	Data data = new Data();
		        	data.Size = file.Length;
		        	data.BodyHash = hash;
		        	data.Body = file;
		        
					string mime = resource.Item2;
		        	Resource res = new Resource();
		        	res.Mime = mime;
		        	res.Data = data;

	        		note.Resources.Add(res);
					 
					content += "<en-media type=\"" + mime + "\" hash=\"" + hashHex + "\"/><br/>"; 
				}
			}
            content += "</en-note>";
			note.Content = content;
			
			if(link.getTags() != null)
			{
				note.TagNames = link.getTags();
			}
			
			NoteAttributes attributes = new NoteAttributes();
			attributes.ContentClass = "ucsd.spaces";
			attributes.SourceURL = link.getUri();
			attributes.Source = link.getComment();
			attributes.SourceApplication = link.getAppType();
			note.Attributes = attributes;
			
			note = noteStore.createNote(authToken, note);
			
			metadata meta = new metadata();
			meta = link.getMetadata();
			
			noteStore.setNoteApplicationDataEntry(authToken, note.Guid, "xone", meta.x1.ToString());
			noteStore.setNoteApplicationDataEntry(authToken, note.Guid, "xtwo", meta.x2.ToString());
			noteStore.setNoteApplicationDataEntry(authToken, note.Guid, "yone", meta.y1.ToString());
			noteStore.setNoteApplicationDataEntry(authToken, note.Guid, "ytwo", meta.y2.ToString());
					
			return note.Guid;
		}
		                      
		
		public String createNotebook(String notebookName)
		{
			Notebook notebook = new Notebook();
			notebook.Name = notebookName;
			notebook = noteStore.createNotebook(authToken, notebook);
			return notebook.Guid;
		}
		
		public void deleteNote(String guid)
		{
			noteStore.expungeNote(authToken, guid);
		}
		
		public void deleteNotebook(String guid)
		{
			noteStore.expungeNotebook(authToken, guid);
		}
		
		public List<slink> searchNotes(string searchStr)
		{
			NoteFilter filter = new NoteFilter();
			filter.Words = searchStr;
			NoteList noteList = noteStore.findNotes(authToken, filter, 0, Evernote.EDAM.Limits.Constants.EDAM_USER_NOTES_MAX);
			List<slink> links = new List<slink>();
			foreach (Note note in noteList.Notes)
			{
				links.Add(getNote(note.Guid));
			}
			return links;
		}
		
		public List<slink> getAllNotes()
		{
			return searchNotes("*");
		}
	}
}

