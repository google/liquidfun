#ifndef B2GEARJOINTJSBINDINGS_H
#define B2GEARJOINTJSBINDINGS_H

extern "C" {
void b2GearJoint_SetRatio(void* joint, double ratio);

void* b2GearJointDef_Create(
    void* world,
    // jointDef
    void* bodyA, void* bodyB, double collideConnected,
    // gear joint def
    void* joint1, void* joint2, double ratio);
}

#endif
