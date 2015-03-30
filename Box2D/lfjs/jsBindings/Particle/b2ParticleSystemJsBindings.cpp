#include<Box2D/Box2D.h>
#include <stdio.h>

double b2ParticleSystem_CreateParticle(void* particleSystem,
    //particleDef
    double colorR, double colorB, double colorG, double colorA,
    double flags, double group, double lifetime, double positionX,
    double positionY, double userData, double velocityX, double velocityY) {
  b2ParticleDef def;
  def.color = b2ParticleColor(colorR, colorG, colorB, colorA);
  def.flags = flags;
  def.group = (b2ParticleGroup*)&group;
  def.lifetime = lifetime;
  def.position = b2Vec2(positionX, positionY);
  def.userData = (double*)&userData;
  def.velocity = b2Vec2(velocityX, velocityY);

  return ((b2ParticleSystem*)particleSystem)->CreateParticle(def);
}

// Shapes array is not currently supported for b2ParticleSystems

void* b2ParticleSystem_GetColorBuffer(void* particleSystem) {
  return ((b2ParticleSystem*)particleSystem)->GetColorBuffer();
}
double b2ParticleSystem_GetParticleCount(void* particleSystem) {
  return ((b2ParticleSystem*)particleSystem)->GetParticleCount();
}

double b2ParticleSystem_GetParticleLifetime(void* particleSystem, double index){
  return ((b2ParticleSystem*)particleSystem)->GetParticleLifetime((int)index);
}

void* b2ParticleSystem_GetPositionBuffer(void* particleSystem) {
  return ((b2ParticleSystem*)particleSystem)->GetPositionBuffer();
}
void* b2ParticleSystem_GetVelocityBuffer(void* particleSystem) {
  return ((b2ParticleSystem*)particleSystem)->GetVelocityBuffer();
}
void b2ParticleSystem_SetDamping(void* particleSystem, double damping) {
  ((b2ParticleSystem*)particleSystem)->SetDamping(damping);
}
void b2ParticleSystem_SetDensity(void* particleSystem, double density) {
  ((b2ParticleSystem*)particleSystem)->SetDensity(density);
}

void b2ParticleSystem_SetGravityScale(void* particleSystem, double gravityScale) {
  ((b2ParticleSystem*)particleSystem)->SetGravityScale(gravityScale);
}

void b2ParticleSystem_SetMaxParticleCount(void* particleSystem, double count) {
  ((b2ParticleSystem*)particleSystem)->SetMaxParticleCount((int)count);
}

void b2ParticleSystem_SetParticleLifetime(void* particleSystem, double index, double lifetime){
  ((b2ParticleSystem*)particleSystem)->SetParticleLifetime((int)index, lifetime);
}

void b2ParticleSystem_SetRadius(void* particleSystem, double radius) {
  ((b2ParticleSystem*)particleSystem)->SetRadius(radius);
}
