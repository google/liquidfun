#include <Box2D/Box2D.h>
void* b2PulleyJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // pulley joint def
    double groundAnchorAx, double groundAnchorAy, double groundAnchorBx,
    double groundAnchorBy, double lengthA, double lengthB,
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double ratio) {
  b2PulleyJointDef def;
  def.bodyA = (b2Body*)bodyA;
  def.bodyB = (b2Body*)bodyB;
  def.collideConnected = collideConnected;

  def.groundAnchorA = b2Vec2(groundAnchorAx, groundAnchorAy);
  def.groundAnchorB = b2Vec2(groundAnchorBx, groundAnchorBy);
  def.lengthA = lengthA;
  def.lengthB = lengthB;
  def.localAnchorA = b2Vec2(localAnchorAx, localAnchorAy);
  def.localAnchorB = b2Vec2(localAnchorBx, localAnchorBy);
  def.ratio = ratio;

  return ((b2World*)world)->CreateJoint(&def);
}

void* b2PulleyJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB, double anchorAx,
    double anchorAy, double anchorBx, double anchorBy,
    double groundAnchorAx, double groundAnchorAy, double groundAnchorBx,
    double groundAnchorBy,  double ratio,
    // jointdef
    double collideConnected) {
  b2PulleyJointDef def;
  def.collideConnected = collideConnected;

  def.Initialize(
      (b2Body*)bodyA, (b2Body*)bodyB, b2Vec2(groundAnchorAx, groundAnchorAy),
      b2Vec2(groundAnchorBx, groundAnchorBy), b2Vec2(anchorAx, anchorAy),
      b2Vec2(anchorBx, anchorBy), ratio);

  return ((b2World*)world)->CreateJoint(&def);
}
