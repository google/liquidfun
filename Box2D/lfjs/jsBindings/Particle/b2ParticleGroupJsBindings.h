#ifndef B2PARTICLEGROUP_H
#define B2PARTICLEGROUP_H

extern "C" {
void b2ParticleGroup_ApplyForce(
    void* particleGroup, double forceX, double forceY);
void b2ParticleGroup_ApplyLinearImpulse(
    void* particleGroup, double impulseX, double impulseY);
void b2ParticleGroup_DestroyParticles(void* particleGroup, double flag);
double b2ParticleGroup_GetBufferIndex(void* particleGroup);
double b2ParticleGroup_GetParticleCount(void* particleGroup);
}

#endif
