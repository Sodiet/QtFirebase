#import "UIKit/UIKit.h"
#include <QtCore>


//#import "FBSDKCoreKit"


#import "Firebase/Auth/Facebook/FBSDKCoreKit.framework/Headers/FBSDKCoreKit.h"
#import "Firebase/Auth/Facebook/FBSDKLoginKit.framework/Headers/FBSDKLoginManager.h"
#import "Firebase/Auth/Facebook/FBSDKLoginKit.framework/Headers/FBSDKLoginManagerLoginResult.h"

#import "Firebase/Invites/GoogleSignIn.framework/Headers/GoogleSignIn.h"


#import "Firebase/Firebase.h"

#import "AppDelegate.h"
#import "qtfirebaseauth.h"
#import "viewdelegate.h"

@interface QIOSApplicationDelegate : UIResponder <UIApplicationDelegate, GIDSignInDelegate>
@end

@interface QIOSApplicationDelegate(AppDelegate)
@end

@implementation QIOSApplicationDelegate (AppDelegate)

  // Firebase project ID e.g. : Your-App-ID

  // Google sign in essentials
    static NSString * const googleClientID = @"404440643421-4jsllvkgfahkssdij14714qapisaubt9.apps.googleusercontent.com";

  // Facebook signin essentails
  // * On the Facebook for Developers site, get the App ID and an App Secret for your app.
  // * Enable Facebook Login:
  // * In the Firebase console, open the Auth section.
  // * On the Sign in method tab, enable the Facebook sign-in method and specify the App ID and App Secret you got from Facebook.
  // * Then, make sure your OAuth redirect URI (e.g. https://Your-App-ID.firebaseapp.com/__/auth/handler) is listed as one of your OAuth redirect URIs in your Facebook app's settings page on the Facebook for Developers site in the Product Settings > Facebook Login config.
  // * Add folowing lines in Info.plist
  // <key>LSApplicationQueriesSchemes</key>
  // <array>
  //    <string>fbauth2</string>
  // </array>
  // * Add URL schemes in Info.plist with your facebook app id
  // <key>CFBundleURLTypes</key>
  // <array>
  //   <dict>
  //   <key>CFBundleURLSchemes</key>
  //   <array>
  //     <string><your fb id here eg. fbxxxxxx></string>
  //   </array>
  //   </dict>
  // </array>
  // * your AppID and App DisplayName
  static NSString * const facebookAppID = @"429883020919797";
  static NSString * const facebookAppDisplayName = @"TestApp";


- (BOOL)application:(UIApplication *)application
  didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    Q_UNUSED(application);
    Q_UNUSED(launchOptions);

      [FIRApp configure];
      [GIDSignIn sharedInstance].clientID = googleClientID;

      // Facebook configure
      [FBSDKSettings setAppID:facebookAppID];
      [FBSDKSettings setDisplayName:facebookAppDisplayName];

      NSLog(@"FireApp configured successfully");

      [[FBSDKApplicationDelegate sharedInstance] application:application
  didFinishLaunchingWithOptions:launchOptions];
    return YES;
}

// [START google facebook twitter openurl method]
- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation {

 BOOL googleHandled = [[GIDSignIn sharedInstance] handleURL:url
                      sourceApplication:sourceApplication
                      annotation:annotation];

BOOL facebookHandled = [[FBSDKApplicationDelegate sharedInstance] application:application
    openURL:url
    sourceApplication:sourceApplication
    annotation:annotation
  ];

  return googleHandled || facebookHandled;
}

@end

