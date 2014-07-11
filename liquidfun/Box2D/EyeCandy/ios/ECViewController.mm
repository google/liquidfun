#import "ECViewController.h"
#import <engine.h>
#import <CoreMotion/CoreMotion.h>

#define GRAVITY 9.80665

@interface ECViewController () {
	Engine *engine;
	NSOperationQueue *motionQueue;
	CMMotionManager *motionManager;
}
@property (strong, nonatomic) EAGLContext *context;
- (void)setupGL;
- (void)tearDownGL;
@end

@implementation ECViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }

    GLKView *view = (GLKView *)self.view;
    view.context = self.context;

	self.preferredFramesPerSecond = 60;

	if (!engine) {
		engine = new Engine(nullptr);
	}

	motionQueue = [NSOperationQueue new];
	motionManager = [CMMotionManager new];

    [self setupGL];
}

- (void)dealloc {
    [self tearDownGL];

    delete engine;

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;

        [self tearDownGL];

        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

}

- (void)setupGL {
    [EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL {
    [EAGLContext setCurrentContext:self.context];
}

- (BOOL)shouldAutorotate {
	return YES;
}

- (NSUInteger)supportedInterfaceOrientations {
	return UIInterfaceOrientationMaskLandscape;
}

- (void)viewDidAppear:(BOOL)animated {
	[super viewDidAppear:animated];
	CGRect bounds = self.view.bounds;
	CGFloat scale = self.view.contentScaleFactor;
	engine->SetSize(
		(int)(scale * CGRectGetWidth(bounds)),
		(int)(scale * CGRectGetHeight(bounds)));
	engine->SetAnimating(true);
	[EAGLContext setCurrentContext:self.context];
	engine->Init();
	[motionManager startAccelerometerUpdatesToQueue:motionQueue
		withHandler:^(CMAccelerometerData *accelerometerData, NSError *error) {
		CMAcceleration acceleration = accelerometerData.acceleration;
		switch ([[UIApplication sharedApplication] statusBarOrientation]) {
		default:
		case UIInterfaceOrientationPortrait:
			engine->SetGravity(
				-(float)(GRAVITY * acceleration.x),
				-(float)(GRAVITY * acceleration.y),
				-(float)(GRAVITY * acceleration.z));
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			engine->SetGravity(
				+(float)(GRAVITY * acceleration.x),
				+(float)(GRAVITY * acceleration.y),
				-(float)(GRAVITY * acceleration.z));
			break;
		case UIInterfaceOrientationLandscapeLeft:
			engine->SetGravity(
				+(float)(GRAVITY * acceleration.y),
				-(float)(GRAVITY * acceleration.x),
				-(float)(GRAVITY * acceleration.z));
			break;
		case UIInterfaceOrientationLandscapeRight:
			engine->SetGravity(
				-(float)(GRAVITY * acceleration.y),
				+(float)(GRAVITY * acceleration.x),
				-(float)(GRAVITY * acceleration.z));
			break;
		}
	}];
}

- (void)viewWillDisappear:(BOOL)animated {
	[super viewWillDisappear:animated];
	[motionManager stopAccelerometerUpdates];
	engine->SetAnimating(false);
	[EAGLContext setCurrentContext:self.context];
	engine->Terminate();
}

- (void)update {
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
	engine->DrawFrame();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		CGPoint point = [touch locationInView:self.view];
		CGFloat scale = self.view.contentScaleFactor;
		engine->TouchDown(scale * point.x, scale * point.y, (int)(intptr_t)touch);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		CGPoint point = [touch locationInView:self.view];
		CGFloat scale = self.view.contentScaleFactor;
		engine->TouchMove(scale * point.x, scale * point.y, (int)(intptr_t)touch);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		CGPoint point = [touch locationInView:self.view];
		CGFloat scale = self.view.contentScaleFactor;
		engine->TouchUp(scale * point.x, scale * point.y, (int)(intptr_t)touch);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		CGPoint point = [touch locationInView:self.view];
		CGFloat scale = self.view.contentScaleFactor;
		engine->TouchCancel(scale * point.x, scale * point.y, (int)(intptr_t)touch);
	}
}

@end
