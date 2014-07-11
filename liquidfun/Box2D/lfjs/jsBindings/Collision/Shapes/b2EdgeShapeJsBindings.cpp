#include <Box2D/Box2D.h>
#include <stdio.h>
// b2Body create fixture from edge shape
void* b2EdgeShape_CreateFixture(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // circle
    double hasVertex0, double hasVertex3,
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  b2EdgeShape edge;
  edge.m_vertex0 = b2Vec2(x0, y0);
  edge.m_vertex1 = b2Vec2(x1, y1);
  edge.m_vertex2 = b2Vec2(x2, y2);
  edge.m_vertex3 = b2Vec2(x3, y3);
  edge.m_hasVertex0 = bool(hasVertex0);
  edge.m_hasVertex3 = bool(hasVertex3);

  def.shape = &edge;
  return ((b2Body*)body)->CreateFixture(&def);
}
