#include <Box2D/Box2D.h>
#include <stdio.h>
// b2Body functions
void b2Body_ApplyAngularImpulse(void* body, double impulse, double wake) {
  ((b2Body*)body)->ApplyAngularImpulse(impulse, wake);
}
void b2Body_ApplyForce(void* body, double forceX, double forceY,
                       double pointX, double pointY, double wake) {
  ((b2Body*)body)->ApplyForce(b2Vec2(forceX, forceY), b2Vec2(pointX, pointY),
                              (bool)wake);
}
void b2Body_ApplyForceToCenter(void* body, double forceX, double forceY,
                               double wake) {
  ((b2Body*)body)->ApplyForceToCenter(b2Vec2(forceX, forceY),
                              (bool)wake);
}
void b2Body_ApplyTorque(void* body, double force, double wake) {
  ((b2Body*)body)->ApplyTorque(force, (bool)wake);
}

void b2Body_DestroyFixture(void* body, void* fixture) {
  ((b2Body*)body)->DestroyFixture((b2Fixture*)fixture);
}

double b2Body_GetAngle(void* body) {
  return ((b2Body*)body)->GetAngle();
}

double b2Body_GetAngularVelocity(void* body) {
  return ((b2Body*)body)->GetAngularVelocity();
}

double b2Body_GetInertia(void* body) {
  return ((b2Body*)body)->GetInertia();
}

void b2Body_GetLinearVelocity(void* body, float* arr) {
  b2Vec2 linVelocity = ((b2Body*)body)->GetLinearVelocity();
  arr[0] = linVelocity.x;
  arr[1] = linVelocity.y;
}

void b2Body_GetLocalPoint(void* body, double pointX, double pointY, float* arr) {
  b2Vec2 localPoint = ((b2Body*)body)->GetLocalPoint(b2Vec2(pointX, pointY));
  arr[0] = localPoint.x;
  arr[1] = localPoint.y;
}

void b2Body_GetLocalVector(void* body, double vX, double vY, float* arr) {
  b2Vec2 localVector = ((b2Body*)body)->GetLocalPoint(b2Vec2(vX, vY));
  arr[0] = localVector.x;
  arr[1] = localVector.y;
}

double b2Body_GetMass(void* body) {
  return ((b2Body*)body)->GetMass();
}

void b2Body_GetPosition(void* body, float* arr) {
  b2Vec2 pos = ((b2Body*)body)->GetPosition();
  arr[0] = pos.x;
  arr[1] = pos.y;
}

void b2Body_GetTransform(void* body, float* arr) {
  b2Transform* t = const_cast<b2Transform*>(&((b2Body*)body)->GetTransform());

  arr[0] = (double)t->p.x;
  arr[1] = (double)t->p.y;
  arr[2] = (double)t->q.s;
  arr[3] = (double)t->q.c;
}

double b2Body_GetType(void* body) {
  return ((b2Body*)body)->GetType();
}

void b2Body_GetWorldCenter(void* body, float* arr) {
  b2Vec2 worldCenter = ((b2Body*)body)->GetWorldCenter();
  arr[0] = worldCenter.x;
  arr[1] = worldCenter.y;
}

void b2Body_GetWorldPoint(void* body, double pointX, double pointY, float* arr) {
  b2Vec2 worldPoint = ((b2Body*)body)->GetWorldPoint(b2Vec2(pointX, pointY));
  arr[0] = worldPoint.x;
  arr[1] = worldPoint.y;
}

void b2Body_GetWorldVector(void* body, double vX, double vY, float* arr) {
  b2Vec2 worldVec = ((b2Body*)body)->GetWorldPoint(b2Vec2(vX, vY));
  arr[0] = worldVec.x;
  arr[1] = worldVec.y;
}

void b2Body_SetAwake(void* body, double flag) {
  ((b2Body*)body)->SetAwake((bool)flag);
}

void b2Body_SetAngularVelocity(void* body, double angle) {
  ((b2Body*)body)->SetAngularVelocity(angle);
}

void b2Body_SetLinearVelocity(void* body, double x, double y) {
  ((b2Body*)body)->SetLinearVelocity(b2Vec2(x, y));
}

void b2Body_SetMassData(void* body, double mass, double centerX,
                        double centerY, double inertia) {
  b2MassData m;
  m.mass = mass;
  m.center.Set(centerX, centerY);
  m.I = inertia;
  ((b2Body*)body)->SetMassData(&m);
}

void b2Body_SetTransform(void* body, double x, double y, double angle) {
  ((b2Body*)body)->SetTransform(b2Vec2(x, y), angle);
}

void b2Body_SetType(void* body, double type) {
  ((b2Body*)body)->SetType((b2BodyType)type);
}
