#include <Box2D/Box2D.h>
void* b2WeldJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // weld joint def
    double dampingRatio, double frequencyHz, double localAnchorAx,
    double localAnchorAy, double localAnchorBx, double localAnchorBy,
    double referenceAngle) {
  b2WeldJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.dampingRatio = dampingRatio;
  def.frequencyHz = frequencyHz;
  def.localAnchorA = b2Vec2(localAnchorAx, localAnchorAy);
  def.localAnchorB = b2Vec2(localAnchorBx, localAnchorBy);
  def.referenceAngle = referenceAngle;

  return ((b2World*)world)->CreateJoint(&def);
}

void* b2WeldJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB, double anchorX,
    double anchorY,
    // joint def
    double collideConnected,
    // weld joint def
    double dampingRatio, double frequencyHz) {
  b2WeldJointDef def;
  def.collideConnected = collideConnected;

  def.dampingRatio = dampingRatio;
  def.frequencyHz = frequencyHz;

  def.Initialize((b2Body*)bodyA, (b2Body*)bodyB, b2Vec2(anchorX, anchorY));

  return ((b2World*)world)->CreateJoint(&def);
}
