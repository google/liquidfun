#ifndef B2CIRCLESHAPEJSBINDINGS_H
#define B2CIRCLESHAPEJSBINDINGS_H
extern "C" {
// b2Body create fixture from circle
void* b2CircleShape_CreateFixture(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // circle
    double px, double py,
    double radius);

void* b2CircleShape_CreateParticleGroup(
    void* particleSystem,
    // ParticleGroupDef
    double angle, double angularVelocity, double colorR,
    double colorG, double colorB, double colorA, double flags, double group,
    double groupFlags, double lifetime, double linearVelocityX, double linearVelocityY,
    double positionX, double positionY, double positionData, double particleCount,
    double strength, double stride, double userData,
    // Circle
    double px, double py,
    double radius);

double b2CircleShape_DestroyParticlesInShape(
    void* particleSystem,
    // circle
    double px, double py, double radius,
    // xf
    double xfpX, double xfpY, double xfqS,
    double xfqC);
}
#endif
