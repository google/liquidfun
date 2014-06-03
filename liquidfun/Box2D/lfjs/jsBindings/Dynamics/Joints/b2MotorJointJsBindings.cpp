#include <Box2D/Box2D.h>

void b2MotorJoint_SetAngularOffset(void* motorJoint, double angle) {
  ((b2MotorJoint*)motorJoint)->SetAngularOffset(angle);
}

void b2MotorJoint_SetLinearOffset(void* motorJoint, double x, double y) {
  ((b2MotorJoint*)motorJoint)->SetLinearOffset(b2Vec2(x, y));
}

void* b2MotorJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // motorJointDef
    double angularOffset, double correctionFactor, double linearOffsetX,
    double linearOffsetY, double maxForce, double maxTorque) {
  b2MotorJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.angularOffset = angularOffset;
  def.correctionFactor = correctionFactor;
  def.linearOffset = b2Vec2(linearOffsetX, linearOffsetY);
  def.maxForce = maxForce;
  def.maxTorque = maxTorque;

  return ((b2World*)world)->CreateJoint(&def);
}

void* b2MotorJointDef_InitializeAndCreate(
    void* world,
    //initialize args
    void* bodyA, void* bodyB,
    // joint def
    double collideConnected,
    // motorjoint def
    double correctionFactor, double maxForce, double maxTorque) {
  b2MotorJointDef def;
  def.collideConnected = collideConnected;

  def.correctionFactor = correctionFactor;
  def.maxForce = maxForce;
  def.maxTorque = maxTorque;

  def.Initialize((b2Body*)bodyA, (b2Body*)bodyB);

  return ((b2World*)world)->CreateJoint(&def);
}
