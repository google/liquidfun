#ifndef B2ROPEJOINTJSBINDINGS_H
#define B2ROPEJOINTJSBINDINGS_H

extern "C" {
void b2RopeJoint_SetMaxLength(void* joint, double length);
double b2RopeJoint_GetLimitState(void* joint);

void* b2RopeJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    //rope joint def
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double maxLength);
}

#endif
