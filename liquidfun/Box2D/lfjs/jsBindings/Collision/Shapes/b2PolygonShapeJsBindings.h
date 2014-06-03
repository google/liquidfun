#ifndef B2POLYGONSHAPEJSBINDINGS_H
#define B2POLYGONSHAPEJSBINDINGS_H
extern "C" {
// b2Body create fixture from polygonShape
void* b2PolygonShape_CreateFixture_3(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2);

void* b2PolygonShape_CreateFixture_4(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3);

void* b2PolygonShape_CreateFixture_5(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4);

void* b2PolygonShape_CreateFixture_6(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    double x5, double y5);

void* b2PolygonShape_CreateFixture_7(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    double x5, double y5,
    double x6, double y6);

void* b2PolygonShape_CreateFixture_8(
    void* body,
    // Fixturedef
    double density, double friction, double isSensor,
    double restitution, double userData,
    // filter
    double categoryBits, double groupIndex, double maskBits,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double x4, double y4,
    double x5, double y5,
    double x6, double y6,
    double x7, double y7);


// functions to create particle group from polygon
void* b2PolygonShape_CreateParticleGroup_4(
    void* particleSystem,
    // ParticleGroupDef
    double angle, double angularVelocity, double colorR,
    double colorG, double colorB, double colorA, double flags, double group,
    double groupFlags, double lifetime, double linearVelocityX, double linearVelocityY,
    double positionX, double positionY, double positionData, double particleCount,
    double strength, double stride, double userData,
    // shape
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3);

// functions to destroy particles in shape
double b2PolygonShape_DestroyParticlesInShape_4(
    void* particleSystem,
    //polygon
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    // xf
    double xfpX, double xfpY, double xfqS,
    double xfqC);

}

#endif
