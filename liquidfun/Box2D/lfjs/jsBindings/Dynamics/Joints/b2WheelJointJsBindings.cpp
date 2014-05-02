#include <Box2D/Box2D.h>

void b2WheelJoint_SetMotorSpeed(void* wheel, double speed) {
  ((b2WheelJoint*)wheel)->SetMotorSpeed(speed);
}

void b2WheelJoint_SetSpringFrequencyHz(void* wheel, double frequency) {
  ((b2WheelJoint*)wheel)->SetSpringFrequencyHz(frequency);
}

void* b2WheelJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // wheel joint def
    double dampingRatio, double enableMotor, double frequencyHz,
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double localAxisAx, double localAxisAy,
    double maxMotorTorque, double motorSpeed) {
  b2WheelJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.dampingRatio = dampingRatio;
  def.enableMotor = enableMotor;
  def.frequencyHz = frequencyHz;
  def.localAnchorA = b2Vec2(localAnchorAx, localAnchorAy);
  def.localAnchorB = b2Vec2(localAnchorBx, localAnchorBy);
  def.localAxisA = b2Vec2(localAxisAx, localAxisAy);
  def.maxMotorTorque = maxMotorTorque;
  def.motorSpeed = motorSpeed;

  return ((b2World*)world)->CreateJoint(&def);
}

void* b2WheelJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB, double anchorX,
    double anchorY, double axisX, double axisY,
    // joint def
    double collideConnected,
    // wheel joint def
    double dampingRatio, double enableMotor, double frequencyHz,
    double maxMotorTorque, double motorSpeed) {
  b2WheelJointDef def;
  def.collideConnected = collideConnected;

  def.dampingRatio = dampingRatio;
  def.enableMotor = enableMotor;
  def.frequencyHz = frequencyHz;
  def.maxMotorTorque = maxMotorTorque;
  def.motorSpeed = motorSpeed;

  def.Initialize((b2Body*)bodyA, (b2Body*)bodyB, b2Vec2(anchorX, anchorY), b2Vec2(axisX, axisY));

  return ((b2World*)world)->CreateJoint(&def);
}
