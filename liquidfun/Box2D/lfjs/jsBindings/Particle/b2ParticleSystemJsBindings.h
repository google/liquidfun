#ifndef B2PARTICLESYSTEMJSBINDINGS
#define B2PARTICLESYSTEMJSBINDINGS
extern "C" {
double b2ParticleSystem_CreateParticle(void* particleSystem,
    //particleDef
    double colorR, double colorB, double colorG, double colorA,
    double flags, double group, double lifetime, double positionX,
    double positionY, double userData, double velocityX, double velocityY);

void* b2ParticleSystem_GetColorBuffer(void* particleSystem);
double b2ParticleSystem_GetParticleCount(void* particleSystem);
void* b2ParticleSystem_GetPositionBuffer(void* particleSystem);
void* b2ParticleSystem_GetVelocityBuffer(void* particleSystem);

void b2ParticleSystem_SetDamping(void* particleSystem, double damping);
void b2ParticleSystem_SetDensity(void* particleSystem, double density);
void b2ParticleSystem_SetRadius(void* particleSystem, double radius);
}
#endif
