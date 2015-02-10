#include <Box2D/Box2D.h>
#include <stdio.h>
#include <emscripten.h>

extern "C" {
  extern void b2WorldBeginContactBody(void* contactPtr);
  extern void b2WorldEndContactBody(void* contactPtr);
  extern void b2WorldPreSolve(void* contactPtr, void* oldManifoldPtr);
  extern void b2WorldPostSolve(void* contactPtr, void* impulsePtr);
  extern bool b2WorldQueryAABB(void* fixturePtr);
  extern float32 b2WorldRayCastCallback(void* fixturePtr, double pointX, double pointY,
                                       double normalX, double normalY, double fraction);
}

// internal classes
// TODO it might be inefficient to call out of asm.js for each contact
// consider short circuiting unused callbacks in this layer, or maybe
// function ptrs.  we could also pass this data in an unfolded
// struct but this might also be inefficient
class b2WorldContactListener : public b2ContactListener
  {
    void BeginContact(b2Contact* contact) {
      b2WorldBeginContactBody(contact);
    }

    void EndContact(b2Contact* contact) {
      b2WorldEndContactBody(contact);
    }

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
      b2WorldPreSolve(contact, const_cast<b2Manifold*>(oldManifold));
    }

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
      b2WorldPostSolve(contact, const_cast<b2ContactImpulse*>(impulse));
    }
  };

b2WorldContactListener listener;

class QueryAABBCallback : public b2QueryCallback {
public:
  bool ReportFixture(b2Fixture* fixture) {
    return b2WorldQueryAABB((void*)fixture);
  }
};

QueryAABBCallback queryAABBCallback;

class RayCastCallback : public b2RayCastCallback {
  float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                    const b2Vec2& normal, float32 fraction) {
    return b2WorldRayCastCallback(fixture, point.x, point.y,
                                  normal.x, normal.y, fraction);
  }
};

RayCastCallback rayCastCallback;

// b2World Exports
void* b2World_Create(double x, double y) {
  return new b2World(b2Vec2(x, y));
}

void* b2World_CreateBody(
    void* world, double active, double allowSleep,
    double angle, double angularVelocity, double angularDamping,
    double awake, double bullet, double fixedRotation,
    double gravityScale, double linearDamping, double linearVelocityX,
    double linearVelocityY, double positionX, double positionY,
    double type, void* userData) {
  b2BodyDef def;
  def.active = active;
  def.allowSleep = allowSleep;
  def.angle = angle;
  def.angularVelocity = angularVelocity;
  def.angularDamping = angularDamping;
  def.awake = awake;
  def.bullet = bullet;
  def.fixedRotation = fixedRotation;
  def.gravityScale = gravityScale;
  def.linearDamping = linearDamping;
  def.linearVelocity.Set(linearVelocityX, linearVelocityY);
  def.position.Set(positionX, positionY);
  def.type = (b2BodyType)type;
  def.userData = userData;
  return ((b2World*)world)->CreateBody(&def);
}

void* b2World_CreateParticleSystem(
    void* world, double colorMixingStrength, double dampingStrength,
    double destroyByAge, double ejectionStrength, double elasticStrength,
    double lifetimeGranularity, double powderStrength, double pressureStrength,
    double radius, double repulsiveStrength, double springStrength,
    double staticPressureIterations, double staticPressureRelaxation, double staticPressureStrength,
    double surfaceTensionNormalStrength, double surfaceTensionPressureStrength, double viscousStrength) {
  b2ParticleSystemDef def;
  def.colorMixingStrength = colorMixingStrength;
  def.dampingStrength = dampingStrength;
  def.destroyByAge = destroyByAge;
  def.ejectionStrength = ejectionStrength;
  def.elasticStrength = elasticStrength;
  def.lifetimeGranularity = lifetimeGranularity;
  def.powderStrength = powderStrength;
  def.pressureStrength = pressureStrength;
  def.radius = radius;
  def.repulsiveStrength = repulsiveStrength;
  def.springStrength = springStrength;
  def.staticPressureIterations = staticPressureIterations;
  def.staticPressureRelaxation = staticPressureRelaxation;
  def.staticPressureStrength = staticPressureStrength;
  def.surfaceTensionNormalStrength = surfaceTensionNormalStrength;
  def.surfaceTensionPressureStrength = surfaceTensionPressureStrength;
  def.viscousStrength = viscousStrength;

  return ((b2World*)world)->CreateParticleSystem(&def);
}

void b2World_Delete(void* world) {
  delete (b2World*)world;
}

void b2World_DestroyBody(void* world, void* body) {
  ((b2World*)world)->DestroyBody((b2Body*)body);
}

void b2World_DestroyJoint(void* world, void* joint) {
  ((b2World*)world)->DestroyJoint((b2Joint*)joint);
}

void b2World_DestroyParticleSystem(void* world, void* particleSystem) {
  ((b2World*)world)->DestroyParticleSystem((b2ParticleSystem*)particleSystem);
}

void b2World_QueryAABB(void* world, double aabbLowerBoundX, double aabbLowerBoundY,
                       double aabbUpperBoundX, double aabbUpperBoundY) {
  b2AABB aabb;
  aabb.lowerBound = b2Vec2(aabbLowerBoundX, aabbLowerBoundY);
  aabb.upperBound = b2Vec2(aabbUpperBoundX, aabbUpperBoundY);
  ((b2World*)world)->QueryAABB(&queryAABBCallback, aabb);
}

void b2World_RayCast(void* world, double point1X, double point1Y,
                     double point2X, double point2Y) {
  ((b2World*)world)->RayCast(&rayCastCallback, b2Vec2(point1X, point1Y),
                             b2Vec2(point2X, point2Y));
}

void b2World_SetContactListener(void* world) {
  ((b2World*)world)->SetContactListener(&listener);
}

void b2World_SetGravity(void* world, double x, double y) {
  ((b2World*)world)->SetGravity(b2Vec2(x, y));
}

void b2World_Step(void* world, float step, float vIterations, float pIterations) {
  ((b2World*)world)->Step(step, (int32)vIterations, (int32)pIterations, 3);
}
