#ifndef B2PULLEYJOINTJSBINDINGS_H
#define B2PULLEYJOINTJSBINDINGS_H

extern "C" {
void* b2PulleyJointDef_Create(
    void* world,
    // joint def
    void* bodyA, void* bodyB, double collideConnected,
    // pulley joint def
    double groundAnchorAx, double groundAnchorAy, double groundAnchorBx,
    double groundAnchorBy, double lengthA, double lengthB,
    double localAnchorAx, double localAnchorAy, double localAnchorBx,
    double localAnchorBy, double ratio);

void* b2PulleyJointDef_InitializeAndCreate(
    void* world,
    // initialize args
    void* bodyA, void* bodyB, double anchorAx,
    double anchorAy, double anchorBx, double anchorBy,
    double groundAnchorAx, double groundAnchorAy, double groundAnchorBx,
    double groundAnchorBy,  double ratio,
    // jointdef
    double collideConnected);

}

#endif
