# Porting from Box2D

LiquidFun extends Erin Catto's popular Box2D physics engine. If your project
already uses Box2D, LiquidFun should be a simple drop-in replacement for you.

### Drop-in LiquidFun Code

To port from Box2D to LiquidFun, simply replace your Box2D directory with
liquidfun/Box2D/Box2D. Your code should compile and run as it did under Box2D,
and you will have access to LiquidFun's particle simulation API.

LiquidFun is based off of a specific version of Box2D. If your version of
Box2D is different from LiquidFun's you may have to adjust your code to
match API differences.

Please see the [Release Notes][] to find the version of Box2D that LiquidFun
is based on.

### Specify `particleIterations`

LiquidFun adds a `particleIterations` parameter to b2World::Step. If you don't
specify `particleIterations`, we calculate a default value based on the
physical constants in your b2ParticleSystems.

However, the number of `particleIterations` affects performance and stability
significantly. You should experiment with the number of iterations to find
a suitable balance for your situation.

### Start using b2ParticleSystem

`b2ParticleSystems` are created with `b2World::CreateParticleSystem`, the
same way `b2Bodys` are created with `b2World::CreateBody`.

For details, please see [Particle Module][] in the [Programmer's Guide][]


  [Release Notes]: ../../ReleaseNotes.html
  [Particle Module]: ../../Programmers-Guide/html/md__chapter11__particles.html
  [Programmer's Guide]: ../../Programmers-Guide/html/index.html

