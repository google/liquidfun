#ifndef B2WHEELJOINTJSBINDINGS_H
#define B2WHEELJOINTJSBINDINGS_H

extern "C" {
void b2WheelJoint_SetMotorSpeed(void* wheel, double speed);
void b2WheelJoint_SetSpringFrequencyHz(void* wheel, double frequency);

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
