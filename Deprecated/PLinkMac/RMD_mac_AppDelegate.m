//
//  RMD_mac_AppDelegate.m
//  RMD(mac)
//
//  Created by Christian Leonhard Corsten on 15.09.10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

/**
 @brief PLink:mac application delegate
 
 This class is the application delegate class file for PLink:mac.
 */


#import "RMD_mac_AppDelegate.h"

@implementation RMD_mac_AppDelegate

@synthesize window;

/**
 *  determines that the application should terminate ather the last application window has been closed
 *  @param theApplication an application
 *  @return NO
 */
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
  return NO;
}

/**
 *  kills the Java core in case it is running
 *  @param sender a sender
 *  @return an application terminate reply
 */
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
  //close the socket connection first and disconnect all clients
  [appController startStop];
  
  @try
  { 
    if([appController RMDJavaPid] != 0)
    {
      killpg(getpgid([appController RMDJavaPid]), SIGTERM);
    }
  }
  @catch (NSException * e)
  {
    NSLog(@"No pid to kill.");
  }

  return NSTerminateNow;
}

@end
