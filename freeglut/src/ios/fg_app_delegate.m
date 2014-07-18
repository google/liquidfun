
#import "fg_app_delegate.h"
#import "fg_view_controller.h"
#import "fg_internal.h"
#import "fg_main_ios.h"

@interface FGAppDelegate ()

@property (copy) NSArray *windows;
@property (strong) CADisplayLink *displayLink;

- (void)pollEvents:(CADisplayLink *)sender;

@end

@implementation FGAppDelegate

@synthesize windows;
@synthesize displayLink;

- (void)dealloc
{
    self.windows = nil;
    self.displayLink = nil;
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application
didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Override point for customization after application launch.
    CGRect frame = [[UIScreen mainScreen] bounds];
    NSMutableArray *mutableWindows = [NSMutableArray array];
    for (SFG_Window *sfgWindow = (SFG_Window*)fgStructure.Windows.First;
         sfgWindow;
         sfgWindow = (SFG_Window*)sfgWindow->Node.Next)
    {
        if (sfgWindow->IsMenu) continue;
        UIWindow *window = [[[UIWindow alloc] initWithFrame:frame] autorelease];
        FGViewController *viewController =
           [[[FGViewController alloc] initWithSFGWindow:sfgWindow] autorelease];
        window.rootViewController = viewController;
        [window makeKeyAndVisible];
        [mutableWindows addObject:window];
    }
    self.windows = mutableWindows;
    self.displayLink = [CADisplayLink
                    displayLinkWithTarget:self selector:@selector(pollEvents:)];
    [displayLink
          addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state.
    // This can occur for certain types of temporary interruptions (such as an
    // incoming phone call or SMS message) or when the user quits the
    // application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down
    // OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate
    // timers, and store enough application state information to restore your
    // application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called
    // instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive
    // state; here you can undo many of the changes made on entering the
    // background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the
    // application was inactive. If the application was previously in the
    // background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if
    // appropriate. See also applicationDidEnterBackground:.
}

- (void)pollEvents:(CADisplayLink *)sender
{

    if (fgState.ExecState != GLUT_EXEC_STATE_RUNNING) exit(0);

    SFG_Window *window;

    glutMainLoopEvent();
    /*
     * Step through the list of windows, seeing if there are any
     * that are not menus
     */
    for ( window = (SFG_Window*)fgStructure.Windows.First;
          window;
          window = (SFG_Window*)window->Node.Next)
    {
        if (!(window->IsMenu))
        {
            break;
        }
    }
    if (!window)
    {
        fgState.ExecState = GLUT_EXEC_STATE_STOP;
    }
    else
    {
        if ( fgState.IdleCallback)
        {
            if ( fgStructure.CurrentWindow &&
                 fgStructure.CurrentWindow->IsMenu)
                /* fail safe */
                fgSetWindow(window);
            fgState.IdleCallback();
        }
    }

}

@end
