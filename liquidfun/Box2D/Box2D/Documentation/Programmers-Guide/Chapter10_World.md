# World Class

[About](#about)<br/>
[Creating and Destroying a World](#cdw)<br/>
[Using a World](#uw)<br/>
[Simulation](#sim)<br/>
[Exploring the World](#ew)<br/>
[AABB Queries](#ab)<br/>
[Ray Casts](#rc)<br/>
[Forces and Impulses](#fi)<br/>
[Coordinate Transformations](#ct)<br/>
[Lists](#lists)<br/>
<a name="about"></a><br/>
## About

The b2World class contains the bodies and joints. It manages all aspects of
the simulation and allows for asynchronous queries (like AABB queries and
ray-casts). Much of your interactions with LiquidFun will be with a b2World
object.

<a name="cdw"></a><br/>
## Creating and Destroying a World

Creating a world is fairly simple. You just need to provide a gravity vector
and a Boolean indicating if bodies can sleep. Usually you will create and
destroy a world using new and delete.

&nbsp;&nbsp;&nbsp;`b2World* myWorld = new b2World(gravity, doSleep);`<br/>
&nbsp;&nbsp;&nbsp;`... do stuff ...`<br/>
&nbsp;&nbsp;&nbsp;`delete myWorld;`<br/>

<a name="uw"></a><br/>
## Using a World

The world class contains factories for creating and destroying bodies and
joints. These factories are discussed later in the sections on bodies and
joints. There are some other interactions with b2World that I will cover now.

<a name="sim"></a><br/>
## Simulation

The world class is used to drive the simulation. You specify a time step and a
velocity and position iteration count. For example:

&nbsp;&nbsp;&nbsp;`float32 timeStep = 1.0f / 60.f;`<br/>
&nbsp;&nbsp;&nbsp;`int32 velocityIterations = 10;`<br/>
&nbsp;&nbsp;&nbsp;`int32 positionIterations = 8;`<br/>
&nbsp;&nbsp;&nbsp;`myWorld->Step(timeStep, velocityIterations,
positionIterations);`<br/>

After the time step you can examine your bodies and joints for information.
Most likely you will grab the position off the bodies so that you can update
your actors and render them. You can perform the time step anywhere in your
game loop, but you should be aware of the order of things. For example, you
must create bodies before the time step if you want to get collision results
for the new bodies in that frame.

As I discussed above in the HelloWorld tutorial, you should use a fixed time
step. By using a larger time step you can improve performance in low frame
rate scenarios. But generally you should use a time step no larger than 1/30
seconds. A time step of 1/60 seconds will usually deliver a high quality
simulation.

The iteration count controls how many times the constraint solver sweeps over
all the contacts and joints in the world. More iteration always yields a
better simulation. But don't trade a small time step for a large iteration
count. 60Hz and 10 iterations is far better than 30Hz and 20 iterations.

After stepping, you should clear any forces you have applied to your bodies.
This is done with the command b2World::ClearForces. This lets you take
multiple sub-steps with the same force field.

&nbsp;&nbsp;&nbsp;`myWorld->ClearForces();`<br/>


<a name="ew"></a><br/>
## Exploring the World

The world is a container for bodies, contacts, and joints. You can grab the
body, contact, and joint lists off the world and iterate over them. For
example, this code wakes up all the bodies in the world:

&nbsp;&nbsp;&nbsp;`for (b2Body* b = myWorld->GetBodyList(); b; b =
b->GetNext())`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`    b->SetAwake(true);`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

Unfortunately real programs can be more complicated. For example, the
following code is broken:

&nbsp;&nbsp;&nbsp;`for (b2Body* b = myWorld->GetBodyList(); b; b =
b->GetNext())`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`GameActor* myActor =
(GameActor*)b->GetUserData();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (myActor->IsDead())`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`myWorld->DestroyBody(b);
// ERROR: now GetNext returns garbage.`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

Everything goes ok until a body is destroyed. Once a body is destroyed, its
next pointer becomes invalid. So the call to b2Body::GetNext() will return
garbage. The solution to this is to copy the next pointer before destroying
the body.

&nbsp;&nbsp;&nbsp;`b2Body* node = myWorld->GetBodyList();`<br/>
&nbsp;&nbsp;&nbsp;`while (node)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Body* b = node;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`node = node->GetNext();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`GameActor* myActor =
(GameActor*)b->GetUserData();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (myActor->IsDead())`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`myWorld->DestroyBody(b);`
<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

This safely destroys the current body. However, you may want to call a game
function that may destroy multiple bodies. In this case you need to be very
careful. The solution is application specific, but for convenience I'll show
one method of solving the problem.

&nbsp;&nbsp;&nbsp;`b2Body* node = myWorld->GetBodyList();`<br/>
&nbsp;&nbsp;&nbsp;`while (node)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Body* b = node;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`node = node->GetNext();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`GameActor* myActor =
(GameActor*)b->GetUserData();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (myActor->IsDead())`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`bool
otherBodiesDestroyed = GameCrazyBodyDestroyer(b);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if
(otherBodiesDestroyed)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`node
= myWorld->GetBodyList();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

Obviously to make this work, GameCrazyBodyDestroyer must be honest about what
it has destroyed.

<a name="ab"></a><br/>
## AABB Queries

Sometimes you want to determine all the shapes in a region. The b2World class
has a fast log(N) method for this using the broad-phase data structure. You
provide an AABB in world coordinates and an implementation of b2QueryCallback.
The world calls your class with each fixture whose AABB overlaps the query
AABB. Return true to continue the query, otherwise return false. For example,
the following code finds all the fixtures that potentially intersect a
specified AABB and wakes up all of the associated bodies.

&nbsp;&nbsp;&nbsp;`class MyQueryCallback : public b2QueryCallback`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`public:`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`bool ReportFixture(b2Fixture*
fixture)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Body* body =
fixture->GetBody();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`body->SetAwake(true);`<br
/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`// Return true to
continue the query.`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`return true;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`};`<br/>
&nbsp;&nbsp;&nbsp;`...`<br/>
&nbsp;&nbsp;&nbsp;`MyQueryCallback callback;`<br/>
&nbsp;&nbsp;&nbsp;`b2AABB aabb;`<br/>
&nbsp;&nbsp;&nbsp;`aabb.lowerBound.Set(-1.0f, -1.0f);`<br/>
&nbsp;&nbsp;&nbsp;`aabb.upperBound.Set(1.0f, 1.0f);`<br/>
&nbsp;&nbsp;&nbsp;`myWorld->Query(&callback, aabb);`<br/>

You cannot make any assumptions about the order of the callbacks.

<a name="rc"></a><br/>
## Ray Casts

You can use ray casts to do line-of-sight checks, fire guns, etc. You perform
a ray cast by implementing a callback class and providing the start and end
points. The world class calls your class with each fixture hit by the ray.
Your callback is provided with the fixture, the point of intersection, the
unit normal vector, and the fractional distance along the ray. You cannot make
any assumptions about the order of the callbacks.

You control the continuation of the ray cast by returning a fraction.
Returning a fraction of zero indicates the ray cast should be terminated. A
fraction of one indicates the ray cast should continue as if no hit occurred.
If you return the fraction from the argument list, the ray will be clipped to
the current intersection point. So you can ray cast any shape, ray cast all
shapes, or ray cast the closest shape by returning the appropriate fraction.

You may also return of fraction of -1 to filter the fixture. Then the ray cast
will proceed as if the fixture does not exist.

Here is an example:

&nbsp;&nbsp;&nbsp;`// This class captures the closest hit shape.`<br/>
&nbsp;&nbsp;&nbsp;`class MyRayCastCallback : public b2RayCastCallback`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`public:`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`MyRayCastCallback()`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`m_fixture = NULL;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`float32 ReportFixture(b2Fixture* fixture,
const b2Vec2& point, const b2Vec2& normal, float32 fraction)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`m_fixture =
fixture;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`m_point = point;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`m_normal = normal;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`m_fraction =
fraction;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`return fraction;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Fixture* m_fixture;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Vec2 m_point;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Vec2 m_normal;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`float32 m_fraction;`<br/>
&nbsp;&nbsp;&nbsp;`};`<br/>
&nbsp;&nbsp;&nbsp;`MyRayCastCallback callback;`<br/>
&nbsp;&nbsp;&nbsp;`b2Vec2 point1(-1.0f, 0.0f);`<br/>
&nbsp;&nbsp;&nbsp;`b2Vec2 point2(3.0f, 1.0f);`<br/>
&nbsp;&nbsp;&nbsp;`myWorld->RayCast(&callback, point1, point2);`<br/>

	Caution
	
	Due to round-off errors, ray casts can sneak through small cracks
between polygons in your static environment. If this is not acceptable in your
application, please enlarge your polygons slightly.

&nbsp;&nbsp;&nbsp;`void SetLinearVelocity(const b2Vec2& v);`<br/>
&nbsp;&nbsp;&nbsp;`b2Vec2 GetLinearVelocity() const;`<br/>
&nbsp;&nbsp;&nbsp;`void SetAngularVelocity(float32 omega);`<br/>
&nbsp;&nbsp;&nbsp;`float32 GetAngularVelocity() const;`<br/>

<a name="fi"></a><br/>
## Forces and Impulses

You can apply forces, torques, and impulses to a body. When you apply a force
or an impulse, you provide a world point where the load is applied. This often
results in a torque about the center of mass.

void ApplyForce(const b2Vec2& force, const b2Vec2& point);
void ApplyTorque(float32 torque);
void ApplyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point);
void ApplyAngularImpulse(float32 impulse);

Applying a force, torque, or impulse wakes the body. Sometimes this is
undesirable. For example, you may be applying a steady force and want to allow
the body to sleep to improve performance. In this case you can use the
following code.

&nbsp;&nbsp;&nbsp;`if (myBody->IsAwake() == true)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`myBody->ApplyForce(myForce,
myPoint);`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

You can apply forces and impulses to particles and particle groups, as well.
Unlike with bodies, however, the load is not applied to an arbitrary world
point. Instead, it acts upon the center of each particle.

You can apply forces and impulses to individual particles or to particle groups.
The following example spreads an impulse of (0.7, 0.3) kg m/s across the
particles in myParticleGroup:

&nbsp;&nbsp;&nbsp;`const b2Vec2 impulse(0.7f, 0.3f);`
&nbsp;&nbsp;&nbsp;`myParticleGroup->ApplyLinearImpulse(impulse);`

<a name="ct"></a><br/>
## Coordinate Transformations

The body class has some utility functions to help you transform points and
vectors between local and world space. If you don't understand these concepts,
please read "Essential Mathematics for Games and Interactive Applications" by
Jim Van Verth and Lars Bishop. These functions are efficient (when inlined).

&nbsp;&nbsp;&nbsp;`b2Vec2 GetWorldPoint(const b2Vec2& localPoint);`<br/>

&nbsp;&nbsp;&nbsp;`b2Vec2 GetWorldVector(const b2Vec2& localVector);`<br/>

&nbsp;&nbsp;&nbsp;`b2Vec2 GetLocalPoint(const b2Vec2& worldPoint);`<br/>

&nbsp;&nbsp;&nbsp;`b2Vec2 GetLocalVector(const b2Vec2& worldVector);`<br/>

<a name="lists"></a><br/>
## Lists

You can iterate over a body's fixtures. This is mainly useful if you need to
access the fixture's user data.

&nbsp;&nbsp;&nbsp;`for (b2Fixture* f = body->GetFixtureList(); f; f =
f->GetNext())`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`MyFixtureData* data =
(MyFixtureData*)f->GetUserData();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`… do something with data …`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

You can similarly iterate over the body's joint list.

The body also provides a list of associated contacts. You can use this to get
information about the current contacts. Be careful, because the contact list
may not contain all the contacts that existed during the previous time step.


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
