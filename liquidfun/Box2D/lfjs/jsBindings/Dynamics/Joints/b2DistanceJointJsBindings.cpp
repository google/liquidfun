#include <Box2D/Box2D.h>
#include <stdio.h>
void* b2DistanceJointDef_Create(
    void* world,
    //joint def
    void* bodyA, void* bodyB, double collideConnected,
    // distanceJointDef
    double dampingRatio, double frequencyHz, double length,
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy) {
  b2DistanceJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.dampingRatio = dampingRatio;
  def.frequencyHz = frequencyHz;
  def.length = length;
  def.localAnchorA = b2Vec2(localAnchorAx, localAnchorAy);
  def.localAnchorB = b2Vec2(localAnchorBx, localAnchorBy);

  return ((b2World*)world)->CreateJoint(&def);
}

void* b2DistanceJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB,
    double anchorAx, double anchorAy,
    double anchorBx, double anchorBy,
    // joint def
    double collideConnected,
    // distancejoint def
    double dampingRatio, double frequencyHz) {
  b2DistanceJointDef def;
  def.collideConnected = collideConnected;

  def.dampingRatio = dampingRatio;
  def.frequencyHz = frequencyHz;
  def.Initialize((b2Body*)bodyA, (b2Body*)bodyB, b2Vec2(anchorAx, anchorAy),
                 b2Vec2(anchorBx, anchorBy));

  return ((b2World*)world)->CreateJoint(&def);

}

void b2DistanceJoint_SetLength(void* joint, double length) {
    ((b2DistanceJoint*)joint)->SetLength(length);
}
double b2DistanceJoint_GetLength(void* joint) {
  return ((b2DistanceJoint*)joint)->GetLength();
}
void b2DistanceJoint_SetFrequency(void* joint, double frequencyHz) {
    ((b2DistanceJoint*)joint)->SetLFrequency(frequencyHz);
}
double b2DistanceJoint_GetFrequency(void* joint) {
  return ((b2DistanceJoint*)joint)->GetFrequency();
}
void b2DistanceJoint_SetDampingRatio(void* joint, double dampingRatio) {
    ((b2DistanceJoint*)joint)->SetDampingRatio(dampingRatio);
}
double b2DistanceJoint_GetDampingRatio(void* joint) {
  return ((b2DistanceJoint*)joint)->GetDampingRatio();
}