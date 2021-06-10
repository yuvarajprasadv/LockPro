//
//  JJLockID.h
//  JJLock
//
//  Created by Created by Praveen on 10/1/19(MM/DD/YY)
//  Modified by Yuvaraj on 05/02/2019
//



#ifndef __JJLock_H__
#define __JJLock_H__

#define PLUGIN_NAME         "Lock Pro"
#define PLUGIN_NAME_STRING  "Lock Pro"
#define PLUGIN_VERSION      "3.2 QC"
#define PlUGIN_BUILD_VERSION "1.0.3"

#define kBetaVersion true
#define kAuthenticate true
#define kTextExpireDate   1623542399   //  Saturday, June 12, 2021 11:59:59 PM

// Company name
#define COMPANY_NAME                            "SGK"

// About Plug-ins menu
#define ABOUT_MENU_PLUGINS_GROUP_NAME           "SGK Plguins"
#define ABOUT_MENU_PLUGINS_GROUP_NAME_STRING    "About SGK Plug-ins"

#define PLUGIN_ABOUT_MESSAGE                    "Tool Name: " PLUGIN_NAME"\nBuild Number: " PlUGIN_BUILD_VERSION"\nDeveloped by: Application Engineering Team\nContact: Typhoon.Support@sgkinc.com\nRelease: " PLUGIN_VERSION"\nCopyright 2021 SGK Inc., All rights reserved."


// Error Messages
#define errAuthentication "Authentication Failed!\nUnable to load " PLUGIN_NAME" Plug-in.\nYou are not an authorized SGK User.\nContact Typhoon.Support@sgkinc.com\n\nCopyright 2021 SGK Inc., All rights reserved."
#define errUATExpired "UAT has been expired.\nPlease contact:\nAsiaInnovationsTeam@sgkinc.com"

#define characterCodeEndOfText      3
#define characterCodeTab            9
#define characterCodeLF             10
#define characterCodeCR             13
#define characterCodeSpace          32
#define numLinkedTextFrame 51

#endif // End JJLockID.h
