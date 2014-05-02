#include <Box2D/Box2D.h>

// create fixture from chain
const int MaxChainVertices = 128;
void* b2ChainShape_CreateFixture(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // chain
    float* vertices, double length) {
  b2FixtureDef def;
  def.density = density;
  def.friction = friction;
  def.isSensor = isSensor;
  def.restitution = restitution;
  def.userData = (void*)&userData;
  def.filter.categoryBits = categoryBits;
  def.filter.groupIndex = groupIndex;
  def.filter.maskBits = maskBits;

  b2ChainShape chain;
  int count = length / 2;
  b2Vec2 vertexArr[MaxChainVertices];
  for (int i = 0, j = 0; i < length; i += 2, j++) {
    vertexArr[j] = b2Vec2(vertices[i], vertices[i+1]);
  }
  chain.CreateChain(vertexArr, count);

  def.shape = &chain;
  return ((b2Body*)body)->CreateFixture(&def);
}
