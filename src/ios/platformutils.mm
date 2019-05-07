#include "platformutils.h"

#import <UIKit/UIKit.h>
#import "../qtfirebasesocialsignin.h"

#import "Firebase/Auth/Facebook/FBSDKCoreKit.framework/Headers/FBSDKAccessToken.h"
#import "Firebase/Auth/Facebook/FBSDKLoginKit.framework/Headers/FBSDKLoginManager.h"
#import "Firebase/Auth/Facebook/FBSDKLoginKit.framework/Headers/FBSDKLoginManagerLoginResult.h"

#import "Firebase/Invites/GoogleSignIn.framework/Headers/GoogleSignIn.h"

PlatformUtils::PlatformUtils()
{

}

void* PlatformUtils::getNativeWindow()
{

    //QWindow *window = QGuiApplication::topLevelAt(QPoint(1,1));
    // Hints from https://forum.qt.io/topic/46297/qt-5-3-2-qml-on-ios-8-cannot-get-uiview-from-qquickwindow-anymore-solved
    //UIView *view = static_cast<UIView *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", window));
    //UIView *view = static_cast<UIView *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", QGuiApplication::focusWindow()));
    UIView *view = (__bridge UIView *)reinterpret_cast<void *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", QGuiApplication::focusWindow()));
    //UIView *view = (__bridge UIView *)reinterpret_cast<void *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", window));

    //UIView* view = (__bridge UIView*)reinterpret_cast<void*>(window->winId());
    //qDebug() << "Getting UIView" << view;

    return view;

    //return QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", QGuiApplication::focusWindow());
}

void PlatformUtils::googleLogin()
{
    GIDSignIn *signin = [GIDSignIn sharedInstance];
    
    [signin setScopes:[NSArray arrayWithObject:@"profile"]];
    
    if (signin.hasAuthInKeychain) {
        
        NSLog(@"user signed in already, signing silently...");
        
        GIDGoogleUser *user = [GIDSignIn sharedInstance].currentUser;
        if(!user) {
            [[GIDSignIn sharedInstance] signInSilently];
        }
    }
    else {
        NSLog(@"Not signed in, start signing procedure");
        [signin signIn];
    }
}

void PlatformUtils::facebookLogin()
{
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    [login logInWithReadPermissions:@[@"email"] handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
     {
         if (error)
         {
             // Process error
         }
         else if (result.isCancelled)
         {
             // Handle cancellations
         }
         else
         {
             if ([result.grantedPermissions containsObject:@"email"])
             {
                 NSLog(@"result is:%@",result);
                 fetchUserInfo();
                 [login logOut];
             }
         }
     }];
}

void PlatformUtils::fetchUserInfo()
{
    if ([FBSDKAccessToken currentAccessToken])
    {
        QtFirebaseSocialSignIn::instance()->facebookSignIn(QString::fromNSString([[FBSDKAccessToken currentAccessToken]tokenString]));
    }
}
