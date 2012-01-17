//
//  AppController.h
//  RMD(mac)
//
//  Created by Christian Leonhard Corsten on 15.09.10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

/**
 @mainpage PLink:mac - Bringing PLink to the Mac.
 
 Based on the PLink Java core, PLink:mac enables to use PLink in a Mac environment.
 A bundle of applications is supported to use paper links with a Mac. 
 
 This framework is based on the PLink Java core. It communicated with the core via JSON
 messaging using sockets. Basically, all necessary methods are located in 'AppController.m'.
 All important defines (e.g., for the communication protocol between the Java core and PLink:mac) 
 are located in 'defines.h'.
 
 
 @section Features
 
 PLink:mac supports the following applications
 
 @li Apple Safari
 @li Apple Mail
 @li Apple Finder (i.e., any file support including the desktop)
 @li Skim PDF Viewer
 
 Communication with the applications is done via ScriptingBridge.
 @section Links
 
 @li <a href="http://code.google.com/p/json-framework">JSON Framework for Objective-C</a>.
 @li <a href="http://www.cocoadev.com/index.pl?AsyncSocket">AsyncSocket Framework for Objective-C</a>.
 @li <a href="http://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/ScriptingBridgeConcepts/Introduction/Introduction.html">ScriptingBridge Framework for Objective-C</a>. 
 
 */

#import <Cocoa/Cocoa.h>
#import "ScriptingBridgeHeader.h"
#import "JSON.h"
#import "AsyncSocket.h"
#import "defines.h"

@class AsyncSocket;

//global constants for the socket communication
extern NSString * const kNotification;
extern NSString * const kNotificationMessage;

@interface AppController : NSObject <NSWindowDelegate>
{
  //JSON
  SBJsonParser *parser;
  
  //Supported ScriptingBride application references
  FinderApplication *finder;
  MailApplication   *mail;
  SafariApplication *safari;
  SkimApplication   *skim;
  //Google Chrome does not support ScriptingBridge
  NSRunningApplication *chrome;
  //This string contains the URL of the active Chrome Tab
  NSString             *chromeTabURL;
  
  //Application identifier strings
  NSString *finderIdentifier;
  NSString *mailIdentifier;
  NSString *safariIdentifier;
  NSString *skimIdentifier;
  NSString *chromeIdentifier;
  NSString *unknownIdentifier;
  NSString *unknownObjectIdentifier;
  
  NSTask *rmdJavaTask;
  
  //UI Outlets
  IBOutlet NSProgressIndicator *progressIndicator;
  IBOutlet NSTextField         *statusField;
  IBOutlet NSWindow            *mainWindow;
  
  IBOutlet NSWindow            *preferencesWindow;
  IBOutlet NSTextField         *socketHostField;
  IBOutlet NSTextField         *socketPortField;
  IBOutlet NSTextField         *gmailUsernameField;
  IBOutlet NSButton            *externalLaunchCheckbox;
  IBOutlet NSButton            *reloadWebsitesCheckbox;
  
  IBOutlet NSButton            *gmailButton;
  
  IBOutlet NSWindow            *aboutWindow;
  
  //Server Socket
  AsyncSocket          *listenSocket;
	NSMutableArray       *connectedSockets;
	BOOL                  itIsRunning;
  NSNotificationCenter *notificationCenter;
  NSString             *socketHost;
  int                   socketPort;
  
  int RMDJavaPid;
  NSDictionary *plistDict;
  
  NSMutableDictionary *fetchedURLs;
  NSMutableDictionary *URLDates;
}

@property (assign) int RMDJavaPid;

//socket connection methods
- (void)sendMessage:(NSString*)message;
- (void)startStop;

//retrieve application data
- (NSString *)objectIdentifierForApplicationIdentifier:(NSString *)appIdentifier;
//beta (not implemented yet): returns multiple selected objects
//- (NSArray *)objectIdentifiersForApplicationIdentifier:(NSString *)appIdentifier;
- (NSString *)frontmostApplicationIdentifier;

//application methods
- (void)finderOpenFile:(NSString *)fileIdentificationName;
- (void)mailOpenEmail:(NSString *)mailIdentificationName;
- (void)safariOpenURL:(NSString *)URLIdentificationName;
- (void)skimOpenPDF:(NSString *)PDFIdentificationName;

- (void)handleFinderFileRequest;
- (void)handleMailEmailRequest;
- (void)handleSafariURLRequest;
- (void)handleSkimPDFRequest;
- (void)handleAppFocusRequest;
- (void)handleObjectPathRequest;
- (void)sendRMDJavaShutdownRequest;

- (IBAction)showPreferencesDialog:(id)sender;
- (IBAction)hidePreferencesDialog:(id)sender;
- (IBAction)showAboutDialog:(id)sender;
- (IBAction)hideAboutDialog:(id)sender;

- (void)launchRDMJava;
- (BOOL)launchApplicationWithBundleIdentifier:(NSString *)bundleIdentifier;
- (void)logInfo:(NSString *)msg;
- (void)applicationToFrontForBundleIdentifier:(NSString *)bundleIdentifier;
- (BOOL)didLoadURL:(NSString *)URLIdentificationName;
- (BOOL)needRefreshForURL:(NSString *)URLIdentificationName;
- (NSDate *)webDateForURL:(NSString *)URLIdentificationName;

- (IBAction)loginToGoogle:(id)sender;

- (NSRunningApplication *)chromeInstance;
//- (NSArray*) authenticateWithGoogleClientLogin:(NSString *)userName withPassword:(NSString*)password;

@end
