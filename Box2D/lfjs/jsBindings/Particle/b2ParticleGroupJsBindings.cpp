#include<Box2D/Box2D.h>
#include<stdio.h>
void b2ParticleGroup_ApplyForce(void* particleGroup, double forceX, double forceY) {
  ((b2ParticleGroup*)particleGroup)->ApplyForce(b2Vec2(forceX, forceY));
}
void b2ParticleGroup_ApplyLinearImpulse(void* particleGroup, double impulseX, double impulseY) {
  ((b2ParticleGroup*)particleGroup)->ApplyLinearImpulse(b2Vec2(impulseX, impulseY));
}
void b2ParticleGroup_DestroyParticles(void* particleGroup, double flag) {
  ((b2ParticleGroup*)particleGroup)->DestroyParticles((bool)flag);
}
double b2ParticleGroup_GetBufferIndex(void* particleGroup) {
  return ((b2ParticleGroup*)particleGroup)->GetBufferIndex();
}
double b2ParticleGroup_GetParticleCount(void* particleGroup) {
  return ((b2ParticleGroup*)particleGroup)->GetParticleCount();
}
