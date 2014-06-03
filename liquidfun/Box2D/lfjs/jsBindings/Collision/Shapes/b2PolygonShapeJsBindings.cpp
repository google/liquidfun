#include <Box2D/Box2D.h>

// b2Body createFixture from polygon shape and def
void* b2PolygonShape_CreateFixture_3(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  const int count = 3;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2)
  };

  b2PolygonShape polygon;
  polygon.Set(points, count);

  def.shape = &polygon;
  return ((b2Body*)body)->CreateFixture(&def);
}

void* b2PolygonShape_CreateFixture_4(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  const int count = 4;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2),
      b2Vec2(x3, y3)
  };

  b2PolygonShape polygon;
  polygon.Set(points, count);
  def.shape = &polygon;
  return ((b2Body*)body)->CreateFixture(&def);
}

void* b2PolygonShape_CreateFixture_5(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  const int count = 5;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2),
      b2Vec2(x3, y3),
      b2Vec2(x4, y4)
  };

  b2PolygonShape polygon;
  polygon.Set(points, count);

  def.shape = &polygon;
  return ((b2Body*)body)->CreateFixture(&def);
}

void* b2PolygonShape_CreateFixture_6(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    double x5, double y5) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  const int count = 6;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2),
      b2Vec2(x3, y3),
      b2Vec2(x4, y4),
      b2Vec2(x5, y5)
  };

  b2PolygonShape polygon;
  polygon.Set(points, count);

  def.shape = &polygon;
  return ((b2Body*)body)->CreateFixture(&def);
}

void* b2PolygonShape_CreateFixture_7(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    double x5, double y5,
    double x6, double y6) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  const int count = 7;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2),
      b2Vec2(x3, y3),
      b2Vec2(x4, y4),
      b2Vec2(x5, y5),
      b2Vec2(x6, y6)
  };

  b2PolygonShape polygon;
  polygon.Set(points, count);

  def.shape = &polygon;
  return ((b2Body*)body)->CreateFixture(&def);
}

void* b2PolygonShape_CreateFixture_8(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    double x5, double y5,
    double x6, double y6,
    double x7, double y7) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  const int count = 8;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2),
      b2Vec2(x3, y3),
      b2Vec2(x4, y4),
      b2Vec2(x5, y5),
      b2Vec2(x6, y6),
      b2Vec2(x7, y7)
  };

  b2PolygonShape polygon;
  polygon.Set(points, count);

  def.shape = &polygon;
  return ((b2Body*)body)->CreateFixture(&def);
}

// Create b2ParticleSystem from 4 sided polygon
void* b2PolygonShape_CreateParticleGroup_4(
    void* particleSystem,
    // ParticleGroupDef
    double angle, double angularVelocity, double colorR,
    double colorG, double colorB, double colorA, double flags, double group,
    double groupFlags, double lifetime, double linearVelocityX, double linearVelocityY,
    double positionX, double positionY, double positionData, double particleCount,
    double strength, double stride, double userData,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3) {
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

  const int count = 4;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2),
      b2Vec2(x3, y3)
  };

  b2PolygonShape p;
  p.Set(points, count);
  def.shape = &p;

  return ((b2ParticleSystem*)particleSystem)->CreateParticleGroup(def);
}

double b2PolygonShape_DestroyParticlesInShape_4(
    void* particleSystem,
    //polygon
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    // xf
    double xfpX, double xfpY, double xfqS,
    double xfqC)  {
  const int count = 4;
  b2Vec2 points[count] = {
      b2Vec2(x0, y0),
      b2Vec2(x1, y1),
      b2Vec2(x2, y2),
      b2Vec2(x3, y3)
  };

  b2PolygonShape p;
  p.Set(points, count);

  b2Transform xf;
  xf.p.Set(xfpX, xfpY);
  xf.q.s = xfqS;
  xf.q.c = xfqC;

  return ((b2ParticleSystem*)particleSystem)->DestroyParticlesInShape(p, xf);
}
