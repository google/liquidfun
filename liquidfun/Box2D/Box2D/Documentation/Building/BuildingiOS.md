# Building for iOS

The LiquidFun source contains [Xcode][] projects to build the Testbed
application and EyeCandy demo. Unlike the OS X projects, the iOS Xcode
projects are *not* generated using [cmake][]. They must be maintained
manually.

### Version Requirements

Following are the minimum tested versions for building the tools and
libraries you on iOS.

-   OS X: Mavericks 10.9.3.
-   Xcode: 5.1.1

### Building with [Xcode][]

-   Double-click `liquidfun/Box2D/Testbed/ios/Testbed.xcodeproj` or
    `liquidfun/Box2D/EyeCandy/ios/EyeCandy.xcodeproj` to open the project
    in [Xcode][].
-   Select "Product-->Build" from the menu.

### Executing a Testbed or EyeCandy

-   Select a `Scheme`, for example "Testbed-->iPhone Retina (3.5-inch)",
	from the combo box to the right of the "Run" button.
-   Click the "Run" button.


  [Xcode]: http://developer.apple.com/xcode/
  [cmake]: http://www.cmake.org
  [OS X]: http://www.apple.com/osx/
