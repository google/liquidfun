#include <Box2D/Box2D.h>
#include <stdio.h>
void b2MouseJoint_SetTarget(void* mouseJoint, double x, double y) {
  ((b2MouseJoint*)mouseJoint)->SetTarget(b2Vec2(x, y));
}
void b2MouseJoint_GetTarget(void* joint, float* arr) {
    b2Vec2 offset = ((b2MouseJoint*)joint)->GetTarget();
    arr[0] = force.x;
    arr[1] = force.y;
}
void b2MouseJoint_SetMaxForce(void* joint, double force) {
    ((b2MouseJoint*)joint)->SetMaxForce(force);
}
double b2MouseJoint_GetMaxForce(void* joint) {
  return ((b2MouseJoint*)joint)->GetMaxForce();
}
void b2MouseJoint_SetFrequency(void* joint, double frequencyHz) {
    ((b2MouseJoint*)joint)->SetLFrequency(frequencyHz);
}
double b2MouseJoint_GetFrequency(void* joint) {
  return ((b2MouseJoint*)joint)->GetFrequency();
}
void b2MouseJoint_SetDampingRatio(void* joint, double dampingRatio) {
    ((b2MouseJoint*)joint)->SetDampingRatio(dampingRatio);
}
double b2MouseJoint_GetDampingRatio(void* joint) {
  return ((b2MouseJoint*)joint)->GetDampingRatio();
}

void* b2MouseJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // mouse joint def
    double dampingRatio, double frequencyHz, double maxForce,
    double targetX, double targetY) {
  b2MouseJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.dampingRatio = dampingRatio;
  def.frequencyHz = frequencyHz;
  def.maxForce = maxForce;
  def.target = b2Vec2(targetX, targetY);

  return ((b2World*)world)->CreateJoint(&def);
}
