#import "viewdelegate.h"
#import "../qtfirebasesocialsignin.h"

#import "Firebase/Invites/GoogleSignIn.framework/Headers/GoogleSignIn.h"


@interface QIOSViewController : UIResponder <UIPageViewControllerDelegate, GIDSignInDelegate, GIDSignInUIDelegate>
@end

@interface QIOSViewController(viewdelegate)
@end

@implementation QIOSViewController (viewdelegate)

- (void)viewDidLoad {
    [super viewDidLoad];
    
    UIApplication* app;
    UIWindow* rootWindow;
    UIViewController* rootViewController;
    
    app = [UIApplication sharedApplication];
    rootWindow = app.windows[0];
    rootViewController = rootWindow.rootViewController;
    
    GIDSignIn *signIn = [GIDSignIn sharedInstance];
    signIn.shouldFetchBasicProfile = YES;
    signIn.delegate = self;
    signIn.uiDelegate = self;
}

- (void)signIn:(GIDSignIn *)signIn
didSignInForUser:(GIDGoogleUser *)user
     withError:(NSError *)error {
    // ...
    if (error == nil) {
        GIDAuthentication *authentication = user.authentication;
        QtFirebaseSocialSignIn::instance()->googleSignIn(QString::fromNSString(authentication.idToken));
        // ...
    } else {
        // ...
    }
}

- (void)signIn:(GIDSignIn *)signIn
didDisconnectWithUser:(GIDGoogleUser *)user
     withError:(NSError *)error {
    // Perform any operations when the user disconnects from app here.
    // ...
}
@end
