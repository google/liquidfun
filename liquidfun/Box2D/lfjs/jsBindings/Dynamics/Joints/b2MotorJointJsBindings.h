#ifndef B2MOTORJOINTJSBINDINGS_H
#define B2MOTORJOINTJSBINDINGS_H

extern "C" {
void b2MotorJoint_SetAngularOffset(void* motorJoint, double angle);
void b2MotorJoint_SetLinearOffset(void* motorJoint, double x, double y);
double b2MotorJoint_GetAngularOffset(void* joint);
void b2MotorJoint_GetLinearOffset(void* joint, float* arr);
void b2MotorJoint_SetMaxForce(void* joint, double force);
double b2MotorJoint_GetMaxForce(void* joint);
void b2MotorJoint_SetMaxTorque(void* joint, double torque);
double b2MotorJoint_GetMaxTorque(void* joint);
void b2MotorJoint_SetCorrectionFactor(void* joint, double factor);
double b2MotorJoint_GetCorrectionFactor(void* joint);

void* b2MotorJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // motorJointDef
    double angularOffset, double correctionFactor, double linearOffsetX,
    double linearOffsetY, double maxForce, double maxTorque);

void* b2MotorJointDef_InitializeAndCreate(
    void* world,
    //initialize args
    void* bodyA, void* bodyB,
    // joint def
    double collideConnected,
    // motorjoint def
    double correctionFactor, double maxForce, double maxTorque);
}

#endif
