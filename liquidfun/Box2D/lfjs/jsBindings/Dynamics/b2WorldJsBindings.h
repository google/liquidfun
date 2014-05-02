#ifndef B2WORLDJSBINDINGS_H
#define B2WORLDJSBINDINGS_H

extern "C" {
//b2World functions
void* b2World_Create(double x, double y);
void* b2World_CreateBody(
    void* world, double active, double allowSleep,
    double angle, double angularVelocity, double angularDamping,
    double awake, double bullet, double fixedRotation,
    double gravityScale, double linearDamping, double linearVelocityX,
    double linearVelocityY, double positionX, double positionY,
    double type, void* userData);

void* b2World_CreateParticleSystem(
    void* world, double colorMixingStrength, double dampingStrength,
    double destroyByAge, double ejectionStrength, double elasticStrength,
    double lifetimeGranularity, double powderStrength, double pressureStrength,
    double radius, double repulsiveStrength, double springStrength,
    double staticPressureIterations, double staticPressureRelaxation, double staticPressureStrength,
    double surfaceTensionNormalStrength, double surfaceTensionPressureStrength, double viscousStrength);


void b2World_Delete(void* world);
void b2World_DestroyBody(void* world, void* body);
void b2World_DestroyJoint(void* world, void* joint);
void b2World_DestroyParticleSystem(void* world, void* particleSystem);
void b2World_QueryAABB(void* world, double aabbLowerBoundX, double aabbLowerBoundY,
                       double aabbUpperBoundX, double aabbUpperBoundY);
void b2World_RayCast(void* world, double point1X, double point1Y,
                     double point2X, double point2Y);
void b2World_SetContactListener(void* world);
void b2World_SetGravity(void* world, double x, double y);
void b2World_Step(void* world, float step, float vIterations, float pIterations);
}

#endif
