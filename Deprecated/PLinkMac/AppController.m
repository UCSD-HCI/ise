//
//  AppController.m
//  RMD(mac)
//
//  Created by Christian Leonhard Corsten on 15.09.10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

/**
 @brief PLink:mac main class file
 
 This class contains all basic methods needed for PLink:mac.
 */

#import "AppController.h"

NSString * const kNotification        = @"kNotification";
NSString * const kNotificationMessage = @"kNotificationMessage";

@implementation AppController

//---------------------------------------------------------------------------------
#pragma mark Properties
//---------------------------------------------------------------------------------
@synthesize RMDJavaPid;

//---------------------------------------------------------------------------------
#pragma mark Init & Dealloc
//---------------------------------------------------------------------------------
/**
 *  initializes all non-GUI stuff, such as running the Java source, setting up ScriptingBridge and setting up the socket
 *  @return an instance of this class
 */
- (id)init
{
  self = [super init];
  if (self != nil)
  {
    //Retrieve external properties from the 'Info.plist'
    NSString *path = [[NSBundle mainBundle] bundlePath];
    NSString *finalPath = [path stringByAppendingPathComponent:@"Contents/Info.plist"];
    plistDict = [[NSDictionary dictionaryWithContentsOfFile:finalPath] retain];
    
    BOOL externalJavaLaunch;
    
    @try
    {
      externalJavaLaunch = [[plistDict valueForKey:@"External Java Launch"] boolValue];
    }
    @catch (NSException * e)
    {
      NSLog(@"Could not retrieve property 'External Java Launch' from application's plist file.");
      externalJavaLaunch = NO;
    }
    
    if(!externalJavaLaunch)
    {
      //launch the RMD Java client (located in RefreshMyDesk.zip) 
      [self launchRDMJava];
    }
 
    notificationCenter = [NSNotificationCenter defaultCenter];
    
    //initialize the JSON parser
    parser = [[SBJsonParser alloc] init];
    
    //enable 'Finder' support (bundle identifier is "com.apple.finder")
    finderIdentifier = FINDERBUNDLEIDENTIFIER;
    finder  = [SBApplication applicationWithBundleIdentifier:finderIdentifier];
    
    //enable 'Mail' support (bundle identifier is "com.apple.mail")
    mailIdentifier = MAILBUNDLEIDENTIFIER;
    mail  = [SBApplication applicationWithBundleIdentifier:mailIdentifier];
    
    //enable 'Safari' support (bundle identifier is "com.apple.safari")
    safariIdentifier = SAFARIBUNDLEDENTIFIER;
    safari  = [SBApplication applicationWithBundleIdentifier:safariIdentifier];
    
    //enable 'Skim' support for PDF viewing (bundle identifier is "net.sourceforge.skim-app.skim")
    skimIdentifier = SKIMBUNDLEIDENTIFIER;
    skim = [SBApplication applicationWithBundleIdentifier:skimIdentifier];
    
    //enable 'Chrome' support (bundle identifier is "com.google.Chrome")
    chromeIdentifier = CHROMEBUNDLEIDENTIFIER;
    chrome = [self chromeInstance];
    
    //for an unkown application type
    unknownIdentifier = UNKNOWNBUNDLEIDENTIFIER;
    //for an unknown object identifier
    unknownObjectIdentifier = @"unknown";
    
    
    //we will store the RMDJava process id here to kill the process when RMD(mac) terminates
    RMDJavaPid = 0;
    
    fetchedURLs = [[NSMutableDictionary alloc] init];
    URLDates = [[NSMutableDictionary alloc] init];
    
    //get socket properties
    @try
    {
      socketHost = [plistDict valueForKey:@"Socket Host"];
      socketPort = [[plistDict valueForKey:@"Socket Port"] intValue];
    }
    @catch (NSException * e)
    {
      socketHost = @"localhost";
      socketPort = 9999;
    }
   
    //Server Socket
    listenSocket = [[AsyncSocket alloc] initWithDelegate:self];
		connectedSockets = [[NSMutableArray alloc] initWithCapacity:1];
		itIsRunning = NO;
    
    //enable socket listening
    [self startStop];
  }
  
  return self;
}

/**
 *  initializes all GUI stuff, i.e., loads the setting values from the .plist and displays them in the corresponding views
 */
- (void)awakeFromNib
{
  [progressIndicator startAnimation:nil];
  
  [mainWindow setDelegate:self];
  
  //load plist values to preferences window
  if (!plistDict)
  {
    NSString *path = [[NSBundle mainBundle] bundlePath];
    NSString *finalPath = [path stringByAppendingPathComponent:@"Contents/Info.plist"];
    plistDict = [[NSDictionary dictionaryWithContentsOfFile:finalPath] retain];
  }
  
  @try
  {
    [socketHostField setStringValue:[plistDict valueForKey:@"Socket Host"]];
    [socketPortField setIntValue:[[plistDict valueForKey:@"Socket Port"] intValue]];
    [externalLaunchCheckbox setState:[[plistDict valueForKey:@"External Java Launch"] boolValue]];
    [reloadWebsitesCheckbox setState:[[plistDict valueForKey:@"Reload Websites"] boolValue]];
    [gmailUsernameField setStringValue:[plistDict valueForKey:@"GMail Username"]];
    [gmailButton setHidden:![[plistDict valueForKey:@"Show GMail Button"] boolValue]];
  }
  @catch (NSException * e)
  {
    NSLog(@"Could not read properties from application's 'Info.plist'");
    //default properties
    [socketHostField setStringValue:@"localhost"];
    [socketPortField setIntValue:9999];
    [externalLaunchCheckbox setState:YES];
    [reloadWebsitesCheckbox setState:NO];
  }
}

/**
 *  deallocates the class variables
 */
- (void)dealloc
{
  [super dealloc];
  [self startStop];
  [listenSocket release];
  [listenSocket dealloc];
  [connectedSockets release];
  [connectedSockets dealloc];
  
  [parser release];
  [parser dealloc];
  [notificationCenter release];
  [notificationCenter dealloc];
  
  [safari release];
  [safari dealloc];
  [finder release];
  [finder dealloc];
  [mail release];
  [mail dealloc];
  [skim dealloc];
  [skim release];
  
  [rmdJavaTask release];
  [rmdJavaTask dealloc];
  
  [progressIndicator release];
  [progressIndicator dealloc];
  [statusField release];
  [statusField dealloc];
  [mainWindow release];
  [mainWindow dealloc];
  
  [preferencesWindow release];
  [preferencesWindow dealloc];
  [socketHostField release];
  [socketHostField dealloc];
  [socketPortField release];
  [socketPortField dealloc];
  [externalLaunchCheckbox release];
  [externalLaunchCheckbox dealloc];
  [gmailUsernameField release];
  [gmailUsernameField dealloc];
  [reloadWebsitesCheckbox release];
  [reloadWebsitesCheckbox dealloc];
  
  [gmailButton release];
  [gmailButton dealloc];
  
  [plistDict release];
  [fetchedURLs release];
  [URLDates release];
}


//---------------------------------------------------------------------------------
#pragma mark Action methods for 'Finder'
//---------------------------------------------------------------------------------
/**
 *  opens a given file in Finder
 *  @param fileIdentificationName a filename URL
 */
- (void) finderOpenFile:(NSString*)fileIdentificationName
{
  //we do not need to check if Finder is running, because it is always running
  
  //get a FinderItem instance of the given URL
  @try
  {
    FinderItem *theItem = [[finder items] objectAtLocation: [NSURL URLWithString:fileIdentificationName]];
    [theItem openUsing:finder withProperties:nil];
    
    [self applicationToFrontForBundleIdentifier:finderIdentifier];
  }
  @catch (NSException * e)
  {
    NSLog(@"The file could not be opened.");
  }
}

//---------------------------------------------------------------------------------
#pragma mark Action methods for 'Mail'
//---------------------------------------------------------------------------------
/**
 *  opens a given mail in Mail (inperformant)
 *  @param mailIdentificationName a string id for a mail as used by Apple Mail
 */
- (void) mailOpenEmail:(NSString*)mailIdentificationName
{
  BOOL successfulLaunch;
  
  if(![mail isRunning])
  {
    successfulLaunch = [self launchApplicationWithBundleIdentifier:mailIdentifier];    
  }
  
  if(successfulLaunch || [mail isRunning])
  {
    //find the mail matching the messageId
    for(MailAccount* mailAccount in [[mail accounts] get])
    {
      for(MailMailbox* mailMailBox in [[mailAccount mailboxes] get])
      {
        for(MailMessage* mailMessage in [[mailMailBox messages] get])
        {
          if([[mailMessage messageId] isEqualToString:mailIdentificationName])
          {
            /*
            //hide all other applications and just show the email window with the retrieved message
            NSArray *theApps = [[NSWorkspace sharedWorkspace] runningApplications];
            for(NSRunningApplication *runningApp in theApps)
            {
              if(![[runningApp bundleIdentifier] isEqualToString:mailIdentifier])
              {
                [runningApp hide];
              }
            }
            
            NSArray *apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:mailIdentifier];
            for(NSRunningApplication *runningApp in apps)
            {
              [runningApp activateWithOptions:NSApplicationActivateAllWindows];
            }
            */
            
            @try
            {
              [mailMessage open];
              
              [self applicationToFrontForBundleIdentifier:mailIdentifier];
            }
            @catch (NSException * e)
            {
              NSLog(@"The email could not be opened.");
            }
            
            return;
          }
        }
      }
    }
  }
  
  return;
}

//---------------------------------------------------------------------------------
#pragma mark Action methods for 'Safari'
//---------------------------------------------------------------------------------
/**
 *  opens a given URL in Safari
 *  @param URLIdentificationName a URL
 */
- (void) safariOpenURL:(NSString*)URLIdentificationName
{
  BOOL successfulLaunch;
  
  if(![safari isRunning])
  {
    successfulLaunch = [self launchApplicationWithBundleIdentifier:safariIdentifier];    
  }
  
  if(successfulLaunch || [safari isRunning])
  {
    BOOL reload;
    
    if([reloadWebsitesCheckbox state] == YES)
    {
      reload = YES;
    }
    else
    {
      reload = ![self didLoadURL:URLIdentificationName];
    }

    BOOL needsRefresh = [self needRefreshForURL:URLIdentificationName];
    
    if(needsRefresh == YES)
    {
      //update the fetch date
      NSDate *date = [NSDate date];
      [fetchedURLs setValue:date forKey:URLIdentificationName];
      [self webDateForURL:URLIdentificationName];
    }
    
    if(reload || needsRefresh)
    {
      NSDictionary* errorDict;
      NSAppleEventDescriptor* returnDescriptor = NULL;
      
      //AppleScript code for opening a URL in Safari
      NSString* scriptCode = [NSString stringWithFormat:@"\
                              tell application \"Safari\"\n\
                              open location \"%@\"\n\
                              end tell", URLIdentificationName];
      
      NSAppleScript* scriptObject = [[NSAppleScript alloc] initWithSource: scriptCode];
      
      @try
      {
        returnDescriptor = [scriptObject executeAndReturnError: &errorDict];
      }
      @catch (NSException * e)
      {
        NSLog(@"Could not execute script for calling the website.");
      }
      
      [scriptObject release];
    }
    
    //bring Safari to foreground
    [self applicationToFrontForBundleIdentifier:safariIdentifier];
  }
}

//---------------------------------------------------------------------------------
#pragma mark Action methods for 'Skim'
//---------------------------------------------------------------------------------
/**
 *  opens a given PDF file with SkimPDF
 *  @param PDFIdentificationName a filename URL of the PDF
 */
- (void)skimOpenPDF:(NSString*)PDFIdentificationName
{
  //Skim will always open automatically (due to the 'open' command)
  @try
  {
    //first of all, we need to parse the Skim PDF identification name
    //it consists of the following format: URL:pagenumber
    
    NSString *URL;
    int       pageNumber;
    
    NSString *formattedString = [PDFIdentificationName stringByReplacingOccurrencesOfString:@"file://" withString:@""];
    NSArray *stringChunks = [formattedString componentsSeparatedByString:@":"];
    
    URL = [@"file://" stringByAppendingString:[stringChunks objectAtIndex:0]];
    pageNumber = [[stringChunks objectAtIndex:1] intValue];
    
    NSLog(@"Skim PDF file: %@ at page: %d", URL, pageNumber);
    
    //now open the PDF with Skim at the given pageNumber
    NSURL *fileURL = [NSURL URLWithString:URL];
    
    SkimDocument *doc = [skim open:fileURL];
    NSArray* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:skimIdentifier];
    [(NSRunningApplication*)[apps objectAtIndex:0] activateWithOptions:NSApplicationActivateIgnoringOtherApps];
    [doc setCurrentPage:[[[doc pages] get] objectAtIndex:pageNumber - 1]];
  }
  @catch (NSException * e)
  {
    NSLog(@"The file could not be opened.");
  }
}

- (void)chromeOpenURL:(NSString*)URLIdentificationName
{
  //create a new JSON package containing the URL
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];
  
  [jsonData setObject:URLIdentificationName forKey:CHROMEURLIDENTIFIER];
  
  [jsonObject setValue:CHROMCHROMEOPENURLREQUEST forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];
  
  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  //TODO: send via different socket!
  [self sendMessage:jsonString];
  
}


//---------------------------------------------------------------------------------
#pragma mark Socket Methods
//---------------------------------------------------------------------------------
/**
 *  toggles the server socket state on/off
 */
- (void)startStop
{
	if(!itIsRunning)
	{
		int port = socketPort;
		
		if(port < 0 || port > 65535)
		{
			port = 0;
		}
		
		NSError *error = nil;
		if(![listenSocket acceptOnPort:port error:&error])
		{
			[self logInfo:FORMAT(@"Error starting server: %@", error)];
			return;
		}
		
		[self logInfo:FORMAT(@"Echo server started on port %hu", [listenSocket localPort])];
		itIsRunning = YES;
	}
	else
	{
		// Stop accepting connections
		[listenSocket disconnect];
    
		// Stop any client connections
		int i;
		for(i = 0; i < [connectedSockets count]; i++)
		{
			// Call disconnect on the socket,
			// which will invoke the onSocketDidDisconnect: method,
			// which will remove the socket from the list.
			[[connectedSockets objectAtIndex:i] disconnect];
		}
		
		[self logInfo:@"Stopped Echo server"];
		itIsRunning = false;
	}
}

/**
 *  sends a message to receiver via socket
 *  @param message a message to be sent
 */
- (void)sendMessage:(NSString *)message
{
  NSData *data = [[message stringByAppendingString:@"\r\n"] dataUsingEncoding:NSUTF8StringEncoding];
  for(AsyncSocket *sock in connectedSockets)
  {
    [sock writeData:data withTimeout:-1 tag:1];
  }
}

- (void)sendData:(NSData *)data
{
	for(AsyncSocket *sock in connectedSockets)
	{
		[sock writeData:data withTimeout:-1 tag:1];
	}
}

//---------------------------------------------------------------------------------
#pragma mark Socket Methods >> Delegate Methods
//---------------------------------------------------------------------------------
/**
 *  adds the socket to the list of connected sockets if the connection was successful (callback method)
 *  @param sock a socket
 *  @param newSocket the new socket to be added
 */
- (void)onSocket:(AsyncSocket *)sock didAcceptNewSocket:(AsyncSocket *)newSocket
{
	[connectedSockets addObject:newSocket];
}

/**
 *  called when the socket did connect to its host (callback method)
 *  @param sock a socket
 *  @param host a host
 *  @param port a port
 */
- (void)onSocket:(AsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port
{
	[self logInfo:FORMAT(@"Accepted client %@:%hu", host, port)];
	
	//NSString *welcomeMsg = @"Welcome to the RMD:mac server\r\n";
	//NSData *welcomeData = [welcomeMsg dataUsingEncoding:NSUTF8StringEncoding];
	
	//[sock writeData:welcomeData withTimeout:-1 tag:WELCOME_MSG];
	[sock readDataToData:[AsyncSocket CRLFData] withTimeout:-1 tag:0];
}

/**
 *  This method is called if a read has timed out.
 *  It allows us to optionally extend the timeout.
 *  We use this method to issue a warning to the user prior to disconnecting them.
 *  @param sock a socket
 *  @param tag a tag
 *  @param elapsed the elapsed time
 *  @param length bytes done
 *  @return a time interval
 */
- (NSTimeInterval)onSocket:(AsyncSocket *)sock
  shouldTimeoutReadWithTag:(long)tag
                   elapsed:(NSTimeInterval)elapsed
                 bytesDone:(CFIndex)length
{
	if(elapsed <= READ_TIMEOUT)
	{
		NSString *warningMsg = @"Are you still there?\r\n";
		NSData *warningData = [warningMsg dataUsingEncoding:NSUTF8StringEncoding];
		
		[sock writeData:warningData withTimeout:-1 tag:WARNING_MSG];
		
		return READ_TIMEOUT_EXTENSION;
	}
	
	return 0.0;
}

/**
 *  called when the socket will be disconnecting (callback method)
 *  @param sock a socket
 *  @param err a received error message
 */
- (void)onSocket:(AsyncSocket *)sock willDisconnectWithError:(NSError *)err
{
	[self logInfo:FORMAT(@"Client Disconnected: %@:%hu", [sock connectedHost], [sock connectedPort])];
}

/**
 *  called when the socket did disconnect (delegate method)
 *  @param sock a socket
 */
- (void)onSocketDidDisconnect:(AsyncSocket *)sock
{
	[connectedSockets removeObject:sock];
}

/**
 *  called when the socket did write data (delegate method)
 *  @param sock a socket
 *  @param tag a tag
 */
- (void)onSocket:(AsyncSocket *)sock didWriteDataWithTag:(long)tag
{
	if(tag == ECHO_MSG)
	{
		[sock readDataToData:[AsyncSocket CRLFData] withTimeout:-1 tag:0];
	}
}

/**
 *  called when the socket did read data (callback method)
 *  This method follows a strict communication protocol for communicating with the Java core.
 *  @param sock a socket
 *  @param data received data
 *  @param tag a tag
 */
- (void)onSocket:(AsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
  //NSLog(@"SOCKET: Incoming data... %@", data);
  
  NSData *truncatedData  = [data subdataWithRange:NSMakeRange(0, [data length] - 1)];
  NSString *json_string  = [[NSString alloc] initWithData:truncatedData encoding:NSUTF8StringEncoding];
  json_string = [json_string stringByReplacingOccurrencesOfString:@"\r\n" withString:@""];
  NSDictionary *jsonDict = [parser objectWithString:json_string error:nil];
  
  NSString *typeString   = [jsonDict objectForKey:@"TYPE"];
  
  //a simple protocol for communication between java server <-> mac client
  //handle the case for request/response type
 
  //the currently selected Finder file is requested
  if([typeString isEqualToString:FINDERFILEREQUEST])
  {
    NSLog(@"SOCKET: <FinderFileREQUEST> received.");
    
    //response
    [self handleFinderFileRequest];
    
    NSLog(@"SOCKET: <FinderFileRESPONSE> sent.");
  }
  
  else if([typeString isEqualToString:MAILEMAILREQUEST])
  {
    NSLog(@"SOCKET: <MailEmailREQUEST> received.");
    
    //response
    [self handleMailEmailRequest];
       
    NSLog(@"SOCKET: <MailEmailRESPONSE> sent.");
  }
  
  else if([typeString isEqualToString:SAFARIURLREQUEST])
  {
    NSLog(@"SOCKET: <SafariURLREQUEST> received.");
    
    //response
    [self handleSafariURLRequest];
      
    NSLog(@"SOCKET: <SafariURLRESPONSE sent.");
  }
  
  else if([typeString isEqualToString:SKIMPDFREQUEST])
  {
    NSLog(@"SOCKET: <SkimPDFREQUEST> received.");
    
    //response
    [self handleSkimPDFRequest];
    
    NSLog(@"SOCKET: <SkimPDFRESPONSE sent.");
  }
  
  else if([typeString isEqualToString:CHROMEURLREQUEST])
  {
    NSLog(@"SOCKET: <ChromeURLREQUEST> received.");
    
    //response
    [self handleChromeURLRequest];
    
    NSLog(@"SOCKET: <ChromeURLRESPONSE> sent.");
  }
  
  else if([typeString isEqualToString:CHROMEOPENURLREQUEST])
  {
    NSLog(@"SOCKET: <ChromeOpenURLREQUEST> received.");
    
    NSString *URLString = [[jsonDict objectForKey:@"DATA"] objectForKey:CHROMEOPENURLIDENTIFIER];
    
    [self chromeOpenURL:URLString];
  }
  
  //Chrome has sent a URL, hence store it locally
  else if([typeString isEqualToString:CHROMEURLPUSH])
  {
    NSLog(@"SOCKET: <ChromeURLPush> received.");

    chromeTabURL = [[jsonDict objectForKey:@"DATA"] objectForKey:CHROMEURLIDENTIFIER];
  }
  
  else if([typeString isEqualToString:APPLICATIONFOCUSREQUEST])
  {
    NSLog(@"SOCKET: <AppFocusREQUEST> received.");
    
    //response
    [self handleAppFocusRequest];
  
    NSLog(@"SOCKET: <AppFocusRESPONSE> sent.");
  }
  
  
  else if([typeString isEqualToString:OBJECTPATHREQUEST])
  {
    NSLog(@"<SOCKET: <ObjectPathREQUEST> received.");

    //response
    [self handleObjectPathRequest];
    
    NSLog(@"SOCKET: <ObjectPathRESPONSE> sent.");
  }
  
  else if([typeString isEqualToString:SAFARIOPENURLREQUEST])
  {
    NSLog(@"SOCKET: <SafariOpenURLREQUEST> received.");
    
    NSString *URLString = [[jsonDict objectForKey:@"DATA"] objectForKey:SAFARIOPENURLIDENTIFIER];
    
    [self safariOpenURL:URLString];
  }
  
  else if([typeString isEqualToString:FINDEROPENFILEREQUEST])
  {
    NSLog(@"SOCKET: <FinderOpenFileREQUEST> received.");
    
    NSString *fileString = [[jsonDict objectForKey:@"DATA"] objectForKey:FINDEROPENFILEIDENTIFIER];
 
    [self finderOpenFile:fileString];
  }
	
  else if([typeString isEqualToString:FINDERUPLOADREQUEST])
  {
	  NSLog(@"SOCKET: <FinderUploadREQUEST> received.");
	  
	  NSString *fileString = [[jsonDict objectForKey:@"DATA"] objectForKey:FINDERUPLOADIDENTIFIER];
	  
	  [self handleFinderUploadRequest:fileString];
	  
	  NSLog(@"SOCKET: <FinderUploadRESPONSE> sent.");
  }
  
  else if([typeString isEqualToString:MAILOPENEMAILREQUEST])
  {
    NSLog(@"< Socket Data <<RECEIVED: MailOpenEmailRequest>> >");
    
    NSString *mailString = [[jsonDict objectForKey:@"DATA"] objectForKey:MAILOPENEMAILIDENTIFIER];

    [self mailOpenEmail:mailString];
  }
  
  else if([typeString isEqualToString:SKIMOPENPDFREQUEST])
  {
    NSLog(@"< Socket Data <<RECEIVED: SkimOpenPDFRequest>> >");
    
    NSString *skimString = [[jsonDict objectForKey:@"DATA"] objectForKey:SKIMOPENPDFIDENTIFIER];
    
    [self skimOpenPDF:skimString];
  }
  
  else if([typeString isEqualToString:RMDJAVADIDFINISHLAUNCHINGNOTIFICATION])
  {
    NSLog(@"< Socket Data <<RECEIVED: RMDJavaDidFisishLaunchingNOTIFICATION>> >");
    
    NSNumber *javaPid   = [[jsonDict objectForKey:@"DATA"] valueForKey:@"PID"];
    RMDJavaPid = [javaPid intValue];
    
    NSLog(@"Java Pid: %d", RMDJavaPid); 
    
    [progressIndicator stopAnimation:nil];
    [progressIndicator setHidden:YES];
    [statusField setStringValue:@"Please, activate pen."];
  }
  
  else if([typeString isEqualToString:RMDJAVADIDACTIVATEPENNOTIFICATION])
  {
    NSLog(@"< Socket Data <<RECEIVED: RMDJavaDidActivatePenNOTIFICATION>> >");
    
    [statusField setStringValue:@"Pen activated."];
    [mainWindow performMiniaturize:nil];
  }
  
  [sock readDataToData:[AsyncSocket LFData] withTimeout:-1 tag:0];
}

//---------------------------------------------------------------------------------
#pragma mark General Helper Methods
//---------------------------------------------------------------------------------

/*
//opens a document (URL, FILE or MAIL so far) identified by the identification name 
- (void)openDocumentOfType:(NSString *)docType withIdentificationName:(NSString *)docIdentificationName
{
  //case: FILE
  if([docType isEqualToString:@"FILE"])
  {
    //open file in Finder
    [self finderOpenFile:docIdentificationName];
  }
  //case: MAIL
  else if([docType isEqualToString:@"MAIL"])
  {
    //open mail in Mail
    [self mailOpenEmail:docIdentificationName];
  }
  //case: URL
  else if([docType isEqualToString:@"URL"])
  {
    //open document in Safari
    [self safariOpenURL:docIdentificationName];
  }
}
*/

/**
 *  returns the name of the frontmost application
 *  @return the name of the frontmost apllication
 */
- (NSString*)frontmostApplicationIdentifier
{  
  if([safari frontmost])
  {
    return safariIdentifier;
  }
  else if([finder frontmost])
  {
    return finderIdentifier;
  }
  else if([mail frontmost])
  {
    return mailIdentifier;
  }
  else if([skim frontmost])
  {
    return skimIdentifier;
  }
  @try
  {
    return [chrome active];
  }
  @catch (NSException * e)
  {
    
  }
  
  return unknownIdentifier;
}

/**
 *  returns the identifier name of the current selected item (such as file URL, web URL, mail id, ...) for a given application
 *  @param appIdentifier the application identifier
 *  @return the object identifier
 */
- (NSString *)objectIdentifierForApplicationIdentifier:(NSString *)appIdentifier;
{
  if([appIdentifier isEqualToString:safariIdentifier])
  {
    if([safari isRunning])
    {
      //retrieve all Safari windows that are opened
      SBElementArray* sfWindows = [[safari windows] retain];
      
      for(SafariWindow* sfWindow in sfWindows)
      {
        //retrieve all opened Safari documents
        SafariDocument* sfDocument = [sfWindow document];
        
        NSDate *date = [NSDate date];
        [fetchedURLs setValue:date forKey:[sfDocument URL]];
        
        //grab the expiration date from the website and store it to the URLDates dictionary
        NSDate *URLdate = [self webDateForURL:[sfDocument URL]];
        NSLog(@"Expiration Date of website: %@", URLdate);
        NSBeep();
        
        //return grabbed URL
        return [sfDocument URL];
      }
    }
  }
  else if([appIdentifier isEqualToString:finderIdentifier])
  {    
    //at the moment only the last selected file is returned
    //TODO: multi selection for finder files
    if([finder isRunning])
    {
      for(FinderItem* finderItem in [[finder selection] get])
      {
        return [finderItem URL];
      }
    }
  }
  else if([appIdentifier isEqualToString:mailIdentifier])
  {
    //at the moment only the last selected mail is returned
    //TODO: multi selection for mail emails
    if([mail isRunning])
    {
      for(MailMessage* mailMessage in [mail selection])
      {
        return [mailMessage messageId];
      }
    }
  }
  else if([appIdentifier isEqualToString:skimIdentifier])
  {
    if([skim isRunning])
    {
      //retrieve all Skim windows that are opened
      SBElementArray* skWindows = [[skim windows] retain];
      
      for(SkimWindow* skWindow in skWindows)
      {
        //retrieve all opened Skim documents
        SkimDocument *skDocument = [skWindow document];
        //NSLog(@"Skim PDF located at: %@", [skDocument file]);
        
        SkimPage *skPage = [[skDocument currentPage] get];
        //NSLog(@"Skim PDF current page: %@ with index: %d", [skPage description], [skPage index]);
        
        NSString *skimPDFObjectIdentifier = [NSString stringWithFormat:@"%@:%d", [skDocument file], [skPage index]];
        
        return skimPDFObjectIdentifier;
      }
    }
  }
  else if([appIdentifier isEqualToString:chromeIdentifier])
  {
    return chromeTabURL;
  }
  
  //if there is no match, return 'Unknown'
  return unknownObjectIdentifier;
}

//---------------------------------------------------------------------------------
#pragma mark Request Handling
//---------------------------------------------------------------------------------
/**
 *  respond to a 'FinderFileRequest' with JSON
 */
- (void)handleFinderFileRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];

  [jsonData setObject:[self objectIdentifierForApplicationIdentifier:finderIdentifier] forKey:FINDERFILEIDENTIFIER];

  [jsonObject setValue:FINDERFILERESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];

  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}

/**
 * respond to a 'FinderUploadRequest' with JSON
 */
- (void)handleFinderUploadRequest:(NSString*)filePath
{
	NSData *data = [NSData dataWithContentsOfFile:filePath];

	[self sendData:data];
}

/**
 *  respond to a 'MailEmailRequest' with JSON
 */
- (void)handleMailEmailRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];

  [jsonData setObject:[self objectIdentifierForApplicationIdentifier:mailIdentifier] forKey:MAILEMAILIDENTIFIER];

  [jsonObject setValue:MAILEMAILRESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];

  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}

/**
 *  respond to a 'SafariURLRequest' with JSON
 */
- (void)handleSafariURLRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];

  [jsonData setObject:[self objectIdentifierForApplicationIdentifier:safariIdentifier] forKey:SAFARIURLIDENTIFIER];

  [jsonObject setValue:SAFARIURLRESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];

  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}
 
/**
 *  respond to a 'SkimPDFRequest' with JSON
 */
- (void)handleSkimPDFRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];
  
  [jsonData setObject:[self objectIdentifierForApplicationIdentifier:skimIdentifier] forKey:SKIMPDFIDENTIFIER];
  
  [jsonObject setValue:SKIMPDFRESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];
  
  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}

/**
 *  respond to a 'ChromeURLRequest' with JSON
 */
- (void)handleChromeURLRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];
  
  //get chromeTabURL and pack it into the JSON package
  [jsonData setObject:chromeTabURL forKey:CHROMEURLIDENTIFIER];
  
  [jsonObject setValue:CHROMEURLRESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];
  
  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}

/**
 *  respond to a 'ChromeURLPush' with JSON
 */
- (void)handleChromeURLPush
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];
  
  [jsonData setObject:[self objectIdentifierForApplicationIdentifier:safariIdentifier] forKey:CHROMEURLIDENTIFIER];
  
  [jsonObject setValue:CHROMEURLRESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];
  
  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}

/**
 *  respond to a 'AppFocusRequest' with JSON
 */
- (void)handleAppFocusRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];
  
  [jsonData setObject:[self frontmostApplicationIdentifier] forKey:APPLICATIONFOCUSIDENTIFIER];
  
  [jsonObject setValue:APPLICATIONFOCUSRESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];
  
  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  
  [self sendMessage:jsonString];  
}

/**
 *  respond to a 'ObjectPathRequest' with JSON
 */
- (void)handleObjectPathRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];

  [jsonData setObject:[self objectIdentifierForApplicationIdentifier:[self frontmostApplicationIdentifier]] forKey:OBJECTPATHIDENTIFIER];

  [jsonObject setValue:OBJECTPATHRESPONSE forKey:@"TYPE"];
  [jsonObject setObject:jsonData forKey:@"DATA"];

  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}


/**
 * @depricated
 *  tells the java core to shut down
 */
- (void)sendRMDJavaShutdownRequest
{
  NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
  
  [jsonObject setValue:RMDJAVASHUTDOWNREQUEST forKey:@"TYPE"];
  
  //pack data into JSON package
  NSString* jsonString = [jsonObject JSONRepresentation];
  //transmit JSON package via socket
  [self sendMessage:jsonString];
}

//---------------------------------------------------------------------------------
#pragma mark Dialog Windows
//---------------------------------------------------------------------------------
/**
 *  shows the preferences dialog window
 *  @param sender a sender
 *  @return interface builder action
 */
- (IBAction)showPreferencesDialog:(id)sender
{
  @try
  {
    if(mainWindow)
    {
      [NSApp beginSheet:preferencesWindow 
         modalForWindow:mainWindow 
          modalDelegate:nil 
         didEndSelector:NULL 
            contextInfo:NULL];
    }
    
  }
  @catch (NSException * e)
  {
  }
}

/**
 *  hides the preferences dialog window
 *  @param sender a sender
 *  @return interface builder action
 */
- (IBAction)hidePreferencesDialog:(id)sender
{
  @try
  {
    [plistDict setValue:[socketHostField stringValue] forKey:@"Socket Host"];
    [plistDict setValue:[NSNumber numberWithInt:[socketPortField intValue]] forKey:@"Socket Port"];
    [plistDict setValue:[NSNumber numberWithBool:[externalLaunchCheckbox state]] forKey:@"External Java Launch"];
    [plistDict setValue:[NSNumber numberWithBool:[reloadWebsitesCheckbox state]] forKey:@"Reload Websites"];
    [plistDict setValue:[gmailUsernameField stringValue] forKey:@"GMail Username"];
    
    //write changes to 'Info.plist'
    NSString *path = [[NSBundle mainBundle] bundlePath];
    NSString *finalPath = [path stringByAppendingPathComponent:@"Contents/Info.plist"];
    [plistDict writeToFile:finalPath atomically: YES];
    [path release];
  }
  @catch (NSException * e)
  {
  }
  
  //close the preferences window
  @try
  {
    [NSApp endSheet:preferencesWindow];
    [preferencesWindow orderOut:mainWindow];
  }
  @catch (NSException * e)
  {
  }
}

/**
 *  delegate method for the mainWindow (we have to ensure that no preferences/about window
 *  will be shown when the window is minimized (this would cause an error otherwise)
 *  @param notification a notification message
 */
- (void)windowWillClose:(NSNotification *)notification
{
  mainWindow = nil;
  [mainWindow release];
}

/**
 *  shows the about dialog window
 *  @param sender a sender
 *  @return interface builder action
 */
- (IBAction)showAboutDialog:(id)sender
{
  @try
  {
    if(mainWindow)
    {
      [NSApp beginSheet:aboutWindow 
         modalForWindow:mainWindow 
          modalDelegate:nil 
         didEndSelector:NULL 
            contextInfo:NULL];
    }
    
  }
  @catch (NSException * e)
  {
  }
}

/**
 *  hides the about dialog window
 *  @param sender a sender
 *  @return interface builder action
 */
- (IBAction)hideAboutDialog:(id)sender
{
  @try
  {
    [NSApp endSheet:aboutWindow];
    [aboutWindow orderOut:mainWindow];
  }
  @catch (NSException * e)
  {
  }
}

//---------------------------------------------------------------------------------
#pragma mark Helper Methods
//---------------------------------------------------------------------------------

/**
 *  launches the RMD Java client (located in ./Java/RefreshMyDesk.zip)
 */
- (void)launchRDMJava
{
  NSString *path = [[NSBundle mainBundle] resourcePath];
  rmdJavaTask = [[NSTask alloc] init];
  [rmdJavaTask setLaunchPath:@"/bin/sh"];
  [rmdJavaTask setArguments:[NSArray arrayWithObjects:[[NSBundle mainBundle]
            pathForResource:@"start" ofType:@"sh"], path,  nil]];
  [rmdJavaTask launch];
}

/**
 *  launches an application
 *  @param bundleIdentifier bundle identifier of the application
 *  @return YES, if launch was successful
 */
- (BOOL)launchApplicationWithBundleIdentifier:(NSString *)bundleIdentifier
{
  NSURL *appURL = [[NSWorkspace sharedWorkspace] URLForApplicationWithBundleIdentifier:bundleIdentifier];
  NSString *appURLString = [appURL absoluteString];
  NSString *cleanAppURLString = [appURLString stringByReplacingOccurrencesOfString:@"file://localhost" withString:@""];
  return [[NSWorkspace sharedWorkspace] launchApplication:cleanAppURLString];
}

/**
 *  print a log message on the standard I/O
 *  @param msg a log message
 */
- (void)logInfo:(NSString *)msg
{
	NSString *paragraph = [NSString stringWithFormat:@"%@\n", msg];

  NSLog(@"SOCKET: %@", paragraph);
}

/**
 *  brings an application (i.e., its windows) to front
 *  @param bundleIdentifier the bundle identifier of the application
 */
- (void)applicationToFrontForBundleIdentifier:(NSString *)bundleIdentifier
{
  NSArray* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:bundleIdentifier];
  [(NSRunningApplication*)[apps objectAtIndex:0] activateWithOptions:NSApplicationActivateIgnoringOtherApps];
}

/**
 *  if a URL has been loaded in a tab before, this methods prevents reloading it
 *  @param URLIdentificationName a URL
 */
- (BOOL)didLoadURL:(NSString *)URLIdentificationName
{
  //unsafe! check first, if Safari has been started!
  
  for(SafariWindow *sfWindow in [safari windows])
  {
    for(SafariTab *sfTab in [sfWindow tabs])
    {
      //the URL has been loaded in a tab before
      if([[sfTab URL] isEqualToString:URLIdentificationName])
      {
        [sfWindow setCurrentTab:sfTab];
        return YES;
      }
    }
  }
  
  //the URL has not been loaded in a tab so far
  return NO;
}

/**
 *  launches the GMail website in Safari with a given username (read from .plist)
 *  @param sender a sender
 *  @return interface builder action 
 */
- (IBAction)loginToGoogle:(id)sender
{
  NSString *username = [plistDict valueForKey:@"GMail Username"];
  
  if(![username isEqualToString:@""])
  {
    [self safariOpenURL:[NSString stringWithFormat:@"https://www.google.com/accounts/ServiceLoginAuth?continue=http://mail.google.com/gmail&service=mail&Email=%@&Passwd=null&null=Sign+in", username]];
  }
  else
  {
    [self safariOpenURL:@"https://www.google.com/accounts/ServiceLoginAuth"];
  }
}

/**
 *  determines if a website is out of date and needs to be reloeded
 *  @param URLIdentificationName a URL
 *  @return YES, if the website needs to be reloaded
 */
- (BOOL)needRefreshForURL:(NSString *)URLIdentificationName
{    
    NSDate *currentDate = [NSDate date];
    NSDate *fetchDate = nil;
    
    @try
    {
      //get the expiration date from the website
      fetchDate = [URLDates valueForKey:URLIdentificationName];
      NSLog(@"Expiration date of website: %@", fetchDate);
      
      //if the link is already on paper
      if([URLDates objectForKey:URLIdentificationName] == nil)
      {
        [self webDateForURL:URLIdentificationName];
      }
    }
    @catch (NSException * e)
    {
      NSLog(@"NO FETCH DATE");
    }
    
    //comparison
    NSDate *laterDate = [fetchDate laterDate:currentDate];
    //NSLog(@"Current date: %@", currentDate);
    //NSLog(@"Latest  date: %@", laterDate);
    
    if([laterDate isEqualToDate:currentDate])
    {
      //the current date is newer than the expiration date, hence the website needs to be reloaded
      NSLog(@"WEBSITE EXPIRED!");
      return YES;
    }
  
  return NO;
}

/**
 *  grabs the expiration date of a website (if supported)
 *  @param URLIdentificationName a URL
 *  @return the expiration date
 */
- (NSDate *)webDateForURL:(NSString *)URLIdentificationName
{
  //Send HTTP Request
  NSURL *oRequestUrl = [NSURL URLWithString:URLIdentificationName];
  NSMutableURLRequest *oRequest = [[[NSMutableURLRequest alloc] init] autorelease];
  [oRequest setValue:@"text/plain" forHTTPHeaderField:@"Content-Type"];
  [oRequest setHTTPMethod:@"POST"];
  [oRequest setURL:oRequestUrl];
  NSMutableData *oHttpBody = [NSMutableData data];
  [oHttpBody appendData:[@"This is Http Request body" dataUsingEncoding:NSUTF8StringEncoding]];
  [oRequest setValue:[[NSNumber numberWithInt:[oHttpBody length]] stringValue] forHTTPHeaderField:@"Content-Length"];
  NSError *oError = [[NSError alloc] init];
  NSHTTPURLResponse *oResponseCode = nil;
  
  NSData *oResponseData = [NSURLConnection sendSynchronousRequest:oRequest returningResponse:&oResponseCode error:&oError];
  
  //if ([oResponseCode statusCode] >= 200)
  //{
   // NSLog(@"Status code is greater than 200");
  //}
  
  NSDictionary *headerDict = [oResponseCode allHeaderFields];
  
  @try
  {
    //parse the 'last modified' value from the HTTP header
    NSString *lastModified = [headerDict valueForKey:@"Last-Modified"];
    if(lastModified == nil)
    {
      @try
      {
        lastModified = [headerDict valueForKey:@"Expires"];
      }
      @catch (NSException * e)
      {
        
      }
    }
    
    NSDateFormatter *inputFormatter = [[NSDateFormatter alloc] init];
    [inputFormatter setDateFormat:@"EEE, dd MMM yyyy HH:mm:ss 'GMT'"];
    NSDate *formatterDate = [inputFormatter dateFromString:lastModified];
    
    if(lastModified != nil)
    {
      //add the exoiration date to the URLDates dictionary
      [URLDates setValue:formatterDate forKey:URLIdentificationName];
      NSLog(@"Added Date: %@", formatterDate);
    }
    
    return formatterDate;
  }
  @catch (NSException *e)
  {
    
  }
  return nil;
}
//THIS CONFIGURATION WORKS!
/*
 - (IBAction)testSocketWrite:(id)sender
 {
 NSMutableDictionary* jsonObject = [NSMutableDictionary dictionary];
 NSMutableDictionary* jsonData = [NSMutableDictionary dictionary];
 
 [jsonData setObject:[self frontmostApplicationIdentifier] forKey:APPLICATIONFOCUSIDENTIFIER];
 
 [jsonObject setValue:APPLICATIONFOCUSRESPONSE forKey:@"TYPE"];
 [jsonObject setObject:jsonData forKey:@"DATA"];
 
 //pack data into JSON package
 NSString* jsonString = [jsonObject JSONRepresentation];
 
 NSData *data = [[jsonString stringByAppendingString:@"\r\n"] dataUsingEncoding:NSUTF8StringEncoding];
 for(AsyncSocket *sock in connectedSockets)
 {
 [sock writeData:data withTimeout:-1 tag:1];
 }
 }
 */

/*
- (NSArray*) authenticateWithGoogleClientLogin:(NSString *)userName withPassword:(NSString*)password {
	/*	Google clientLogin API:
   Content-type: application/x-www-form-urlencoded
   Email=userName
   Passwd=password
   accountType=HOSTED_OR_GOOGLE
   service=xapi
   source = @"myComp-myApp-1.0"
   */
/*
  NSString *pluginVersion = @"Bla";
	//define our return objects
	BOOL authOK;
	NSString *authMessage = [[NSString alloc] init];
	NSArray *returnArray = nil;
	//begin NSURLConnection prep:
	NSMutableURLRequest *httpReq = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:GOOGLE_CLIENT_AUTH_URL] ];
	[httpReq setTimeoutInterval:30.0];
	//[httpReq setCachePolicy:NSURLRequestReloadIgnoringCacheData];
	[httpReq setHTTPMethod:@"POST"];
	//set headers
	[httpReq addValue:@"Content-Type" forHTTPHeaderField:@"application/x-www-form-urlencoded"];
	//set post body
	NSString *requestBody = [[NSString alloc] 
                           initWithFormat:@"Email=%@&Passwd=%@&service=xapi&accountType=HOSTED_OR_GOOGLE&source=%@",
                           userName, password, [NSString stringWithFormat:@"%@%d", pluginVersion]];
  
	[httpReq setHTTPBody:[requestBody dataUsingEncoding:NSASCIIStringEncoding]];
  
	NSHTTPURLResponse *response = nil;
	NSError *error = nil;
	NSData *data = nil;
	NSString *responseStr = nil;
	NSArray *responseLines = nil;
	NSString *errorString;
	//NSDictionary *dict;
	int responseStatus = 0;
	//this should be quick, and to keep same workflow, we'll do this sync.
	//this should also get us by without messing with threads and run loops on Tiger.
	data = [NSURLConnection sendSynchronousRequest:httpReq returningResponse:&response error:&error];
  
	if ([data length] > 0) {
		responseStr = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
		NSLog(@"Response From Google: %@", responseStr);
		responseStatus = [response statusCode];
		//dict = [[NSDictionary alloc] initWithDictionary:[response allHeaderFields]];
		//if we got 200 authentication was successful
		if (responseStatus == 200 ) {
			authOK = TRUE;
			authMessage = @"Authentication Successful...";
		}
		//403 = authentication failed.
		else if (responseStatus == 403) {
			authOK = FALSE;
			//get Error code.
			responseLines  = [responseStr componentsSeparatedByString:@"\n"];
			//find the line with the error string:
			int i;
			for (i =0; i < [responseLines count]; i++ ) {
				if ([[responseLines objectAtIndex:i] rangeOfString:@"Error="].length != 0) {
					errorString = [responseLines objectAtIndex:i] ;
				}
			}
			
			errorString = [errorString stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
			/*
       Official Google clientLogin Error Codes:
       Error Code	Description
       BadAuthentication	The login request used a username or password that is not recognized.
       NotVerified	The account email address has not been verified. The user will need to access their Google account directly to resolve the issue before logging in using a non-Google application.
       TermsNotAgreed	The user has not agreed to terms. The user will need to access their Google account directly to resolve the issue before logging in using a non-Google application.
       CaptchaRequired	A CAPTCHA is required. (A response with this error code will also contain an image URL and a CAPTCHA token.)
       Unknown	The error is unknown or unspecified; the request contained invalid input or was malformed.
       AccountDeleted	The user account has been deleted.
       AccountDisabled	The user account has been disabled.
       ServiceDisabled	The user's access to the specified service has been disabled. (The user account may still be valid.)
       ServiceUnavailable	The service is not available; try again later.
       */
/*
			if ([errorString  rangeOfString:@"BadAuthentication" ].length != 0) {
				authMessage = @"Please Check your Username and Password and try again.";
			}else if ([errorString  rangeOfString:@"NotVerified"].length != 0) {
				authMessage = @"This account has not been verified. You will need to access your Google account directly to resolve this";
			}else if ([errorString  rangeOfString:@"TermsNotAgreed" ].length != 0) {
				authMessage = @"You have not agreed to Google terms of use. You will need to access your Google account directly to resolve this";
			}else if ([errorString  rangeOfString:@"CaptchaRequired" ].length != 0) {
				authMessage = @"Google is requiring a CAPTCHA response to continue. Please complete the CAPTCHA challenge in your browser, and try authenticating again";
				//NSString *captchaURL = [responseStr substringFromIndex: [responseStr rangeOfString:@"CaptchaURL="].length]; 
				//either open the standard URL in a browser, or show a custom sheet with the image and send it back...
				//parse URL to append to GOOGLE_CAPTCHA_URL_PREFIX
				//but for now... just launch the standard URL.
				[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:GOOGLE_CAPTCHA_STANDARD_UNLOCK_URL]];			
			}else if ([errorString  rangeOfString:@"Unknown" ].length != 0) {
				authMessage = @"An Unknow error has occurred; the request contained invalid input or was malformed.";
			}else if ([errorString  rangeOfString:@"AccountDeleted" ].length != 0) {
				authMessage = @"This user account previously has been deleted.";
			}else if ([errorString  rangeOfString:@"AccountDisabled" ].length != 0) {
				authMessage = @"This user account has been disabled.";
			}else if ([errorString  rangeOfString:@"ServiceDisabled" ].length != 0) {
				authMessage = @"Your access to the specified service has been disabled. Please try again later.";
			}else if ([errorString  rangeOfString:@"ServiceUnavailable" ].length != 0) {
				authMessage = @"The service is not available; please try again later.";
			}
      
		}//end 403 if
    
	}
	//check most likely: no internet connection error:
	if (error != nil) {
		authOK = FALSE;
		if ( [error domain]  == NSURLErrorDomain) {
			authMessage = @"Could not reach Google.com. Please check your Internet Connection";
		}else {
			//other error
			authMessage = [authMessage stringByAppendingFormat:@"Internal Error. Please contact notoptimal.net for further assistance. Error: %@", [error localizedDescription] ];
		}
	}
	//NSLog (@"err localized description %@", [error localizedDescription]) ;
	//NSLog (@"err localized failure reasons %@", [error localizedFailureReason]) ;
	//NSLog(@"err code  %d", [error code]) ;
	//NSLog (@"err domain %@", [error domain]) ;
  
	//build return array [0] = authOK [1] = authMessage
	returnArray =  [NSArray arrayWithObjects: [NSNumber numberWithBool:authOK], authMessage, responseLines, nil ];
	return returnArray;
}
*/

- (NSRunningApplication *)chromeInstance
{
  //check for a running instance of Google Chrome
  NSArray* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:CHROMEBUNDLEIDENTIFIER];
  
  NSLog(@"Running apps: %@", [apps description]);
  
  NSRunningApplication *chromeInstance = nil;
  
  @try
  {
    chromeInstance = (NSRunningApplication*)[apps objectAtIndex:0];
  }
  @catch (NSException * e)
  {
    NSLog(@"Chrome is not running.");
  }
  
  if(chromeInstance == nil)
  {
    //start Chrome and get its instance
    [self launchApplicationWithBundleIdentifier:CHROMEBUNDLEIDENTIFIER];
    NSArray* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:CHROMEBUNDLEIDENTIFIER];
    @try
    {
      return (NSRunningApplication*)[apps objectAtIndex:0];
    }
    @catch (NSException * e)
    {
      NSLog(@"Chrome is not running.");
    }
  }
  
  return chromeInstance;
}

@end
