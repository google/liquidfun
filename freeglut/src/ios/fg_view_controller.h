#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "fg_internal.h"

@interface FGViewController : GLKViewController

@property (assign) SFG_Window *sfgWindow;

- (id)initWithSFGWindow:(SFG_Window*)sfgWindow;

@end
