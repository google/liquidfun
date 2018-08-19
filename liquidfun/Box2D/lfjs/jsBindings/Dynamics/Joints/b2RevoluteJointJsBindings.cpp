#include <Box2D/Box2D.h>
#include <stdio.h>

void b2RevoluteJoint_EnableLimit(void* joint, double flag) {
  ((b2RevoluteJoint*)joint)->EnableLimit(flag);
}
void b2RevoluteJoint_EnableMotor(void* joint, double flag) {
  ((b2RevoluteJoint*)joint)->EnableMotor(flag);
}
double b2RevoluteJoint_GetJointAngle(void* joint) {
  return ((b2RevoluteJoint*)joint)->GetJointAngle();
}
double b2RevoluteJoint_IsLimitEnabled(void* joint) {
  return ((b2RevoluteJoint*)joint)->IsLimitEnabled();
}
double b2RevoluteJoint_IsMotorEnabled(void* joint) {
  return ((b2RevoluteJoint*)joint)->IsMotorEnabled();
}
double b2RevoluteJoint_GetReferenceAngle(void* joint) {
    return ((b2RevoluteJoint*)joint)->GetReferenceAngle();
}
double b2RevoluteJoint_GetJointSpeed(void* joint) {
    return ((b2RevoluteJoint*)joint)->GetJointSpeed();
}
double b2RevoluteJoint_GetLowerLimit(void* joint) {
    return ((b2RevoluteJoint*)joint)->GetLowerLimit();
}
double b2RevoluteJoint_GetUpperLimit(void* joint) {
    return ((b2RevoluteJoint*)joint)->GetUpperLimit();
}
void b2RevoluteJoint_SetLimits(void* joint, double lower, double upper) {
    ((b2RevoluteJoint*)joint)->SetLimits(lower, upper);
}
double b2RevoluteJoint_GetMotorSpeed(void* joint) {
    return ((b2RevoluteJoint*)joint)->GetMotorSpeed();
}
void b2RevoluteJoint_SetMaxMotorTorque(void* joint, double torque) {
    ((b2RevoluteJoint*)joint)->SetMaxMotorTorque(torque);
}
double b2RevoluteJoint_GetMaxMotorTorque(void* joint) {
    return ((b2RevoluteJoint*)joint)->GetMaxMotorTorque();
}
double b2RevoluteJoint_GetMotorTorque(void* joint, double inv_dt) {
    return ((b2RevoluteJoint*)joint)->GetMotorTorque(inv_dt);
}

void* b2RevoluteJointDef_Create(
    void* world,
    //Joint def
    void* bodyA, void* bodyB, double collideConnected,
    //revoluteJointDef
    double enableLimit, double enableMotor, double lowerAngle,
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double maxMotorTorque, double motorSpeed,
    double referenceAngle, double upperAngle) {
  b2RevoluteJointDef revJoint;
  revJoint.bodyA = (b2Body*)bodyA;
  revJoint.bodyB = (b2Body*)bodyB;
  revJoint.collideConnected = collideConnected;
  revJoint.enableLimit = enableLimit;
  revJoint.enableMotor = enableMotor;
  revJoint.localAnchorA = b2Vec2(localAnchorAx, localAnchorAy);
  revJoint.localAnchorB = b2Vec2(localAnchorBx, localAnchorBy);
  revJoint.lowerAngle = lowerAngle;
  revJoint.maxMotorTorque = maxMotorTorque;
  revJoint.motorSpeed = motorSpeed;
  revJoint.referenceAngle = referenceAngle;
  revJoint.upperAngle = upperAngle;

  return ((b2World*)world)->CreateJoint(&revJoint);
}

void* b2RevoluteJointDef_InitializeAndCreate(
    void* world, void* bodyA, void* bodyB, double anchorX, double anchorY,
    //revoluteJointDef
    double collideConnected, double enableLimit,
    double enableMotor, double lowerAngle, double maxMotorTorque,
    double motorSpeed, double upperAngle) {
  b2RevoluteJointDef revJoint;
  revJoint.collideConnected = collideConnected;
  revJoint.enableLimit = enableLimit;
  revJoint.enableMotor = enableMotor;
  revJoint.lowerAngle = lowerAngle;
  revJoint.maxMotorTorque = maxMotorTorque;
  revJoint.motorSpeed = motorSpeed;
  revJoint.upperAngle = upperAngle;

  revJoint.Initialize((b2Body*)bodyA, (b2Body*)bodyB, b2Vec2(anchorX, anchorY));
  return ((b2World*)world)->CreateJoint(&revJoint);
}

void b2RevoluteJoint_SetMotorSpeed(void* joint, double speed) {
  ((b2RevoluteJoint*)joint)->SetMotorSpeed(speed);
}
