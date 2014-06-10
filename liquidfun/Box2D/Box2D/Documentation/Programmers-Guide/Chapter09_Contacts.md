# Contacts

[About](#about)<br/>
[Contact Class](#cc)<br/>
[Accessing Contacts](#ac)<br/>
[Contact Listener](#cl)<br/>
[Contact Filtering](#cf)<br/>


<a name="about"></a>
## About

Contacts are objects created by LiquidFun to manage collision between two
fixtures. If the fixture has children, such as a chain shape, then a contact
exists for each relevant child. There are different kinds of contacts, derived
from b2Contact, for managing contact between different kinds of fixtures. For
example there is a contact class for managing polygon-polygon collision and
another contact class for managing circle-circle collision.

Here is some terminology associated with contacts.

### contact point

A contact point is a point where two shapes touch. LiquidFun approximates
contact with a small number of points.

### contact normal

A contact normal is a unit vector that points from one shape to another. By
convention, the normal points from fixtureA to fixtureB.

### contact separation

Separation is the opposite of penetration. Separation is negative when shapes
overlap. It is possible that future versions of LiquidFun will create contact
points with positive separation, so you may want to check the sign when
contact points are reported.

### contact manifold

Contact between two convex polygons may generate up to 2 contact points. Both
of these points use the same normal, so they are grouped into a contact
manifold, which is an approximation of a continuous region of contact.

### normal impulse

The normal force is the force applied at a contact point to prevent the shapes
from penetrating. For convenience, LiquidFun works with impulses. The normal
impulse is just the normal force multiplied by the time step.

### tangent impulse

The tangent force is generated at a contact point to simulate friction. For
convenience, this is stored as an impulse.

### contact ids

LiquidFun tries to re-use the contact force results from a time step as the
initial guess for the next time step. LiquidFun uses contact ids to match
contact points across time steps. The ids contain geometric features indices
that help to distinguish one contact point from another.

Contacts are created when two fixture’s AABBs overlap. Sometimes collision
filtering will prevent the creation of contacts. Contacts are destroyed with
the AABBs cease to overlap.

So you might gather that there may be contacts created for fixtures that are
not touching (just their AABBs). Well, this is correct. It's a "chicken or egg"
problem. We don't know if we need a contact object until one is created
to analyze the collision. We could delete the contact right away if the shapes
are not touching, or we can just wait until the AABBs stop overlapping.
LiquidFun takes the latter approach because it lets the system cache
information to improve performance.

<a name="cc">
## Contact Class

As mentioned before, the contact class is created and destroyed by LiquidFun.
Contact objects are not created by the user. However, you are able to access
the contact class and interact with it.

You can access the raw contact manifold:

&nbsp;&nbsp;&nbsp;`b2Manifold* GetManifold();`<br/>
&nbsp;&nbsp;&nbsp;`const b2Manifold* GetManifold() const;`<br/>

You can potentially modify the manifold, but this is generally not supported
and is for advanced usage.

There is a helper function to get the b2WorldManifold:

&nbsp;&nbsp;&nbsp;`void GetWorldManifold(b2WorldManifold* worldManifold)
const;`<br/>

This uses the current positions of the bodies to compute world positions of
the contact points.

Sensors do not create manifolds, so for them use:

&nbsp;&nbsp;&nbsp;`bool touching = sensorContact->IsTouching();`<br/>

This function also works for non-sensors.

You can get the fixtures from a contact. From those you can get the bodies.

&nbsp;&nbsp;&nbsp;`b2Fixture* fixtureA = myContact->GetFixtureA();`<br/>
&nbsp;&nbsp;&nbsp;`b2Body* bodyA = fixtureA->GetBody();`<br/>
&nbsp;&nbsp;&nbsp;`MyActor* actorA = (MyActor*)bodyA->GetUserData();`<br/>

You can disable a contact. This only works inside the
b2ContactListener::PreSolve event, discussed below.

<a name="ac">
## Accessing Contacts

You can get access to contacts in several ways. You can access the contacts
directly on the world and body structures. You can also implement a contact
listener.

You can iterate over all contacts in the world:

&nbsp;&nbsp;&nbsp;`for (b2Contact* c = myWorld->GetContactList(); c; c =
c->GetNext())`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`// process c`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

You can also iterate over all the contacts on a body. These are stored in a
graph using a contact edge structure.

&nbsp;&nbsp;&nbsp;`for (b2ContactEdge* ce = myBody->GetContactList(); ce; ce =
ce->next)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`  b2Contact* c = ce->contact;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`  // process c`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

You can also access contacts using the contact listener that is described
below.

	Caution
	
	Accessing contacts off b2World and b2Body may miss some transient
contacts that occur in the middle of the time step. Use b2ContactListener to
get the most accurate results.

<a name="cl">
## Contact Listener

You can receive contact data by implementing b2ContactListener. The contact
listener supports several events: begin, end, pre-solve, and post-solve.

&nbsp;&nbsp;&nbsp;`class MyContactListener : public b2ContactListener`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`public:`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`void BeginContact(b2Contact*
contact)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{ /* handle begin event */ }`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`void EndContact(b2Contact* contact)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{ /* handle end event */ }`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`void PreSolve(b2Contact* contact, const
b2Manifold* oldManifold)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{ /* handle pre-solve event */ }`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`void PostSolve(b2Contact* contact, const
b2ContactImpulse* impulse)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{ /* handle post-solve event */ }`<br/>
&nbsp;&nbsp;&nbsp;`};`<br/>

	Caution
	
	Do not keep a reference to the pointers sent to b2ContactListener.
Instead make a deep copy of the contact point data into your own buffer. The
example below shows one way of doing this.
	
	At run-time you can create an instance of the listener and register it
with b2World::SetContactListener. Be sure your listener remains in scope while
the world object exists.

### Begin Contact Event

This is called when two fixtures begin to overlap. This is called for sensors
and non-sensors. This event can only occur inside the time step.

### End Contact Event

This is called when two fixtures cease to overlap. This is called for sensors
and non-sensors. This may be called when a body is destroyed, so this event
can occur outside the time step.

### Pre-Solve Event

This is called after collision detection, but before collision resolution.
This gives you a chance to disable the contact based on the current
configuration. For example, you can implement a one-sided platform using this
callback and calling b2Contact::SetEnabled(false). The contact will be
re-enabled each time through collision processing, so you will need to disable
the contact every time-step. The pre-solve event may be fired multiple times
per time step per contact due to continuous collision detection.

&nbsp;&nbsp;&nbsp;`void PreSolve(b2Contact* contact, const b2Manifold*
oldManifold)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;`b2WorldManifold worldManifold;`<br/>
&nbsp;&nbsp;&nbsp;`contact->GetWorldManifold(&worldManifold);`<br/>
&nbsp;&nbsp;&nbsp;`if (worldManifold.normal.y < -0.5f)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`contact->SetEnabled(false);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

The pre-solve event is also a good place to determine the point state and the
approach velocity of collisions.

&nbsp;&nbsp;&nbsp;`void PreSolve(b2Contact* contact, const b2Manifold*
oldManifold)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2WorldManifold worldManifold;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`contact->GetWorldManifold(&worldManifold);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2PointState state1[2], state2[2];`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2GetPointStates(state1, state2,
oldManifold, contact->GetManifold());`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (state2[0] == b2_addState)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`const b2Body* bodyA =
contact->GetFixtureA()->GetBody();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`const b2Body* bodyB =
contact->GetFixtureB()->GetBody();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Vec2 point =
worldManifold.points[0];`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Vec2 vA =
bodyA->GetLinearVelocityFromWorldPoint(point);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Vec2 vB =
bodyB->GetLinearVelocityFromWorldPoint(point);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`float32
approachVelocity = b2Dot(vB – vA, worldManifold.normal);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (approachVelocity >
1.0f)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`MyPlayCollisionSound();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

### Post-Solve Event

The post solve event is where you can gather collision impulse results. If you
don’t care about the impulses, you should probably just implement the
pre-solve event.

It is tempting to implement game logic that alters the physics world inside a
contact callback. For example, you may have a collision that applies damage
and try to destroy the associated actor and its rigid body. However, LiquidFun
does not allow you to alter the physics world inside a callback because you
might destroy objects that LiquidFun is currently processing, leading to
orphaned pointers.

The recommended practice for processing contact points is to buffer all
contact data that you care about and process it after the time step. You
should always process the contact points immediately after the time step;
otherwise some other client code might alter the physics world, invalidating
the contact buffer. When you process the contact buffer you can alter the
physics world, but you still need to be careful that you don't orphan pointers
stored in the contact point buffer. The testbed has example contact point
processing that is safe from orphaned pointers.

This code from the CollisionProcessing test shows how to handle orphaned
bodies when processing the contact buffer. Here is an excerpt. Be sure to read
the comments in the listing. This code assumes that all contact points have
been buffered in the b2ContactPoint array m_points.

&nbsp;&nbsp;&nbsp;`// We are going to destroy some bodies according to
contact`<br/>
&nbsp;&nbsp;&nbsp;`// points. We must buffer the bodies that should be
destroyed`<br/>
&nbsp;&nbsp;&nbsp;`// because they may belong to multiple contact points.`<br/>
&nbsp;&nbsp;&nbsp;`const int32 k_maxNuke = 6;`<br/>
&nbsp;&nbsp;&nbsp;`b2Body* nuke[k_maxNuke];`<br/>
&nbsp;&nbsp;&nbsp;`int32 nukeCount = 0;`<br/>
&nbsp;&nbsp;&nbsp;`// Traverse the contact buffer. Destroy bodies that`<br/>
&nbsp;&nbsp;&nbsp;`// are touching heavier bodies.`<br/>
&nbsp;&nbsp;&nbsp;`for (int32 i = 0; i < m_pointCount; ++i)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`ContactPoint* point = m_points + i;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Body* bodyA =
point->fixtureA->GetBody();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Body* bodyB =
point->FixtureB->GetBody();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`float32 massA = bodyA->GetMass();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`float32 massB = bodyB->GetMass();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (massA > 0.0f && massB > 0.0f)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (massB > massA)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`nuke[nukeCount++] = bodyA;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`else`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`nuke[nukeCount++] = bodyB;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (nukeCount == k_maxNuke)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`break;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;`// Sort the nuke array to group duplicates.`<br/>
&nbsp;&nbsp;&nbsp;`std::sort(nuke, nuke + nukeCount);`<br/>
&nbsp;&nbsp;&nbsp;`// Destroy the bodies, skipping duplicates.`<br/>
&nbsp;&nbsp;&nbsp;`int32 i = 0;`<br/>
&nbsp;&nbsp;&nbsp;`while (i < nukeCount)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`b2Body* b = nuke[i++];`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`while (i < nukeCount && nuke[i] ==
b)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`++i;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`m_world->DestroyBody(b);`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

## Contact Filtering

Often in a game you don't want all objects to collide. For example, you may
want to create a door that only certain characters can pass through. This is
called contact filtering, because some interactions are filtered out.

LiquidFun allows you to achieve custom contact filtering by implementing a
b2ContactFilter class. This class requires you to implement a ShouldCollide
function that receives two b2Shape pointers. Your function returns true if the
shapes should collide.

The default implementation of ShouldCollide uses the b2FilterData defined in
Chapter 6, Fixtures.

&nbsp;&nbsp;&nbsp;`bool b2ContactFilter::ShouldCollide(b2Fixture* fixtureA,
b2Fixture* fixtureB)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`const b2Filter& filterA =
fixtureA->GetFilterData();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`const b2Filter& filterB =
fixtureB->GetFilterData();`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`if (filterA.groupIndex ==
filterB.groupIndex && filterA.groupIndex != 0)`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`return
filterA.groupIndex > 0;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`}`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`return collide;`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

At run-time you can create an instance of your contact filter and register it
with b2World::SetContactFilter. Make sure your filter stays in scope while the
world exists.

&nbsp;&nbsp;&nbsp;`MyContactFilter filter;`<br/>
&nbsp;&nbsp;&nbsp;`world->SetContactFilter(&filter);`<br/>
&nbsp;&nbsp;&nbsp;`// filter remains in scope …`<br/>


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
