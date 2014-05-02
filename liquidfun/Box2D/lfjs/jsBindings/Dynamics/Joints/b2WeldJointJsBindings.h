#ifndef B2WELDJOINTJSBINDINGS_H
#define B2WELDJOINTJSBINDINGS_H

extern "C" {
void* b2WeldJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // weld joint def
    double dampingRatio, double frequencyHz, double localAnchorAx,
    double localAnchorAy, double localAnchorBx, double localAnchorBy,
    double referenceAngle);

void* b2WeldJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB, double anchorX,
    double anchorY,
    // joint def
    double collideConnected,
    // weld joint def
    double dampingRatio, double frequencyHz);
}

#endif
