#ifndef B2EDGESHAPEJSBINDINGS_H
#define B2EDGESHAPEJSBINDINGS_H
extern "C" {
// b2Body create fixture from edgeshape
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
    double x3, double y3);
}
#endif
