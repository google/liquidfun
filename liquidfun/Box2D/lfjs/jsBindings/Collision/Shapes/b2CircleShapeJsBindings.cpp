#include <Box2D/Box2D.h>

// create fixture from circle
void* b2CircleShape_CreateFixture(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // circle
    double px, double py,
    double radius) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  b2CircleShape circle;
  circle.m_p.Set(px, py);
  circle.m_radius = radius;

  def.shape = &circle;
  return ((b2Body*)body)->CreateFixture(&def);
}

// Create particle system from circle
void* b2CircleShape_CreateParticleGroup(
    void* particleSystem,
    // ParticleGroupDef
    double angle, double angularVelocity, double colorR,
    double colorG, double colorB, double colorA, double flags, double group,
    double groupFlags, double lifetime, double linearVelocityX, double linearVelocityY,
    double positionX, double positionY, double positionData, double particleCount,
    double strength, double stride, double userData,
    // Circle
    double px, double py,
    double radius) {
  b2ParticleGroupDef def;
  def.angle = angle;
  def.angularVelocity = angularVelocity;
  def.color = b2ParticleColor(colorR, colorG, colorB, colorA);
  def.flags = flags;
  def.group = NULL;
  def.groupFlags = groupFlags;
  def.lifetime = lifetime;
  def.linearVelocity = b2Vec2(linearVelocityX, linearVelocityY);
  def.position = b2Vec2(positionX, positionY);
  def.positionData = NULL;
  def.particleCount = particleCount;
  def.shapeCount = 0;
  def.shapes = NULL;
  def.strength = strength;
  def.stride = stride;
  def.userData = (double*)&userData;

  b2CircleShape c;
  c.m_p = b2Vec2(px, py);
  c.m_radius = radius;

  def.shape = &c;

  return ((b2ParticleSystem*)particleSystem)->CreateParticleGroup(def);
}

double b2CircleShape_DestroyParticlesInShape(
    void* particleSystem,
    // circle
    double px, double py, double radius,
    // xf
    double xfpX, double xfpY, double xfqS,
    double xfqC) {
  b2CircleShape circle;
  circle.m_p.Set(px, py);
  circle.m_radius = radius;

  b2Transform xf;
  xf.p.Set(xfpX, xfpY);
  xf.q.s = xfqS;
  xf.q.c = xfqC;
  return ((b2ParticleSystem*)particleSystem)->DestroyParticlesInShape(circle, xf);
}
