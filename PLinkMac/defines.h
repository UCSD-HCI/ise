/*
 *  defines.h
 *  RMD(mac)
 *
 *  Created by Christian Leonhard Corsten on 15.09.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

//---------------------------------------------------------
//Protocol Defines
//---------------------------------------------------------

/**
 *  Protocol defines for JSON exchange with the Java core
 */
#define CREATELINKREQUESTTYPE  @"CreateLinkRequest"
#define OPENLINKREQUESTTYPE    @"OpenLinkRequest"
#define CREATELINKRESPONSETYPE @"CreateLinkResponse"
#define OPENLINKRESPONSETYPE   @"OpenLinkResponse"

#define FINDERFILEREQUEST    @"FinderFileRequest"
#define FINDERFILERESPONSE   @"FinderFileResponse"
#define FINDERFILEIDENTIFIER @"FinderFileIdentifier"

#define FINDEROPENFILEREQUEST    @"FinderOpenFileRequest"
#define FINDEROPENFILERESPONSE   @"FinderOpenFileResponse"
#define FINDEROPENFILEIDENTIFIER @"FinderOpenFileIdentifier"

#define SAFARIURLREQUEST    @"SafariURLRequest"
#define SAFARIURLRESPONSE   @"SafariURLResponse"
#define SAFARIURLIDENTIFIER @"SafariURLIdentifier"

#define SAFARIOPENURLREQUEST    @"SafariOpenURLRequest"
#define SAFARIOPENURLRESPONSE   @"SafariOpenURLResponse"
#define SAFARIOPENURLIDENTIFIER @"SafariOpenURLIdentifier"

#define MAILEMAILREQUEST    @"MailEmailRequest"
#define MAILEMAILRESPONSE   @"MailEmailResponse"
#define MAILEMAILIDENTIFIER @"MailEmailIdentifier"

#define MAILOPENEMAILREQUEST    @"MailOpenEmailRequest"
#define MAILOPENEMAILRESPONSE   @"MailOpenEmailResponse"
#define MAILOPENEMAILIDENTIFIER @"MailOpenEmailIdentifier"

#define SKIMPDFREQUEST    @"SkimPDFRequest"
#define SKIMPDFRESPONSE   @"SkimPDFResponse"
#define SKIMPDFIDENTIFIER @"SkimPDFIdentifier"

#define SKIMOPENPDFREQUEST    @"SkimOpenPDFRequest"
#define SKIMOPENPDFRESPONSE   @"SkimOpenPDFResponse"
#define SKIMOPENPDFIDENTIFIER @"SkimOpenPDFIdentifier"

#define CHROMEURLREQUEST    @"ChromeURLRequest"
#define CHROMEURLRESPONSE   @"ChromeURLResponse"
#define CHROMEURLIDENTIFIER @"ChromeURLIdentifier"

#define CHROMEOPENURLREQUEST    @"ChromeOpenURLRequest"
#define CHROMEOPENURLRESPONSE   @"ChromeOpenURLResponse"
#define CHROMEOPENURLIDENTIFIER @"ChromeOpenURLIdentifier"

#define CHROMEURLPUSH @"ChromeURLPush"
#define CHROMCHROMEOPENURLREQUEST @"ChromeChromeOpenURLRequest"

#define APPLICATIONFOCUSREQUEST    @"AppFocusRequest"
#define APPLICATIONFOCUSRESPONSE   @"AppFocusResponse"
#define APPLICATIONFOCUSIDENTIFIER @"AppFocusIdentifier"

#define OBJECTPATHREQUEST    @"ObjectPathRequest"
#define OBJECTPATHRESPONSE   @"ObjectPathResponse"
#define OBJECTPATHIDENTIFIER @"ObjectPathIdentifier"

#define RMDJAVADIDFINISHLAUNCHINGNOTIFICATION @"RMDJavaDidFinishLaunchingNotification"
#define RMDJAVADIDACTIVATEPENNOTIFICATION     @"RMDJavaDidActivatePenNotification"
#define RMDJAVASHUTDOWNREQUEST                @"RMDJavaShutdownRequest"

//---------------------------------------------------------
//Socket Defines
//---------------------------------------------------------

/**
 *  Socket parameter defines
 */
#define WELCOME_MSG  0
#define ECHO_MSG     1
#define WARNING_MSG  2

#define READ_TIMEOUT 15.0
#define READ_TIMEOUT_EXTENSION 10.0

#define FORMAT(format, ...) [NSString stringWithFormat:(format), ##__VA_ARGS__]

//---------------------------------------------------------
//ScriptingBridge Defines
//---------------------------------------------------------

/**
 *  ScriptingBridge parameter defines
 */
#define FINDERBUNDLEIDENTIFIER @"com.apple.finder"
#define MAILBUNDLEIDENTIFIER   @"com.apple.mail"
#define SAFARIBUNDLEDENTIFIER  @"com.apple.Safari"
#define SKIMBUNDLEIDENTIFIER   @"net.sourceforge.skim-app.skim"
#define CHROMEBUNDLEIDENTIFIER @"com.google.Chrome"

#define UNKNOWNBUNDLEIDENTIFIER @"xxx.xxx.unknown"

/**
 *  GMail parameter defines
 */
#define GOOGLE_CLIENT_AUTH_URL @"https://www.google.com/accounts/ClientLogin"
#define GOOGLE_CAPTCHA_STANDARD_UNLOCK_URL @"https://www.google.com/accounts/DisplayUnlockCaptcha"