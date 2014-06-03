#include <Box2D/Box2D.h>
#include <stdio.h>
void b2MouseJoint_SetTarget(void* mouseJoint, double x, double y) {
  ((b2MouseJoint*)mouseJoint)->SetTarget(b2Vec2(x, y));
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
