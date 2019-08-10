#include<Box2D/Box2D.h>

void* b2Joint_GetBodyA(void* joint) {
  return ((b2Joint*)joint)->GetBodyA();
}
void* b2Joint_GetBodyB(void* joint) {
  return ((b2Joint*)joint)->GetBodyB();
}
double b2Joint_GetType(void* joint) {
  return ((b2Joint*)joint)->GetType();
}
void b2Joint_GetAnchorA(void* joint, float* arr) {
  b2Vec2 anchor = ((b2Joint*)joint)->GetAnchorA();
  arr[0] = anchor.x;
  arr[1] = anchor.y;
}
void b2Joint_GetAnchorB(void* joint, float* arr) {
  b2Vec2 anchor = ((b2Joint*)joint)->GetAnchorB();
  arr[0] = anchor.x;
  arr[1] = anchor.y;
}
void b2Joint_GetReactionForce(void* joint, double inv_dt, float* arr) {
  b2Vec2 force = ((b2Joint*)joint)->GetReactionForce(inv_dt);
  arr[0] = force.x;
  arr[1] = force.y;
}
double b2Joint_GetReactionTorque(void* joint, double inv_dt) {
    return ((b2Joint*)joint)->GetReactionTorque(inv_dt);
}
bool b2Joint_IsActive(void* joint) {
    return ((b2Joint*)joint)->IsActive();
}
bool b2Joint_GetCollideConnected(void* joint) {
    return ((b2Joint*)joint)->GetCollideConnected();
}