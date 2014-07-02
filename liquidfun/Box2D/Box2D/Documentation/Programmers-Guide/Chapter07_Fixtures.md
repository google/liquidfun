# Fixtures

[About](#about)<br/>
[Fixture Creation](#fc)<br/>
[Sensors](#se)<br/>

<a name="about">
## About

Recall that shapes don’t know about bodies and may be used independently of
the physics simulation. Therefore LiquidFun provides the b2Fixture class to
attach shapes to bodies. A body may have zero or more fixtures. A body with
multiple fixtures is sometimes called a *compound body.*

Fixtures hold the following:

* a single shape

* broad-phase proxies

* density, friction, and restitution

* collision filtering flags

* back pointer to the parent body

* user data

* sensor flag

These are described in the following sections.

<a name="fc">
## Fixture Creation

Fixtures are created by initializing a fixture definition and then passing the
definition to the parent body.

&nbsp;&nbsp;&nbsp;`b2FixtureDef fixtureDef;`<br/>
&nbsp;&nbsp;&nbsp;`fixtureDef.shape = &myShape;`<br/>
&nbsp;&nbsp;&nbsp;`fixtureDef.density = 1.0f;`<br/>
&nbsp;&nbsp;&nbsp;`b2Fixture* myFixture =
myBody->CreateFixture(&fixtureDef);`<br/>

This creates the fixture and attaches it to the body. You do not need to store
the fixture pointer since the fixture will automatically be destroyed when the
parent body is destroyed. You can create multiple fixtures on a single body.

You can destroy a fixture on the parent body. You may do this to model a
breakable object. Otherwise you can just leave the fixture alone and let the
body destruction take care of destroying the attached fixtures.

&nbsp;&nbsp;&nbsp;`myBody->DestroyFixture(myFixture);`<br/>

### Density

The fixture density is used to compute the mass properties of the parent body.
The density can be zero or positive. You should generally use similar
densities for all your fixtures. This will improve stacking stability.

The mass of a body is not adjusted when you set the density. You must call
ResetMassData for this to occur.

&nbsp;&nbsp;&nbsp;`fixture->SetDensity(5.0f);`<br/>
&nbsp;&nbsp;&nbsp;`body->ResetMassData();`<br/>

### Friction

Friction is used to make objects slide along each other realistically.
LiquidFun supports static and dynamic friction, but uses the same parameter
for both. Friction is simulated accurately in LiquidFun and the friction
strength is proportional to the normal force (this is called Coulomb
friction). The friction parameter is usually set between 0 and 1, but can be
any non-negative value. A friction value of 0 turns off friction and a value
of 1 makes the friction strong. When the friction force is computed between
two shapes, LiquidFun must combine the friction parameters of the two parent
fixtures. This is done with the geometric mean:

&nbsp;&nbsp;&nbsp;`float32 friction;`<br/>
&nbsp;&nbsp;&nbsp;`friction = sqrtf(fixtureA->friction *
fixtureB->friction);`<br/>

So if one fixture has zero friction then the contact will have zero friction.

You can override the default mixed friction using b2Contact::SetFriction. This
is usually done in the b2ContactListener callback.

### Restitution

Restitution is used to make objects bounce. The restitution value is usually
set to be between 0 and 1. Consider dropping a ball on a table. A value of
zero means the ball won't bounce. This is called an inelastic collision. A
value of one means the ball's velocity will be exactly reflected. This is
called a perfectly elastic collision. Restitution is combined using the
following formula.

&nbsp;&nbsp;&nbsp;`float32 restitution;`<br/>
&nbsp;&nbsp;&nbsp;`restitution = b2Max(fixtureA->restitution,
fixtureB->restitution);`<br/>

Restitution is combined this way so that you can have a bouncy super ball
without having a bouncy floor.

You can override the default mixed restitution using
b2Contact::SetRestitution. This is usually done in the b2ContactListener
callback.

When a shape develops multiple contacts, restitution is simulated
approximately. This is because LiquidFun uses an iterative solver. LiquidFun
also uses inelastic collisions when the collision velocity is small. This is
done to prevent jitter. See b2_velocityThreshold in b2Settings.h.

### Filtering

Collision filtering allows you to prevent collision between fixtures. For
example, say you make a character that rides a bicycle. You want the bicycle
to collide with the terrain and the character to collide with the terrain, but
you don't want the character to collide with the bicycle (because they must
overlap). LiquidFun supports such collision filtering using categories and
groups.

LiquidFun supports 16 collision categories. For each fixture you can specify
which category it belongs to. You also specify what other categories this
fixture can collide with. For example, you could specify in a multiplayer game
that all players don't collide with each other and monsters don't collide with
each other, but players and monsters should collide. This is done with masking
bits. For example:

&nbsp;&nbsp;&nbsp;`playerFixtureDef.filter.categoryBits = 0x0002;`<br/>
&nbsp;&nbsp;&nbsp;`monsterFixtureDef.filter.categoryBits = 0x0004;`<br/>
&nbsp;&nbsp;&nbsp;`playerFixtureDef.filter.maskBits = 0x0004;`<br/>
&nbsp;&nbsp;&nbsp;`monsterFixtureDef.filter.maskBits = 0x0002;`<br/>

Here is the rule for a collision to occur:

&nbsp;&nbsp;&nbsp;`uint16 catA = fixtureA.filter.categoryBits;`<br/>
&nbsp;&nbsp;&nbsp;`uint16 maskA = fixtureA.filter.maskBits;`<br/>
&nbsp;&nbsp;&nbsp;`uint16 catB = fixtureB.filter.categoryBits;`<br/>
&nbsp;&nbsp;&nbsp;`uint16 maskB = fixtureB.filter.maskBits;`<br/>
&nbsp;&nbsp;&nbsp;`if ((catA & maskB) != 0 && (catB & maskA) != 0)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  `// fixtures can collide`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

Collision groups let you specify an integral group index. You can have all
fixtures with the same group index always collide (positive index) or never
collide (negative index). Group indices are usually used for things that are
somehow related, like the parts of a bicycle. In the following example,
fixture1 and fixture2 always collide, but fixture3 and fixture4 never collide.

&nbsp;&nbsp;&nbsp;`fixture1Def.filter.groupIndex = 2;`<br/>
&nbsp;&nbsp;&nbsp;`fixture2Def.filter.groupIndex = 2;`<br/>
&nbsp;&nbsp;&nbsp;`fixture3Def.filter.groupIndex = -8;`<br/>
&nbsp;&nbsp;&nbsp;`fixture4Def.filter.groupIndex = -8;`<br/>

Collisions between fixtures of different group indices are filtered according
the category and mask bits. In other words, group filtering has higher
precedence than category filtering.

Note that additional collision filtering occurs in LiquidFun. Here is a list:

* A fixture on a static body can only collide with a dynamic body.

* A fixture on a kinematic body can only collide with a dynamic body.

* Fixtures on the same body never collide with each other.

* You can optionally enable/disable collision between fixtures on bodies
connected by a joint.

Sometimes you might need to change collision filtering after a fixture has
already been created. You can get and set the b2Filter structure on an
existing fixture using b2Fixture::GetFilterData and b2Fixture::SetFilterData.
Note that changing the filter data will not add or remove contacts until the
next time step (see the World class).

<a name="se">
## Sensors

Sometimes game logic needs to know when two fixtures overlap yet there should
be no collision response. This is done by using sensors. A sensor is a fixture
that detects collision but does not produce a response.

You can flag any fixture as being a sensor. Sensors may be static, kinematic,
or dynamic. Remember that you may have multiple fixtures per body and you can
have any mix of sensors and solid fixtures. Also, sensors only form contacts
when at least one body is dynamic, so you will not get a contact for kinematic
versus kinematic, kinematic versus static, or static versus static.

Sensors do not generate contact points. There are two ways to get the state of
a sensor:

1. `b2Contact::IsTouching`

2. `b2ContactListener::BeginContact and EndContact`


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
