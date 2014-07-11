# Particle Module

[About](#About)<br/>
[Particles](#Particles)<br/>
[Particle Systems](#ps)<br/>
[Particle Groups](#pg)<br/>
[Discrete Particles vs. Particle Groups](#dp)<br/>
[Stepping the World](#stw)<br/>
[Maximum Velocity](#mv)<br/>
[Creating and Destroying Particles](#cdp)<br/>
[Creating and Destroying Particle Groups](#cdpg)<br/>
[Particle Behaviors](#pb)<br/>
[Particle Properties](#pp)<br/>
[Rendering with OpenGL](#gl)<br/>
[Sample Applications](#sa)<br/>

<a name="About">
## About
The Particle module offers the ability to create and manipulate liquid or soft
(deformable) bodies. It allows you to create (and destroy) particles with
various behaviors and properties, and provides various methods for
manipulating
them. The module permits you to define particles discretely or as groups. It
is
designed to allow you to manipulate large numbers of particles efficiently.

<a name="Particles">
## Particles
A particle is round, and the minimal unit of matter in a particle system. By
default, a particle behaves as a liquid. You can set behavioral flags,
however, to assign different behaviors (explained in
[Particle Behaviors](#pb)) to individual particles or groups of particles.
You can also set other particle properties including position, velocity, and
color.<br/>
The `b2Particle.h` file contains the enumerated behavior values, as well as
the
variables specifying other particle properties. The corresponding enum is
named `b2ParticleFlag.`

<a name="ps">
## Particle Systems

The "world" that particles inhabit is called a particle system. A particle
system describes a wide variety of physical coefficients that help dictate
how particles interact with the world around them. A few examples of these
conditions are default particle radius, elasticity, and viscosity. For more
detail, see the API Reference description of the b2ParticleSystemDef struct.

The following example creates a particle system:

&nbsp;&nbsp;&nbsp;`const b2ParticleSystemDef particleSystemDef;`<br/>
&nbsp;&nbsp;&nbsp;`m_particleSystems[0] =
`m_world->CreateParticleSystem(&particleSystemDef);`<br/>

You can also create more than one particle system: Thus, one "world's"
particles may have a certain default radius, elasticity, etc., while the other
"world" has different default values for these properties. The following sample
shows the creation of multiple particle systems:

&nbsp;&nbsp;&nbsp;`const b2ParticleSystemDef particleSystemDef;`<br/>
&nbsp;&nbsp;&nbsp;`for (int i = 0; i < NUM_PARTICLE_SYSTEMS; ++i) {`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
`m_particleSystems[i] = m_world->CreateParticleSystem(&particleSystemDef);`
<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

In many, if not most,
cases, it will not be necessary to adjust the default values or create multiple
particle systems. You may find it useful in some cases, however.

For example, dividing particles into multiple systems can yield a performance
gain by allowing you to simulate only the visible systems while putting all
other systems in a "paused" state using `b2ParticleSystem::SetPaused()`.

The "Multiple Systems" example in the Testbed provides an example of
two particle systems influencing a rigid body while not interacting with each
other.

<a name="pg">
## Particle Groups

Instead of creating particles individually, you can create a group of
particles to manipulate en masse. Some of the particle-group properties that
you can set are the same as those for discrete particles: behavior, position,
linear velocity, and color. There are also properties specific to groups:
rotational angle, rotational velocity, and strength.<br/>
The `b2ParticleGroup.h` file contains the declarations for all of these
variables, as well as the enum for particle-group behavior:
`b2ParticleGroupFlag`.

<a name="dp">
## Discrete Particles vs. Particle Groups

With one main exception, there is no functional difference between working
with individual particles and groups of particles. The exception is rigid
particles: Because of the internal algorithm used to make particles rigid, you
must define them as a group.

Particle groups do offer several conveniences. First, they allow you to create
and destroy large numbers of particles automatically. If you do not create a
group, you must create all of the particles individually. Also, a group allows
you to assign the same property, such as angle of rotation, to all of its
particles at once.

<a name="stw">
## Stepping the World (Particle Iterations)

The particle solver can iterate multiple times per step. Larger numbers of
steps improve the stability and fidelity of the particle simulation. However,
more steps also require more processor cycles.

The cycles cost is almost linear: double the number of particle iterations
will almost double the cycles cost of b2ParticleSystem::Solve.

Use the `particleIterations` parameter in `b2World::Step` to set the number
of iterations. The default value of `particleIterations` is 1.

You should experiment with `particleIterations` in your game to find the best
balance of stability versus cycles. Try calling `b2CalculateParticleIterations`
or `b2World::CalculateReasonableParticleIterations` to estimate a reasonable
value. Note that these functions are, necessarily, a simplification, and
should be used only as a starting point.

If your simulation seems overly bouncy or energetic, or if the particles in
your simulation are passing through contacts, try increasing the number of
particle iterations.

Note that, as particle iterations increases, the affect of pressure on
highly-compressed particles also increases. That is, particles get more
incompressible as you increase particle iterations.

<a name="mv">
## Maximum Velocity

The particle simulation enforces a maximum velocity on the particles, for
stability and to prevent excessive interpenetration. The maximum velocity is,

&nbsp;&nbsp;&nbsp;`particle diameter / (particle iterations *
b2World::Step's dt)`<br/>

<a name="cdp">
## Creating and Destroying Particles

To create individual particles, create a `b2ParticleDef`-struct object. Next,
specify the behavior and properties of the particle. Finally, call the method
to create the particle.<br/>
The following example creates an individual particle.

&nbsp;&nbsp;&nbsp;`b2ParticleDef pd;`<br>
&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle;`<br/>
&nbsp;&nbsp;&nbsp;`pd.color.Set(0, 0, 255, 255);`<br/>
&nbsp;&nbsp;&nbsp;`pd.position.Set(i, 0);`<br/>
&nbsp;&nbsp;&nbsp;`int tempIndex = m_particleSystem->CreateParticle(pd);`<br/>

Particle lists are self-compacting. Therefore, the index returned by
CreateParticle is only valid until a lower-indexed particle, or a group
referencing a lower-indexed particle, is deleted.<br/>
To destroy an individual particle, invoke the function

&nbsp;&nbsp;&nbsp;`void DestroyParticle(int32 index);`

The following example destroys the particle created above.

&nbsp;&nbsp;&nbsp;`m_particleSystem->DestroyParticle(tempIndex);`<br/>

### Particle lifetimes

In addition to manual destruction of particles as described above, particles
can also expire and be destroyed due to age.

The following example tells the system to track particle ages for the purpose
of destroying them.

&nbsp;&nbsp;&nbsp;`m_particleSystem->SetParticleDestructionByAge(true);`

A particle can die one of two "age-related" deaths. First, you can set a
lifetime for a particle--a period of time after which it expires.  The following
example does this:

&nbsp;&nbsp;&nbsp;`m_particleSystem->SetParticleLifetime(`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`index, Random() *`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
`(k_particleLifetimeMax - k_particleLifetimeMin) +`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`k_particleLifetimeMin);`<br/>

where `index` specifies the number of the particle whose lifetime is being
assigned, and the `Random()` function generates a random value for that
lifetime.

You do not need to set a specific lifetime for a particle for it to have an
age-related death. If you set a maximum number of particles that can exist in a
particle system, and you have have told the system to track particle ages, the
system clamps particle count by culling "excess" particles. Particle culling
takes place in age order, with the oldest ones destroyed first.

The following example sets a maximum particle count for a particle system.

&nbsp;&nbsp;&nbsp;`m_particleSystem->SetMaxParticleCount(k_maxParticleCount);`

The Faucet example in the Testbed provides an example of both types of
lifetime-driven particle destruction.

### Stuck Particles

Particles may get stuck and become obstructions that need to be destroyed or
relocated. A particle is identified as possibly stuck if it remains in contact
with two or more surfaces for a user-specified number (threshold) of particle
iterations. Once "candidates" are identified, you can implement your own logic
to decide whether they are actually stuck, and how to deal with them.

The ability to implement your own logic gives you flexibility in deciding
when you want to consider a particle stuck. For instance, a ball may
be traveling down a chute, making contact with walls on multiple sides. This
state satisfies the "possibly stuck" condition described in the previous
paragraph. But you could implement logic judging the ball not stuck as long
as it keeps traveling down the chute.

On the other hand, you could also decide that not only an immobile particle,
but even a mobile one trapped in a certain spatial range, is stuck. The system
relies on you to judge the candidates.

The following example shows one possible implementation for such a case.

<pre>
  // This code example of app logic deciding whether or not to eliminate stuck
  // particles shows a user who set up a global array of sensor fixtures
  // covering areas they know to be "problematic" for stuck particles in
  // their geometry, and then at each step testing any stuck particles against
  // those sensors, eliminating any stuck particles that lie inside a known
  // problem region.
  void DestroyStuckParticlesInSensors(
      const b2Fixture * const *sensors, int32 num)
  {
  	const int32 stuck = gParticleSystem->GetStuckCandidateCount();
  	if (stuck > 0)
  	{
  		const int32 *candidates = gParticleSystem->GetStuckCandidates();
  		const b2Vec2 *positions = gParticleSystem->GetPositionBuffer();
  		for (int32 i = 0; i < stuck; ++i)
  		{
  			const int32 particle = candidates[i];
  			const b2Vec2 &position = positions[particle];
  			for (int32 j = 0; j < num; ++j)
  			{
  				if(sensors[j]->TestPoint(position))
  				{
  					gParticleSystem->DestroyParticle(particle);
  				}
  			}
  		}
  	}
  }
  // particles in multiple contacts for 5 or more iterations are
  // candidates
  gParticleSystem->SetStuckThreshold(5);

  // step the world (assuming the timestep, velocity iterations,
  // and position iterations have been set globally).
  gWorld->Step(gTimeStep, gVelocityIterations, gPositionIterations);

  // Perform the above check for stuck particles against sensors
  // in this global array.
  DestroyStuckParticlesInSensors(gProblemAreaSensors, gNumSensors);
</pre>

<a name="cdpg">
## Creating and Destroying Particle Groups

A particle group begins life in a shaped container. You must therefore start a
particle group definition by specifying a shape. Next, create a
b2ParticleGroupDef-struct object. Then, specify the behavior and properties of
the particles themselves. Finally, call the method to create a particle
group.<br/>
The following example creates five differently colored, box-shaped groups of
particles.

&nbsp;&nbsp;&nbsp;`b2ParticleGroupDef pd;`<br/>
&nbsp;&nbsp;&nbsp;`b2PolygonShape shape;`<br/>
&nbsp;&nbsp;&nbsp;`shape.SetAsBox(10, 5);`<br/>
&nbsp;&nbsp;&nbsp;`pd.shape = &shape;`<br/>
&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle;`<br/>
&nbsp;&nbsp;&nbsp;`pd.angle = -0.5f;`<br/>
&nbsp;&nbsp;&nbsp;`pd.angularVelocity = 2.0f;`<br/>
&nbsp;&nbsp;&nbsp;`for (int32 i = 0; i < 5; i++)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`pd.position.Set(10 + 20 * i, 40);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`pd.color.Set(i * 255 / 5, 255 - i * 255 /
5, 128, 255);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
`m_particleSystem->CreateParticleGroup(pd);`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

To destroy a particles in a group, invoke the function

&nbsp;&nbsp;&nbsp;`DestroyParticles(bool callDestructionListener);`<br/>

Groups are automatically destroyed when they contain no particles if the
`b2_particleGroupCanBeEmpty` is not set in the group's flags.

The following example destroys all particle groups in the particle system.

<pre>
  b2ParticleGroup* group = m_particleSystem->GetParticleGroupList();
  while (group)
  {
      m_particleSystem->SetGroupFlags(
          m_particleSystem->GetGroupFlags() & ~b2_particleGroupCanBeEmpty);
      group->DestroyParticles(false);
      // The destruction of particle groups are deferred to the next call of
      // Step() so it's safe to reference the group here.
      group = group->GetNext();
  }
</pre>

The next several sections provide more information on how to define particle
behaviors and properties.

<a name="pb">
## Particle Behaviors

Particle behaviors are defined either for entire groups of, or individual,
particles.

For a group of particles, use the `b2ParticleGroupFlag` enum, which provides
two types of particle groups:

###Solid

A solid particle group prevents other bodies from lodging inside of it. Should
anything penetrate it, the solid particle group pushes the offending body back
out to its surface.

A solid particle group also possesses an especially strong repulsive force. It
is useful, for example, in a case where:

* Something should be expected to bounce with unusual vigor
** As when a racquetball strikes the wall of a court

Use the `b2_SolidParticleGroup` flag of the `b2ParticleGroupFlag` enum to
specify a solid particle group. For example:

&nbsp;&nbsp;&nbsp;`pd.groupFlags = b2_solidParticleGroup;`

###Rigid

Rigid particle groups are ones whose shape does not change, even when they
collide
with other bodies. Working with rigid particle groups confers a few advantages
over simply
working with rigid bodies: With a rigid particle group, you can:<br/>

* Delete part of the group (i.e., some of its particles).
    * For example, firing a bullet that leaves a hole in a box-shaped group of
particles.
* Merge it with other groups.
    * For example, creating a snowman from three round particle groups, and
then merging them into a single particle group.

Use the `b2_rigidParticleGroup` flag of the `b2ParticleGroupFlag` enum to
specify a rigid particle group.  For example:

&nbsp;&nbsp;&nbsp;`pd.groupFlags = b2_rigidParticleGroup;`

For individual particles, use the b2ParticleFlag enum. The b2ParticleFlag enum
provides the flags described in the following sections.  Note that different
particle behaviors may exact different performance costs.

### Elastic

Elastic particles deform and may also bounce when they collide with rigid
bodies.<br/>
Set particle behavior as elastic using the statement

&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle;`

The green circle and the blue box in the "Elastic Particles" demo of the
Testbed application comprise elastic particles.

### Color-mixing

Color-mixing particles take on some of the color of other particles with which
they collide. If only one of the two colliding particles is a color-mixing
one,
the other particle retains its pre-collision color.<br/>
<br/>
The following example shows how color mixture is calculated. It shows the
collision of two color-mixing particles: one red ("R") and one green ("G").

1. First, the system calculates deltaColor, which is the value by which each
   color will change.

	deltaColor = colorMixingStrength * (B's color - A's color).<br/>
	= 0.5 * ((0,255,0,255) - (255,0,0,255))<br/>
	= 0.5 * (-255,255,0,0)<br/>
	= (-127.5,127.5,0,0)

2. Then, it applies the delta to each particle

	R's color += deltaColor<br/>
	G's color -= deltaColor

3. As a result, both particles are now yellow:

	A's color = (127.5,127.5,0,255)<br/>
	B's color = (127.5,127.5,0,255)<br/>
<br/>
Note that when one of the operations in step 2 results in a negative number,
the
system uses the absolute value of that number. When it results in a value over
255, it rolls over from zero.<br/>
Set particle behavior as color-mixing using the statement<br/>
&nbsp;&nbsp;&nbsp;`pd.flags = b2_colorMixingParticle;`

The "Surface Tension" demo of the Testbed application uses color-mixing
particles.

### Powder

Powder particles produce a scattering effect such as you might see with sand
or
dust.<br/>
Set particle behavior as powder using the statement<br/>

&nbsp;&nbsp;&nbsp;`pd.flags = b2_powderParticle;`

The "Sparky" demo of the Testbed application uses powder particles.

### Spring

Spring particles produce the effect of being attached to one another, as by a
spring. Particles are "connected" in pairs. Each particle is connected to the
one that was closest to it at time of creation. Once paired, particles do not
change "partners." The farther an external force pulls them from one another,
the greater the power with which they collide when that external force is
removed. No matter how far particles get from one another, the connection
between them does not "snap."<br/>
Set spring behavior using the statement<br/>

&nbsp;&nbsp;&nbsp;`pd.flags = b2_springParticle;`

The red circle in the "Elastic Particles" demo of the Testbed application
comprises spring particles.

### Tensile

Tensile particles are used to produce the effect of surface tension, or the
taut
curvature on the surface of a body of liquid. They might be used, for example,
to create the surface tension you would see on a drop of water.<br/>
Once the tension is broken, the particles bounce as if they were elastic, but
also continue to attract each other. As a result, particles tend to form
clusters as they bounce.<br/>
Set tensile behavior using the statement

&nbsp;&nbsp;&nbsp;`pd.flags = b2_tensileParticle;`

The "Surface Tension" demo of the Testbed application uses tensile particles.

### Viscous

Viscous particles exhibit clinginess or stickiness, like oil.<br/>
Set viscous behavior using the statement

&nbsp;&nbsp;&nbsp;`pd.flags = b2_viscousParticle;`

The "Liquid Timer" demo of the Testbed application uses viscous particles.

### Static Pressure

Particles are subject to compression when pressure acts upon them. For example,
when particles pour into a container, the ones at the bottom of the container
are "crushed" under the weight of those above them and packed more tightly
together than the ones at the top of the pile.

The static-pressure particle eliminates this differential; the same amount of
pressure acts upon each particle in the group.

The following example sets static-pressure behavior.

&nbsp;&nbsp;&nbsp;`pd.flags = b2_staticPressureParticle;`

### Wall

Wall particles are static. They are permanently stationary, even if something
collides with them. <br/>
Set wall behavior using the statement

&nbsp;&nbsp;&nbsp;`pd.flags = b2_wallParticle;`

### Barrier

Solid or rigid particle groups are not inherently tunneling-proof. Particles
traveling at high enough velocities may penetrate them. Barrier particles,
used in conjunction with other particle types, provide particle groups
with protection against tunneling. This functionality is useful when, for
example, you want to ensure that liquid particles will not leak out of a
container formed of wall particles.

Barrier particles only prevent penetration of the particle groups they inhabit.
They cannot prevent particles from getting between groups of particles, even if
the groups' positions make them look as if they are contiguous.

You can use barrier particles with elastic, spring, or wall particles.

The following example creates an impermeable group of wall particles:

&nbsp;&nbsp;&nbsp;`pd.flags = b2_wallParticle | b_barrierParticle;`
&nbsp;&nbsp;&nbsp;`pd.groupFlags = b2_solidParticleGroup;`

### Zombie

Zombie particles are useful when you want efficiently to destroy multiple
particles in a single step. All of the particles that you designate as zombies
are destroyed at the same time, in a single iteration of the solver.
Destroying
particles in a batch, after designating them as zombies, yields better
performance than destroying them one by one: Whereas destroying particles
one-by-one takes (number of parti`cles) * (time per particle) to complete,
destroying them all in a batch takes the same time as it would to destroy a
single particle.<br/>
In the following example, every other particle in a group is designated as a
zombie, and will be destroyed in the next step of the solver. (For more
information on the LiquidFun solver, see Chapter 1. Introduction.)

&nbsp;&nbsp;&nbsp;`b2ParticleGroup*group=
m_particleSystem->CreateParticleGroup(pd);`<br/>
&nbsp;&nbsp;&nbsp;`for (int32 i=0;i<group->GetParticleCount();i+=2)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`group->GetFlagsBuffer()[i] |=`
`b2_zombieParticle;`<br/>
&nbsp;&nbsp;&nbsp;`}`

Note that you can assign multiple behaviors to a group or particle. Use
the | ("bitwise OR") operator to chain behavior flags. For example, for a group:

&nbsp;&nbsp;&nbsp;`pd.groupFlags = b2_solidParticleGroup |
b2_rigidParticleGroup;`

And for particles:

&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle | b2_viscousParticle;`

To define a group combining a specific group behavior with a specific particle
behavior, use two statements. For example:

&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle;`<br/>
&nbsp;&nbsp;&nbsp;`pd.groupFlags = b2_solidParticleGroup;`<br/>

<a name="pp">
## Particle Properties

### Color

Set particle or particle-group color using the statement

&nbsp;&nbsp;&nbsp;`pd.color.Set(r, g, b, a);`

whose parameters set red, green, blue, and opacity, respectively. Each
parameter takes a value of 0-255.

### Size

There are two points to keep in mind when using small particles. First, in the
case of particle groups, particle size can affect performance. This is
because particle size is inversely proportional to the number of particles
generated to constitute a group. Having a large number of particles, in turn,
can diminish performance.

Set particle size using the statement

&nbsp;&nbsp;&nbsp;`m_particleSystem->SetRadius(r);`

where `r` is a float32 value greater than 0.0f. Default particle radius is
1.0f.

Small particles may also behave unpredictably (i.e., break conservation of
momentum) in scenarios such as explosions. Slowing these particles down by
reducing gravity scale can stabilize their behavior.

Set gravity scale using the statement

&nbsp;&nbsp;&nbsp;`m_particleSystem->SetGravityScale(g);`

where `g` is a `float32` value greater than 0.0f. Default gravity scale is
1.0f.

It is worth noting that adjusting the number of particle iterations per solver
step can also affect the effect of gravity on particles. Larger iteration sizes
confer greater resistance to gravity. A common reason for increasing the number
of particle-iterations is to prevent volume loss (i.e. compression) due to
gravity.

### Position

Set particle or particle-group position using the statement

&nbsp;&nbsp;&nbsp;`pd.position.Set(x, y);`

 where `x` and `y` are the world-coordinates of the translation of the
particle
group.

### Velocity

For discrete particles, set velocity using the statement

&nbsp;&nbsp;&nbsp;`pd.velocity.Set(x,y);`

where `x` is velocity along the x-axis, and `y` is velocity along the
y-axis.<br/>
For particle groups, set velocity using the statements

&nbsp;&nbsp;&nbsp;`pd.linearVelocity.Set(x,y);`<br/>
&nbsp;&nbsp;&nbsp;`pd.angularVelocity = aV;`<br/>

where `x` is the group's velocity along the x-axis, `y` is velocity along the
y-axis, and `aV` is the group's angular (i.e., rotational) velocity (expressed
as radians per second).

### Angle (Groups Only)

This property applies only to rigid particle groups. It indicates the angle at
which a group is tilted. Set angle with the statement

&nbsp;&nbsp;&nbsp;`pd.angle =checkout a;`

where `a` is the angle of tilt, expressed in radians. Left unspecified, the
value defaults to 0.

### Strength (Groups Only)

Strength describes the cohesion of a group of particles. Set strength with the
statement

&nbsp;&nbsp;&nbsp;`pd.strength = s;`<br/>

where `s` is a float32 value between 0.0 (least cohesive) and 1.0 (most
cohesive). The default value is 1.0.

<a name="gl">
## Rendering with OpenGL

The Particle module provides particularly efficient rendering via OpenGL.

Each type of particle property lives in a contiguous memory buffer. For
example,
all particles' position data live next door to one another, all color data
live
next door to one another, and so forth. Table 1 provides a visual
representation
of this storage.

**_Table 1. Memory Map of Particle Buffers_**

<table>
<tr>
<td></td>
<td>Particle 1</td>
<td>Particle 2</td>
<td>Particle 3</td>
</tr>
<tr>
<td>Position</td>
<td>x1,y1</td>
<td>x2,y2</td>
<td>x3,y3</td>
</tr>
<tr>
<td>Address</td>
<td>0x00001000</td>
<td>0x00001008</td>
<td>0x00001010</td>
</tr>
<tr>
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
<tr>
<td>Color</td>
<td>r1,g1,b1,a1</td>
<td>r2,g2,b2,a2</td>
<td>r3,g3,b3,a3</td>
</tr>
<tr>
<td>Address</td>
<td>0x00002000</td>
<td>0x00002004</td>
<td>0x00002008</td>
</tr>
</table>

OpenGL can use these buffers directly in rendering.<br/>
In this example, OpenGL 1.1 would use glVertexPointer and glColorPointer to
get
the values from memory. OpenGL 2.0 would use glVertexAttribPointer.<br/>
OpenGL can be used to render either individual particles or particle groups.

<a name="sa">
## Sample Applications

Among the samples included in the LiquidFun distribution are two applications 
that offer a quick look into the capabilities of the library.

Testbed includes a large number of demos that provide examples of different 
types of particle behavior. While some of the demos are "look only," others are 
interactive, allowing you to use your mouse or touchscreen to affect the 
behavior on screen. 

Experimenting with each of the demos, and comparing their behavior against the 
source code, can provide useful insights into how different particles behave 
under various conditions. Testbed builds and runs on Android, MacOSX, Linux, 
and Windows.

EyeCandy is an Android-only application and is twofold in purpose: It provides
a simple Android example of how to use LiquidFun; and, it seeks to inspire
developers with its demonstration of the powerful liquid shaders it brings to
mobile hardware.
 
When running the program, you can slosh the fluid around by changing the
orientation of the Android device. You can also toggle bewteen shaders by
tapping the screen.


*This content is licensed under
[Creative Commons Attribution 4.0](http://creativecommons.org/licenses/by/4.0/legalcode).
For details and restrictions, please see the
[Content License](md__content_license.html).*
