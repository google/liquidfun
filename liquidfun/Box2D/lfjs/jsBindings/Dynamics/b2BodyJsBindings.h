#ifndef B2BODYJSBINDINGS_H
#define B2BODYJSBINDINGS_H

extern "C" {
// b2Body exports
void b2Body_ApplyAngularImpulse(void* body, double impulse, double wake);
void b2Body_ApplyForce(void* body, double forceX, double forceY,
                       double pointX, double pointY, double wake);
void b2Body_ApplyForceToCenter(void* body, double forceX, double forceY,
                               double wake);
void b2Body_ApplyTorque(void* body, double force, double wake);
void b2Body_DestroyFixture(void* body, void* fixture);
double b2Body_GetAngle(void* body);
double b2Body_GetAngularVelocity(void* body);
double b2Body_GetInertia(void* body);
void b2Body_GetLinearVelocity(void* body, float* arr);
void b2Body_GetLocalPoint(void* body, double pointX, double pointY, float* arr);
void b2Body_GetLocalVector(void* body, double vX, double vY, float* arr);
double b2Body_GetMass(void* body);
double b2Body_GetType(void* body);
void b2Body_GetPosition(void* body, float* arr);
void b2Body_GetTransform(void* body, float* arr);
void b2Body_GetWorldCenter(void* body, float* arr);
void b2Body_GetWorldPoint(void* body, double pointX, double pointY, float* arr);
void b2Body_GetWorldVector(void* body, double vX, double vY, float* arr);
void b2Body_SetAngularVelocity(void* body, double angle);
void b2Body_SetAwake(void* body, double flag);
void b2Body_SetLinearVelocity(void* body, double x, double y);
void b2Body_SetMassData(void* body, double mass, double centerX,
                        double centerY, double inertia);
void b2Body_SetTransform(void* body, double x, double y, double angle);
void b2Body_SetType(void* body, double type);
}
#endif
