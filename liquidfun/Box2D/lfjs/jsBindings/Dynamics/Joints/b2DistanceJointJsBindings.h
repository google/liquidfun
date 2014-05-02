#ifndef B2DISTANCEJOINTJSBINDINGS_H
#define B2DISTANCEJOINTJSBINDINGS_H

extern "C" {
void* b2DistanceJointDef_Create(
    void* world,
    //joint def
    void* bodyA, void* bodyB, double collideConnected,
    // distanceJointDef
    double dampingRatio, double frequencyHz, double length,
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy);

void* b2DistanceJointDef_InitializeAndCreate(
    void* world,
    void* bodyA, void* bodyB,
    double anchorAx, double anchorAy,
    double anchorBx, double anchorBy,
    // joint def
    double collideConnected,
    // distancejoint def
    double dampingRatio, double frequencyHz);

}

#endif
