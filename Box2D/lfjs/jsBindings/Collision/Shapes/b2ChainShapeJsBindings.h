#ifndef B2CHAINSHAPEJSBINDINGS_H
#define B2CHAINSHAPEJSBINDINGS_H
extern "C" {
// b2body create fixture from chain
void* b2ChainShape_CreateFixture(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // chain
    float* vertices, double length);
}
#endif
