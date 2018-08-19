#ifndef B2JOINTJSBINDINGS_H
#define B2JOINTJSBINDINGS_H

extern "C"  {
void* b2Joint_GetBodyA(void* joint);
void* b2Joint_GetBodyB(void* joint);
double b2Joint_GetType(void* joint);
void b2Joint_GetAnchorA(void* joint, float* arr);
void b2Joint_GetAnchorB(void* joint, float* arr);
void b2Joint_GetReactionForce(void* joint, double inv_dt, float* arr);
double b2Joint_GetReactionTorque(void* joint, double inv_dt);
bool b2Joint_IsActive(void* joint);
bool b2Joint_GetCollideConnected(void* joint);
}

#endif
