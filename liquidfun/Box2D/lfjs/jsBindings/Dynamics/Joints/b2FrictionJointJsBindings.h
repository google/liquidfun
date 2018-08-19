#ifndef B2FRICTIONJOINTJSBINDINGS_H
#define B2FRICTIONJOINTJSBINDINGS_H

extern "C" {
// The creation function lives off of b2World, but we put it here for neatness
void* b2FrictionJointDef_Create(
    void* world,
    //joint def
    void* bodyA, void* bodyB, double collideConnected,
    // friction joint def
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double maxForce, double maxTorque);

void* b2FrictionJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB, double anchorX,
    double anchorY,
    // joint def
    double collideConnected,
    // frictionjointdef
    double maxForce, double maxTorque);
}

void b2FrictionJoint_SetMaxForce(void* joint, double force);
double b2FrictionJoint_GetMaxForce(void* joint);
void b2FrictionJoint_SetMaxTorque(void* joint, double torque);
double b2FrictionJoint_GetMaxTorque(void* joint);

#endif
