#ifndef B2MOUSEJOINTJSBINDINGS_H
#define B2MOUSEJOINTJSBINDINGS_H

extern "C" {
void b2MouseJoint_SetTarget(void* mouseJoint, double x, double y);
void b2MouseJoint_GetTarget(void* mouseJoint, float* arr);
void b2MouseJoint_SetMaxForce(void* joint, double force);
double b2MouseJoint_GetMaxForce(void* joint);
void b2MouseJoint_SetFrequency(void* joint, double frequencyHz);
double b2MouseJoint_GetFrequency();
void b2MouseJoint_SetDampingRatio(void* joint, double dampingRatio);
double b2MouseJoint_GetDampingRatio();

void* b2MouseJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // mouse joint def
    double dampingRatio, double frequencyHz, double maxForce,
    double targetX, double targetY);
}

#endif
