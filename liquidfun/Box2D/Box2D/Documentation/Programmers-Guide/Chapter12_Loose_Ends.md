# Loose Ends

[User Data](#ud)<br/>
[Implicit Destruction](#id)<br/>
[Pixels and Coordinate Systems](#pcs)<br/>

<a name="ud"></a>
## User Data

The b2Fixture, b2Body, and b2Joint classes allow you to attach user data as a
void pointer. This is handy when you are examining LiquidFun data structures
and you want to determine how they relate to the objects in your game engine.

For example, it is typical to attach an actor pointer to the rigid body on
that actor. This sets up a circular reference. If you have the actor, you can
get the body. If you have the body, you can get the actor.

&nbsp;&nbsp;&nbsp;`GameActor* actor = GameCreateActor();`<br/>
&nbsp;&nbsp;&nbsp;`b2BodyDef bodyDef;`<br/>
&nbsp;&nbsp;&nbsp;`bodyDef.userData = actor;`<br/>
&nbsp;&nbsp;&nbsp;`actor->body = box2Dworld->CreateBody(&bodyDef);`<br/>

Here are some examples of cases where you would need the user data:

* Applying damage to an actor using a collision result.

* Playing a scripted event if the player is inside an axis-aligned box.

* Accessing a game structure when LiquidFun notifies you that a joint is going
to be destroyed.

Keep in mind that user data is optional and you can put anything in it.
However, you should be consistent. For example, if you want to store an actor
pointer on one body, you should keep an actor pointer on all bodies. Don't
store an actor pointer on one body, and a foo pointer on another body. Casting
an actor pointer to a foo pointer may lead to a crash.

User data pointers are NULL by default.

For fixtures you might consider defining a user data structure that lets you
store game specific information, such as material type, effects hooks, sound
hooks, etc.

&nbsp;&nbsp;&nbsp;`struct FixtureUserData`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`  int materialIndex;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`  . . .`<br/>
&nbsp;&nbsp;&nbsp;`};`<br/>
&nbsp;&nbsp;&nbsp;`FixtureUserData myData = new FixtureUserData;`<br/>
&nbsp;&nbsp;&nbsp;`myData->materialIndex = 2;`<br/>
&nbsp;&nbsp;&nbsp;`b2FixtureDef fixtureDef;`<br/>
&nbsp;&nbsp;&nbsp;`fixtureDef.shape = &someShape;`<br/>
&nbsp;&nbsp;&nbsp;`fixtureDef.userData = myData;`<br/>
&nbsp;&nbsp;&nbsp;`b2Fixture* fixture = body->CreateFixture(&fixtureDef);`<br/>
&nbsp;&nbsp;&nbsp;`. . .`<br/>
&nbsp;&nbsp;&nbsp;`delete fixture->GetUserData();`<br/>
&nbsp;&nbsp;&nbsp;`fixture->SetUserData(NULL);`<br/>
&nbsp;&nbsp;&nbsp;`body->DestroyFixture(fixture);`<br/>

<a name="id"></a>
## Implicit Destruction

LiquidFun doesn't use reference counting. So if you destroy a body it is
really gone. Accessing a pointer to a destroyed body has undefined behavior.
In other words, your program will likely crash and burn. To help fix these
problems, the debug build memory manager fills destroyed entities with
FDFDFDFD. This can help find problems more easily in some cases.

If you destroy a LiquidFun entity, it is up to you to make sure you remove all
references to the destroyed object. This is easy if you only have a single
reference to the entity. If you have multiple references, you might consider
implementing a handle class to wrap the raw pointer.

Often when using LiquidFun you will create and destroy many bodies, shapes,
and joints. Managing these entities is somewhat automated by LiquidFun. If you
destroy a body then all associated shapes and joints are automatically
destroyed. This is called implicit destruction.

When you destroy a body, all its attached shapes, joints, and contacts are
destroyed. This is called implicit destruction. Any body connected to one of
those joints and/or contacts is woken. This process is usually convenient.
However, you must be aware of one crucial issue:

	Caution
	
	When a body is destroyed, all fixtures and joints attached to the body
are automatically destroyed. You must nullify any pointers you have to those
shapes and joints. Otherwise, your program will die horribly if you try to
access or destroy those shapes or joints later.



To help you nullify your joint pointers, LiquidFun provides a listener class
named b2DestructionListener that you can implement and provide to your world
object. Then the world object will notify you when a joint is going to be
implicitly destroyed

 Note that there no notification when a joint or fixture is explicitly
destroyed. In this case ownership is clear and you can perform the necessary
cleanup on the spot. If you like, you can call your own implementation of
b2DestructionListener to keep cleanup code centralized.

Implicit destruction is a great convenience in many cases. It can also make
your program fall apart. You may store pointers to shapes and joints somewhere
in your code. These pointers become orphaned when an associated body is
destroyed. The situation becomes worse when you consider that joints are often
created by a part of the code unrelated to management of the associated body.
For example, the testbed creates a b2MouseJoint for interactive manipulation
of bodies on the screen.

LiquidFun provides a callback mechanism to inform your application when
implicit destruction occurs. This gives your application a chance to nullify
the orphaned pointers. This callback mechanism is described later in this
manual.

You can implement a b2DestructionListener that allows b2World to inform you
when a shape or joint is implicitly destroyed because an associated body was
destroyed. This will help prevent your code from accessing orphaned pointers.

&nbsp;&nbsp;&nbsp;`class MyDestructionListener : public
b2DestructionListener`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`void SayGoodbye(b2Joint* joint)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`// remove all
references to joint.`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`};`<br/>

You can then register an instance of your destruction listener with your world
object. You should do this during world initialization.

&nbsp;&nbsp;&nbsp;`myWorld->SetListener(myDestructionListener);`

<a name="pcs"></a>
## Pixels and Coordinate Systems

Recall that LiquidFun uses MKS (meters, kilograms, and seconds) units and
radians for angles. You may have trouble working with meters because your game
is expressed in terms of pixels. To deal with this in the testbed I have the
whole *game* work in meters and just use an OpenGL viewport transformation to
scale the world into screen space.

&nbsp;&nbsp;&nbsp;`float lowerX = -25.0f, upperX = 25.0f, lowerY = -5.0f,
upperY = 25.0f;`<br/>
&nbsp;&nbsp;&nbsp;`gluOrtho2D(lowerX, upperX, lowerY, upperY);`<br/>

If your game must work in pixel units then you should convert your length
units from pixels to meters when passing values from LiquidFun. Likewise you
should convert the values received from LiquidFun from meters to pixels. This
will improve the stability of the physics simulation.

You have to come up with a reasonable conversion factor. I suggest making this
choice based on the size of your characters. Suppose you have determined to
use 50 pixels per meter (because your character is 75 pixels tall). Then you
can convert from pixels to meters using these formulas:

&nbsp;&nbsp;&nbsp;`xMeters = 0.02f * xPixels;`<br/>
&nbsp;&nbsp;&nbsp;`yMeters = 0.02f * yPixels;`<br/>

In reverse:

&nbsp;&nbsp;&nbsp;`xPixels = 50.0f * xMeters;`<br/>
&nbsp;&nbsp;&nbsp;`yPixels = 50.0f * yMeters;`<br/>

You should consider using MKS units in your game code and just convert to
pixels when you render. This will simplify your game logic and reduce the
chance for errors since the rendering conversion can be isolated to a small
amount of code.

If you use a conversion factor, you should try tweaking it globally to make
sure nothing breaks. You can also try adjusting it to improve stability.


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
