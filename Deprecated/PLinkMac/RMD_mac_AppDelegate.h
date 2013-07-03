//
//  RMD_mac_AppDelegate.h
//  RMD(mac)
//
//  Created by Christian Leonhard Corsten on 15.09.10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppController.h"

#include <signal.h>
#include <unistd.h>

@interface RMD_mac_AppDelegate : NSObject <NSApplicationDelegate>
{
  NSWindow *window;
  IBOutlet AppController *appController;
}

@property (assign) IBOutlet NSWindow *window;

@end
