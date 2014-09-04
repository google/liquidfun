#import "fg_view_controller.h"
#import "fg_internal.h"
#import <GL/freeglut.h>

void fghRedrawWindow ( SFG_Window *window );

@interface FGViewController ()
@property (strong, nonatomic) EAGLContext *context;
- (void)setupGL;
- (void)tearDownGL;
@end

@implementation FGViewController

@synthesize sfgWindow;

- (id)initWithSFGWindow:(SFG_Window*)aWindow
{
    self = [super init];
    if (self)
    {
        sfgWindow = aWindow;
    }
    return self;
}

- (void)dealloc
{
    [self tearDownGL];
    if ([EAGLContext currentContext] == self.context)
    {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    EAGLContext *context =
        [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    self.context = context;
    if (!context)
    {
        NSLog(@"Failed to create ES context");
    }
    GLKView *view = (GLKView *)self.view;
    view.context = context;
    self.preferredFramesPerSecond = 60;
    sfgWindow->Window.Handle = self;
    sfgWindow->Window.Context = context;
    [self setupGL];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil))
    {
        self.view = nil;

        [self tearDownGL];

        if ([EAGLContext currentContext] == self.context)
        {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)update
{
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    fghRedrawWindow(self.sfgWindow);
}

typedef struct
{
    int button;
    CGPoint location;
} MouseEvent;

- (MouseEvent)mouseEventFromTouches:(NSSet *)touches
{
    MouseEvent event;
    CGPoint sum = CGPointZero;
    UIView *view = self.view;
    for (UITouch *touch in touches)
    {
        CGPoint location = [touch locationInView:view];
        sum.x += location.x;
        sum.y += location.y;
    }
    NSUInteger count = [touches count];
    CGFloat scale = view.contentScaleFactor;
    event.button = (int)count-1;
    event.location.x = scale/count*sum.x;
    event.location.y = scale/count*sum.y;
    return event;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    MouseEvent mouse = [self mouseEventFromTouches:touches];
    INVOKE_WCB(
        *sfgWindow,
        Mouse,
        (
            mouse.button,
            GLUT_DOWN,
            mouse.location.x,
            mouse.location.y
        )
    );
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    MouseEvent mouse = [self mouseEventFromTouches:touches];
    INVOKE_WCB(
        *sfgWindow,
        Motion,
        (
            mouse.location.x,
            mouse.location.y
        )
    );
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    MouseEvent mouse = [self mouseEventFromTouches:touches];
    INVOKE_WCB(
        *sfgWindow,
        Mouse,
        (
            mouse.button,
            GLUT_UP,
            mouse.location.x,
            mouse.location.y
        )
    );
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    MouseEvent mouse = [self mouseEventFromTouches:touches];
    INVOKE_WCB(
        *sfgWindow,
        Mouse,
        (
            mouse.button,
            GLUT_UP,
            mouse.location.x,
            mouse.location.y
        )
    );
}

@end
