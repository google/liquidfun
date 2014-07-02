# Bodies

[About](#about)<br/>
[Body Definition](#bd)<br/>
[Body Factory](#bf)<br/>
[Using a Body](#ub)<br/>


<a name="about">
## About

Bodies have position and velocity. You can apply forces, torques, and impulses
to bodies. Bodies can be static, kinematic, or dynamic. Here are the body type
definitions:

### b2_staticBody

A static body does not move under simulation and behaves as if it has infinite
mass. Internally, LiquidFun stores zero for the mass and the inverse mass.
Static bodies can be moved manually by the user. A static body has zero
velocity. Static bodies do not collide with other static or kinematic bodies.

### b2_kinematicBody

A kinematic body moves under simulation according to its velocity. Kinematic
bodies do not respond to forces. They can be moved manually by the user, but
normally a kinematic body is moved by setting its velocity. A kinematic body
behaves as if it has infinite mass, however, LiquidFun stores zero for the
mass and the inverse mass. Kinematic bodies do not collide with other
kinematic or static bodies.

### b2_dynamicBody

A dynamic body is fully simulated. They can be moved manually by the user, but
normally they move according to forces. A dynamic body can collide with all
body types. A dynamic body always has finite, non-zero mass. If you try to set
the mass of a dynamic body to zero, it will automatically acquire a mass of
one kilogram and it won’t rotate.

Bodies are the backbone for fixtures (shapes). Bodies carry fixtures and move
them around in the world. Bodies are always rigid bodies in LiquidFun. That
means that two fixtures attached to the same rigid body never move relative to
each other and fixtures attached to the same body don’t collide.

Fixtures have collision geometry and density. Normally, bodies acquire their
mass properties from the fixtures. However, you can override the mass
properties after a body is constructed.

You usually keep pointers to all the bodies you create. This way you can query
the body positions to update the positions of your graphical entities. You
should also keep body pointers so you can destroy them when you are done with
them.

<a name="bd">
## Body Definition

Before a body is created you must create a body definition (b2BodyDef). The
body definition holds the data needed to create and initialize a body.

LiquidFun copies the data out of the body definition; it does not keep a
pointer to the body definition. This means you can recycle a body definition
to create multiple bodies.

Let’s go over some of the key members of the body definition.

### Body Type

As discussed at the beginning of this chapter, there are three different body
types: static, kinematic, and dynamic. You should establish the body type at
creation because changing the body type later is expensive.

&nbsp;&nbsp;&nbsp;`bodyDef.type = b2_dynamicBody;`<br/>

Setting the body type is mandatory.

### Position and Angle

The body definition gives you the chance to initialize the position of the
body on creation. This has far better performance than creating the body at
the world origin and then moving the body.

	Caution
	
	Do not create a body at the origin and then move it. If you create
several bodies at the origin, then performance will suffer.



A body has two main points of interest. The first point is the body's origin.
Fixtures and joints are attached relative to the body's origin. The second
point of interest is the center of mass. The center of mass is determined from
mass distribution of the attached shapes or is explicitly set with b2MassData.
Much of LiquidFun's internal computations use the center of mass position. For
example b2Body stores the linear velocity for the center of mass.

When you are building the body definition, you may not know where the center
of mass is located. Therefore you specify the position of the body's origin.
You may also specify the body's angle in radians, which is not affected by the
position of the center of mass. If you later change the mass properties of the
body, then the center of mass may move on the body, but the origin position
does not change and the attached shapes and joints do not move.

&nbsp;&nbsp;&nbsp;`bodyDef.position.Set(0.0f, 2.0f);   // the body's origin
position.`<br/>
&nbsp;&nbsp;&nbsp;`bodyDef.angle = 0.25f * b2_pi;      // the body's angle in
radians.`<br/>

A rigid body is also a frame of reference. You can define fixtures and joints
in that frame. Those fixtures and joint anchors never move in the local frame
of the body.

### Damping

Damping is used to reduce the world velocity of bodies. Damping is different
than friction because friction only occurs with contact. Damping is not a
replacement for friction and the two effects should be used together.

Damping parameters should be between 0 and infinity, with 0 meaning no
damping, and infinity meaning full damping. Normally you will use a damping
value between 0 and 0.1. I generally do not use linear damping because it
makes bodies look floaty.

&nbsp;&nbsp;&nbsp;`bodyDef.linearDamping = 0.0f;`<br/>
&nbsp;&nbsp;&nbsp;`bodyDef.angularDamping = 0.01f;`<br/>

Damping is approximated for stability and performance. At small damping values
the damping effect is mostly independent of the time step. At larger damping
values, the damping effect will vary with the time step. This is not an issue
if you use a fixed time step (recommended).

### Gravity Scale

You can use the gravity scale to adjust the gravity on a single body. Be
careful though, increased gravity can decrease stability.

&nbsp;&nbsp;&nbsp;`// Set the gravity scale to zero so this body will
float`<br/>
&nbsp;&nbsp;&nbsp;`bodyDef.gravityScale = 0.0f;`<br/>

### Sleep Parameters

What does sleep mean? Well it is expensive to simulate bodies, so the less we
have to simulate the better. When a body comes to rest we would like to stop
simulating it.

When LiquidFun determines that a body (or group of bodies) has come to rest,
the body enters a sleep state which has very little CPU overhead. If a body is
awake and collides with a sleeping body, then the sleeping body wakes up.
Bodies will also wake up if a joint or contact attached to them is destroyed.
You can also wake a body manually.

The body definition lets you specify whether a body can sleep and whether a
body is created sleeping.

&nbsp;&nbsp;&nbsp;`bodyDef.allowSleep = true;`<br/>
&nbsp;&nbsp;&nbsp;`bodyDef.awake = true;`<br/>

### Fixed Rotation

You may want a rigid body, such as a character, to have a fixed rotation. Such
a body should not rotate, even under load. You can use the fixed rotation
setting to achieve this:

&nbsp;&nbsp;&nbsp;`bodyDef.fixedRotation = true;`<br/>

The fixed rotation flag causes the rotational inertia and its inverse to be
set to zero.

### Bullets

Game simulation usually generates a sequence of images that are played at some
frame rate. This is called discrete simulation. In discrete simulation, rigid
bodies can move by a large amount in one time step. If a physics engine
doesn't account for the large motion, you may see some objects incorrectly
pass through each other. This effect is called tunneling.

By default, LiquidFun uses continuous collision detection (CCD) to prevent
dynamic bodies from tunneling through static bodies. This is done by sweeping
shapes from their old position to their new positions. The engine looks for
new collisions during the sweep and computes the time of impact (TOI) for
these collisions. Bodies are moved to their first TOI and then halted for the
remainder of the time step.

Normally CCD is not used between dynamic bodies. This is done to keep
performance reasonable. In some game scenarios you need dynamic bodies to use
CCD. For example, you may want to shoot a high speed bullet at a stack of
dynamic bricks. Without CCD, the bullet might tunnel through the bricks.

Fast moving objects in LiquidFun can be labeled as bullets. Bullets will
perform CCD with both static and dynamic bodies. You should decide what bodies
should be bullets based on your game design. If you decide a body should be
treated as a bullet, use the following setting.

&nbsp;&nbsp;&nbsp;`bodyDef.bullet = true;`<br/>

The bullet flag only affects dynamic bodies.

LiquidFun performs continuous collision sequentially, so bullets may miss fast
moving bodies.

### Activation

You may wish a body to be created but not participate in collision or
dynamics. This state is similar to sleeping except the body will not be woken
by other bodies and the body's fixtures will not be placed in the broad-phase.
This means the body will not participate in collisions, ray casts, etc.

You can create a body in an inactive state and later re-activate it.

&nbsp;&nbsp;&nbsp;`bodyDef.active = true;`<br/>

Joints may be connected to inactive bodies. These joints will not be
simulated. You should be careful when you activate a body that its joints are
not distorted.

### User Data

User data is a void pointer. This gives you a hook to link your application
objects to bodies. You should be consistent to use the same object type for
all body user data.

&nbsp;&nbsp;&nbsp;`b2BodyDef bodyDef;`<br/>
&nbsp;&nbsp;&nbsp;`bodyDef.userData = &myActor;`<br/>

<a name="bf">
## Body Factory

Bodies are created and destroyed using a body factory provided by the world
class. This lets the world create the body with an efficient allocator and add
the body to the world data structure.

Bodies can be dynamic or static depending on the mass properties. Both body
types use the same creation and destruction methods.

&nbsp;&nbsp;&nbsp;`b2Body* dynamicBody = myWorld->CreateBody(&bodyDef);`<br/>
&nbsp;&nbsp;&nbsp;`... do stuff …`<br/>
&nbsp;&nbsp;&nbsp;`myWorld->DestroyBody(dynamicBody);`<br/>
&nbsp;&nbsp;&nbsp;`dynamicBody = NULL;`<br/>

	Caution
	
	You should never use new or malloc to create a body. The world won't
know about the body and the body won't be properly initialized.



Static bodies do not move under the influence of other bodies. You may
manually move static bodies, but you should be careful so that you don't
squash dynamic bodies between two or more static bodies. Friction will not
work correctly if you move a static body. Static bodies never collide with
static or kinematic bodies. It is faster to attach several shapes to a static
body than to create several static bodies with a single shape on each one.
Internally, LiquidFun sets the mass and inverse mass of static bodies to zero.
This makes the math work out so that most algorithms don't need to treat
static bodies as a special case.

LiquidFun does not keep a reference to the body definition or any of the data
it holds (except user data pointers). So you can create temporary body
definitions and reuse the same body definitions.

LiquidFun allows you to avoid destroying bodies by deleting your b2World
object, which does all the cleanup work for you. However, you should be
mindful to nullify body pointers that you keep in your game engine.

When you destroy a body, the attached fixtures and joints are automatically
destroyed. This has important implications for how you manage shape and joint
pointers.

<a name="ub">
## Using a Body

After creating a body, there are many operations you can perform on the body.
These include setting mass properties, accessing position and velocity,
applying forces, and transforming points and vectors.

### Mass Data

Every body has a mass (scalar), center of mass (2-vector), and rotational
inertia (scalar). For static bodies, the mass and rotational inertia are set
to zero. When a body has fixed rotation, its rotational inertia is zero.

Normally the mass properties of a body are established automatically when
fixtures are added to the body. You can also adjust the mass of a body at
run-time. This is usually done when you have special game scenarios that
require altering the mass.

&nbsp;&nbsp;&nbsp;`void SetMassData(const b2MassData* data);`<br/>

After setting a body's mass directly, you may wish to revert to the natural
mass dictated by the fixtures. You can do this with:

&nbsp;&nbsp;&nbsp;`void ResetMassData();`<br/>

The body's mass data is available through the following functions:

&nbsp;&nbsp;&nbsp;`float32 GetMass() const;`

&nbsp;&nbsp;&nbsp;`float32 GetInertia() const;`

&nbsp;&nbsp;&nbsp;`const b2Vec2& GetLocalCenter() const;`

&nbsp;&nbsp;&nbsp;`void GetMassData(b2MassData* data) const;`

### State Information

There are many aspects to the body's state. You can access this state data
efficiently through the following functions:

&nbsp;&nbsp;&nbsp;`void SetType(b2BodyType type);`

&nbsp;&nbsp;&nbsp;`b2BodyType GetType();`

&nbsp;&nbsp;&nbsp;`void SetBullet(bool flag);`

&nbsp;&nbsp;&nbsp;`bool IsBullet() const;`

&nbsp;&nbsp;&nbsp;`void SetSleepingAllowed(bool flag);`

&nbsp;&nbsp;&nbsp;`bool IsSleepingAllowed() const;`

&nbsp;&nbsp;&nbsp;`void SetAwake(bool flag);`

&nbsp;&nbsp;&nbsp;`bool IsAwake() const;`

&nbsp;&nbsp;&nbsp;`void SetActive(bool flag);`

&nbsp;&nbsp;&nbsp;`bool IsActive() const;`

&nbsp;&nbsp;&nbsp;`void SetFixedRotation(bool flag);`

&nbsp;&nbsp;&nbsp;`bool IsFixedRotation() const;`

### Position and Velocity

You can access the position and rotation of a body. This is common when
rendering your associated game actor. You can also set the position, although
this is less common since you will normally use LiquidFun to simulate movement.

&nbsp;&nbsp;&nbsp;`bool SetTransform(const b2Vec2& position, float32
angle);`<br/>
&nbsp;&nbsp;&nbsp;`const b2Transform& GetTransform() const;`<br/>
&nbsp;&nbsp;&nbsp;`const b2Vec2& GetPosition() const;`<br/>
&nbsp;&nbsp;&nbsp;`float32 GetAngle() const;`<br/>

You can access the center of mass position in local and world coordinates.
Much of the internal simulation in LiquidFun uses the center of mass. However,
you should normally not need to access it. Instead you will usually work with
the body transform. For example, you may have a body that is square. The body
origin might be a corner of the square, while the center of mass is located at
the center of the square.

&nbsp;&nbsp;&nbsp;`const b2Vec2& GetWorldCenter() const;`<br/>
&nbsp;&nbsp;&nbsp;`const b2Vec2& GetLocalCenter() const;`<br/>

You can access the linear and angular velocity. The linear velocity is for the
center of mass. Therefore, the linear velocity may change if the mass
properties change.


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
