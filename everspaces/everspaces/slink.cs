using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace everspaces
{
	public struct metadata
	{
		public int x1;
		public int y1;
		public int x2;
		public int y2;
	}
	
	public class slink
	{
		private metadata meta;
		private string notebookGuid;
		private string noteGuid;
		private string title;
		private string comment;
		private List<Tuple<byte[], string>> resources;
		private List<string> tags;
		private string uri;
		private string appType;
		
		public slink(metadata meta)
		{
			this.meta = meta;
			notebookGuid = ""; // if empty; default notebook will be used.
			noteGuid = ""; // this will be set by the evernote server.
			title = "";
			comment = "";
			resources = null; // if the link is a Finder file, the file will be uploaded.
			tags = null;
			appType = ""; // this will be set by app request
			uri = ""; // this will be set by app request
		}
		
		public metadata getMetadata()
		{
			return meta;
		}
		
		public string getNotebookGuid()
		{
			return notebookGuid;
		}
		
		public string getNoteGuid()
		{
			return noteGuid;
		}
		
		public string getTitle()
		{
			return title;
		}
		
		public string getComment()
		{
			return comment;
		}
		
		public List<Tuple<byte[], string>> getResources()
		{
			return resources;
		}
		
		public List<String> getTags()
		{
			return tags;
		}
		
		public string getUri()
		{
			return uri;
		}
		
		public string getAppType()
		{
			return appType;
		}
		
		public void setNotebookGuid(string guid)
		{
			notebookGuid = guid;
		}
		
		public void setNoteGuid(string guid)
		{
			noteGuid = guid;
		}
		
		public void setTitle(string title)
		{
			this.title = title;
		}
		
		public void setComment(string comment)
		{
			this.comment = comment;
		}
		
		public void setResources(List<String> files)
		{
			if(resources == null)
				resources = new List<Tuple<byte[], string>>();
			
			foreach (String file in files)
			{
				try
				{
					string extension = Path.GetExtension(file);
					string mime = getMimeType(extension);
					byte[] data = ReadFully(File.OpenRead(file));
					resources.Add(new Tuple<byte[], string>(data, mime));
				}
				catch
				{
				}
			}
		}
		
		public void addResource(String file)
		{
			if(resources == null)
				resources = new List<Tuple<byte[], string>>();
			
			try
			{
				string extension = Path.GetExtension(file);
				string mime = getMimeType(extension);
				byte[] data = ReadFully(File.OpenRead(file));
				resources.Add(new Tuple<byte[], string>(data, mime));
			}
			catch
			{
			}
		}
		
		public void setResources(List<Tuple<byte[], string>> resources)
		{
			this.resources = resources;
		}
		
		public void setTags(List<String> tags)
		{
			this.tags = tags;
		}
		
		public void setAppType(string appType)
		{
			this.appType = appType;
		}
	
		public void setUri(string uri)
		{
			this.uri = uri;
		}
		
	   	private static byte[] ReadFully(Stream stream) 
		{
	        byte[] buffer = new byte[32768];
	        using (MemoryStream ms = new MemoryStream()) 
			{
	            while (true) 
				{
	              	int read = stream.Read (buffer, 0, buffer.Length);
	              	if (read <= 0) 
					{
	              	    return ms.ToArray();
	              	}
	              	ms.Write (buffer, 0, read);
	            }
	        }
		}
		
		private static Dictionary<String, String> mtypes = new Dictionary<string, string> {
	        {".pdf", "application/pdf" },
			{".gif", "image/gif"},
	        {".jpg", "image/jpeg"},
			{".jpeg", "image/jpeg"},
	        {".png", "image/png"},
			{".wav", "audio/wav"},
			{".mpg", "audio/mpeg"},
			{".mpeg", "audio/mpeg"},
			{".amr", "audio/amr"},
			{".mp4", "video/mp4"}
    	};
		
		private static string getMimeType(string extension)
		{
			string mimeType = "application/octet-stream";

			if(mtypes.ContainsKey(extension.ToLower()))
		    {
				mimeType = mtypes[extension.ToLower()];
			}
	
	        return mimeType;
		}
	}
}

