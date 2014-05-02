#include <Box2D/Box2D.h>
void* b2RopeJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    //rope joint def
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double maxLength) {
  b2RopeJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.localAnchorA = b2Vec2(localAnchorAx, localAnchorAy);
  def.localAnchorB = b2Vec2(localAnchorBx, localAnchorBy);
  def.maxLength = maxLength;

  return ((b2World*)world)->CreateJoint(&def);
}
