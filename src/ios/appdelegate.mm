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
#import "platformutils.h"

@interface QIOSApplicationDelegate : UIResponder <UIApplicationDelegate, GIDSignInDelegate>
@end

@interface QIOSApplicationDelegate(AppDelegate)
@end

@implementation QIOSApplicationDelegate (AppDelegate)

  static NSString * const googleClientID = @"404440643421-lbasejr6a7qasqist87pr62vh842jlgv.apps.googleusercontent.com";
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

