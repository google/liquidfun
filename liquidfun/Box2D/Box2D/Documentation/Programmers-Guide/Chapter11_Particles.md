# Particle Module

[About](#About)<br/>
[Particles](#Particles)<br/>
[Particle Groups](#pg)<br/>
[Discrete Particles vs. Particle Groups](#dp)<br/>
[Creating and Destroying Particles](#cdp)<br/>
[Creating and Destroying Particle Groups](#cdpg)<br/>
[Particle Behaviors](#pb)<br/>
[Particle Properties](#pp)<br/>
[Rendering with OpenGL](#gl)<br/>

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
named
`b2ParticleFlag.`

<a name="pg">
## Particle Groups

Instead of creating particles individually, you can create a group of
particles to manipulate en masse. Some of the particle-group properties that
you
can set are the same as those for discrete particles: behavior, position,
linear
velocity, and color. There are also properties specific to groups: rotational
angle, rotational velocity, and strength.<br/>
The `b2ParticleGroup.h` file contains the declarations for all of these
variables, as well as the enum for particle-group behavior:
`b2ParticleGroupFlag`.

<a name="dp">
## Discrete Particles vs. Particle Groups

With one main exception, there is no functional difference between working
with
individual particles and groups of particles. The exception is rigid
particles:
Because of the internal algorithm used to make particles rigid, you must
define
them as a group.

Particle groups do offer several conveniences. First, they allow you to create
and destroy large numbers of particles automatically. If you do not create a
group, you must create all of the particles individually. Also, a group allows
you to assign the same property, such as angle of rotation, to all of its
particles at once.

<a name="cdp">
## Creating and Destroying Particles

To create individual particles, create a `b2ParticleDef`-struct object. Next,
specify the behavior and properties of the particle. Finally, call the method
to
create the particle.<br/>
The following example creates an individual particle.

&nbsp;&nbsp;&nbsp;`b2ParticleDef pd;`<br>
&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle;`<br/>
&nbsp;&nbsp;&nbsp;`pd.color.Set(0, 0, 255, 255);`<br/>
&nbsp;&nbsp;&nbsp;`pd.position.Set(i, 0);`<br/>
&nbsp;&nbsp;&nbsp;`int tempIndex = m_world->CreateParticle(pd);`<br/>

Particle lists are self-compacting. Therefore, the index returned by
CreateParticle is only valid until a lower-indexed particle, or a group
referencing a lower-indexed particle, is deleted.<br/>
To destroy an individual particle, invoke the function

&nbsp;&nbsp;&nbsp;`void DestroyParticle(int32 index);`

The following example destroys the particle created above.

&nbsp;&nbsp;&nbsp;`m_world->DestroyParticle(tempIndex);`<br/>


<a name="cdpg">
## Creating and Destroying Particle Groups

A particle group begins life in a shaped container. You must therefore start a
particle group definition by specifying a shape. Next, create a
b2ParticleGroupDef-struct object. Then, specify the behavior and properties of
the particle. Finally, call the method to create a particle group.<br/>
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
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`world->CreateParticleGroup(pd);`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`m_world->CreateParticleGroup(pd);`<br/>
&nbsp;&nbsp;&nbsp;`}`<br/>

To destroy a group of particles, invoke the function

&nbsp;&nbsp;&nbsp;`DestroyParticleGroup(b2ParticleGroup* group);`<br/>

The following example destroys all particle groups in the world.

&nbsp;&nbsp;&nbsp;`b2ParticleGroup* group =
m_world->GetParticleGroupList();`<br/>
&nbsp;&nbsp;&nbsp;`while (group)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`	b2ParticleGroup* nextGroup =
group->GetNext(); // access this before we destroy the group`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`	m_world->DestroyParticleGroup(group);`
<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`	group = nextGroup;`<br/>
&nbsp;&nbsp;&nbsp;`}`

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

You cannot define a particle group as only solid. It must have one of four
other behaviors, as well: it must either be combined with the rigid-group
behavior, or with the wall, spring, or elastic particle behavior.

To define a group combining solid- and rigid-group behaviors, use a single
statement. For example:

&nbsp;&nbsp;&nbsp;`pd.groupFlags = b2_solidParticleGroup | b2_rigidParticleGroup;`

To define a group combining solid-group behavior with a given particle
behavior, use two statements. For example:

&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle;`<br/>
&nbsp;&nbsp;&nbsp;`pd.groupFlags = b2_solidParticleGroup;`<br/>

A solid particle group possesses especially strong repulsive force. It is
useful, for example, in a case where:

* Something should be expected to bounce with unusual vigor
** As when a racquetball strikes the wall of a court

Use the `b2_SolidParticleGroup` flag of the `b2ParticleGroupFlag` enum to
specify a solid particle group. In many cases, a group will be defined as not
only solid, but with additional behaviors, as well. For example, solid and
elastic



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

### Color-mixing

Color-mixing particles take on some of the color of other particles with which
they collide. If only one of the two colliding particles is a color-mixing
one,
the other particle retains its pre-collision color.<br/>
<br/>
The following example shows how color mixture is calculated. It shows the
collision of two color-mixing particles: one red ("R") and one green ("G).

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


### Powder

Powder particles produce a scattering effect such as you might see with sand
or
dust.<br/>
Set particle behavior as powder using the statement<br/>

&nbsp;&nbsp;&nbsp;`pd.flags = b2_powderParticle;`

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

### Viscous

Viscous particles exhibit clinginess or stickiness, like oil.<br/>
Set viscous behavior using the statement

&nbsp;&nbsp;&nbsp;`pd.flags = b2_viscousParticle;`

### Wall

Wall particles are static. They are permanently stationary, even if something
collides with them. <br/>
Set wall behavior using the statement

&nbsp;&nbsp;&nbsp;`pd.flags = b2_wallParticle;`

### Zombie Particles

Zombie particles are useful when you want efficiently to destroy multiple
particles in a single step. All of the particles that you designate as zombies
are destroyed at the same time, in a single iteration of the solver.
Destroying
particles in a batch, after designating them as zombies, yields better
performance than destroying them one by one: Whereas destroying particles
one-by-one takes (number of particles) * (time per particle) to complete,
destroying them all in a batch takes the same time as it would to destroy a
single particle.<br/>
In the following example, every other particle in a group is designated as a
zombie, and will be destroyed in the next step of the solver. (For more
information on the LiquidFun solver, see Chapter 1. Introduction.)

&nbsp;&nbsp;&nbsp;`b2ParticleGroup*group=
m_world->CreateParticleGroup(pd);`<br/>
&nbsp;&nbsp;&nbsp;`for (int32 i=0;i<group->GetParticleCount();i+=2)`<br/>
&nbsp;&nbsp;&nbsp;`{`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`group->GetParticleFlagsBuffer()[i] |=`
`b2_zombieParticle;`<br/>
&nbsp;&nbsp;&nbsp;`}`

<a name="pp">

Note that you can assign multiple behaviors to a particle or group. Use
the | ("bitwise OR") operator to chain behavior flags. For example:

&nbsp;&nbsp;&nbsp;`pd.flags = b2_elasticParticle | b2_viscousParticle;`

## Particle Properties

### Color

Set particle or particle-group color using the statement

&nbsp;&nbsp;&nbsp;`pd.color.Set(r, g, b, a);`

whose parameters set red, green, blue, and opacity, respectively. Each
parameter
takes a value of 0-255.

### Size

Set particle size using the statement

&nbsp;&nbsp;&nbsp;`m_world->SetParticleRadius®;`

where `r` is a float32 value greater than 0.0f. Its default value is 1.0f.<br/>
There are two points to keep in mind when using small particles. First, in the
case of particle groups, that particle size can affect performance. This is
because the smaller the particle size, the larger the number of particles
generated to constitute a group. Having a large number of particles, in turn,
can diminish performance.<br/>

Second, small particles may behave unpredictably (i.e., break conservation of
momentum) in scenarios such as explosions. Slowing these particles down by
reducing gravity scale can stabilize their behavior.<br/>
<br/>
Note that

Set gravity scale using the statement

&nbsp;&nbsp;&nbsp;`m_world->SetParticleGravityScale(g);`

where `g` is a float32 value.

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

&nbsp;&nbsp;&nbsp;`pd.angle = a;`

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
