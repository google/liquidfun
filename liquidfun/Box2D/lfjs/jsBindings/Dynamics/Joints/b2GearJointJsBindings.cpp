#include <Box2D/Box2D.h>
void b2GearJoint_SetRatio(void* joint, double ratio) {
    ((b2GearJoint*)joint)->SetRatio(ratio);
}

void* b2GearJointDef_Create(
    void* world,
    // jointDef
    void* bodyA, void* bodyB, double collideConnected,
    // gear joint def
    void* joint1, void* joint2, double ratio) {
  b2GearJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.joint1 = (b2Joint*)joint1;
  def.joint2 = (b2Joint*)joint2;
  def.ratio = ratio;

  return ((b2World*)world)->CreateJoint(&def);
}
