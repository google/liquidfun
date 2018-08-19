#ifndef B2WHEELJOINTJSBINDINGS_H
#define B2WHEELJOINTJSBINDINGS_H

extern "C" {
void b2WheelJoint_SetMotorSpeed(void* wheel, double speed);
void b2WheelJoint_SetSpringFrequencyHz(void* wheel, double frequency);

void b2WheelJoint_GetLocalAxisA(void* joint, float* arr);
double b2WheelJoint_GetJointTranslation(void* joint);
double b2WheelJoint_GetJointSpeed(void* joint);
double b2WheelJoint_IsMotorEnabled(void* joint);
void b2WheelJoint_EnableMotor(void* joint, double flag);
double b2WheelJoint_GetMotorSpeed(void* joint);
void b2WheelJoint_SetMaxMotorTorque(void* joint, double torque);
double b2WheelJoint_GetMaxMotorTorque(void* joint);
double b2WheelJoint_GetMotorTorque(void* joint, double inv_dt);
double b2WheelJoint_GetSpringFrequencyHz(void* joint);
void b2WheelJoint_SetSpringDampingRatio(void* joint, double ratio);
double b2WheelJoint_GetSpringDampingRatio(void* joint);

void* b2WheelJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // wheel joint def
    double dampingRatio, double enableMotor, double frequencyHz,
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double localAxisAx, double localAxisAy,
    double maxMotorTorque, double motorSpeed);

void* b2WheelJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB, double anchorX,
    double anchorY, double axisX, double axisY,
    //joint def
    double collideConnected,
    // wheel joint def
    double dampingRatio, double enableMotor, double frequencyHz,
    double maxMotorTorque, double motorSpeed);
}

#endif
