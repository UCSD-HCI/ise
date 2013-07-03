/*
  A simple Evernote API demo application that authenticates with the
  Evernote web service, lists all notebooks and notes in the user's account,
  and creates a simple test note in the default notebook.
  
  Before running this sample, you must change the API consumer key
  and consumer secret to the values that you received from Evernote.

  To compile (Unix Mono):
    ( cd ../../src ; \
      gmcs -recurse:'*.cs' -t:library -out:../sample/client/enthrift.dll )
    gmcs -lib:. -r:enthrift EDAMTest.cs
  
  To run (Unix Mono):
    mono EDAMTest.exe myuser mypass  
*/

using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Security.Cryptography;
using Thrift;
using Thrift.Protocol;
using Thrift.Server;
using Thrift.Transport;
using Evernote.EDAM.Type;
using Evernote.EDAM.UserStore;
using Evernote.EDAM.NoteStore;
using Evernote.EDAM.Error;

public class EDAMTest {
    public static void Main(string[] args) {

        String username = "myinteractivespace";
        String password = "thisspaces";

        // NOTE: You must change the consumer key and consumer secret to the 
        //       key and secret that you received from Evernote
        String consumerKey = "myinteractivespace";
        String consumerSecret = "e1ec9b1b89983c94";
        
        String evernoteHost = "sandbox.evernote.com";
        String edamBaseUrl = "https://" + evernoteHost;
        // If using Mono, see http://www.mono-project.com/FAQ:_Security
        
        Uri userStoreUrl = new Uri(edamBaseUrl + "/edam/user");
        TTransport userStoreTransport = new THttpClient(userStoreUrl);
        TProtocol userStoreProtocol = new TBinaryProtocol(userStoreTransport);
        UserStore.Client userStore = new UserStore.Client(userStoreProtocol);
        
        bool versionOK =
            userStore.checkVersion("C# EDAMTest",
        	   Evernote.EDAM.UserStore.Constants.EDAM_VERSION_MAJOR,
        	   Evernote.EDAM.UserStore.Constants.EDAM_VERSION_MINOR);
        Console.WriteLine("Is my EDAM protocol version up to date? " + versionOK);
        if (!versionOK) {
            return;
        }
        
        AuthenticationResult authResult = null;
        try {
            authResult = userStore.authenticate(username, password,
                                                consumerKey, consumerSecret);
        } catch (EDAMUserException ex) {
            String parameter = ex.Parameter;
            EDAMErrorCode errorCode = ex.ErrorCode;
            
            Console.WriteLine("Authentication failed (parameter: " + parameter + " errorCode: " + errorCode + ")");
            
            if (errorCode == EDAMErrorCode.INVALID_AUTH) {
                if (parameter == "consumerKey") {
                    if (consumerKey == "en-edamtest") {
                        Console.WriteLine("You must replace the variables consumerKey and consumerSecret with the values you received from Evernote.");
                    } else {
                        Console.WriteLine("Your consumer key was not accepted by " + evernoteHost);
                        Console.WriteLine("This sample client application requires a client API key. If you requested a web service API key, you must authenticate using OAuth");
                    }
                    Console.WriteLine("If you do not have an API Key from Evernote, you can request one from http://www.evernote.com/about/developer/api");
                } else if (parameter == "username") {
                    Console.WriteLine("You must authenticate using a username and password from " + evernoteHost);
                    if (evernoteHost == "www.evernote.com" == false) {
                        Console.WriteLine("Note that your production Evernote account will not work on " + evernoteHost + ",");
                        Console.WriteLine("you must register for a separate test account at https://" + evernoteHost + "/Registration.action");
                    }
                } else if (parameter == "password") {
                    Console.WriteLine("The password that you entered is incorrect");
                }
            }
            
            return;
        }
        
        User user = authResult.User;
        String authToken = authResult.AuthenticationToken;
        Console.WriteLine("Authentication successful for: " + user.Username);
        Console.WriteLine("Authentication token = " + authToken);
        
        Uri noteStoreUrl = new Uri(edamBaseUrl + "/edam/note/" + user.ShardId);
        TTransport noteStoreTransport = new THttpClient(noteStoreUrl);
        TProtocol noteStoreProtocol = new TBinaryProtocol(noteStoreTransport);
        NoteStore.Client noteStore = new NoteStore.Client(noteStoreProtocol);
        
        List<Notebook> notebooks = noteStore.listNotebooks(authToken);
        Console.WriteLine("Found " + notebooks.Count + " notebooks:");
        Notebook defaultNotebook = notebooks[0];
        foreach (Notebook notebook in notebooks) {
            Console.WriteLine("  * " + notebook.Name);
            if (notebook.DefaultNotebook) {
                defaultNotebook = notebook;
            }
        }
        
        Console.WriteLine();
        Console.WriteLine("Creating a note in the default notebook: " +
                          defaultNotebook.Name);
        Console.WriteLine();
        
        byte[] image = ReadFully(File.OpenRead("enlogo.png"));
        byte[] hash = new MD5CryptoServiceProvider().ComputeHash(image);
        string hashHex = BitConverter.ToString(hash).Replace("-", "").ToLower();
        
        Data data = new Data();
        data.Size = image.Length;
        data.BodyHash = hash;
        data.Body = image;
        
        Resource resource = new Resource();
        resource.Mime = "image/png";
        resource.Data = data;
        
        Note note = new Note();
        note.NotebookGuid = defaultNotebook.Guid;
        note.Title = "Test note from EDAMTest.cs";
        note.Content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
            "<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">" +
            "<en-note>Here's the Evernote logo:<br/>" +
            "<en-media type=\"image/png\" hash=\"" + hashHex + "\"/>" +
            "</en-note>";

        note.Resources = new List<Resource>();
        note.Resources.Add(resource);
        
        Note createdNote = noteStore.createNote(authToken, note);
        
        Console.WriteLine("Successfully created new note with GUID: " + createdNote.Guid);
    }
      
    public static byte[] ReadFully(Stream stream) {
        byte[] buffer = new byte[32768];
        using (MemoryStream ms = new MemoryStream()) {
            while (true) {
              	int read = stream.Read (buffer, 0, buffer.Length);
              	if (read <= 0) {
              	    return ms.ToArray();
              	}
              	ms.Write (buffer, 0, read);
            }
        }
    }
}
