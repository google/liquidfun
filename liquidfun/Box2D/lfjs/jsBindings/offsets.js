/*
  TODO: Make this a fully automated process
  this semi automatically generated file is used for optimizations.
  Please profile first whenever you want to use it, somethings end up costing more.
  Things which weren't faster with offsets:
  navigating bodys list was MANY ORDERS OF MAGNITUDE slower
 */

var Offsets = {
  b2Body: {
    type: 0,
    islandIndex: 8,
    xf: 12,
    xf0: 28,
    sweep: 44,
    linearVelocity: 80,
    angularVelocity: 88,
    force: 92,
    torque: 100,
    world: 104,
    prev: 108,
    next: 112,
    fixtureList: 116,
    fixtureCount: 120,
    jointList: 124,
    contactList: 128,
    mass: 132,
    invMass: 136,
    I: 140,
    invI: 144,
    linearDamping: 148,
    angularDamping: 152,
    gravityScale: 156,
    sleepTime: 160,
    userData: 164
  },
  b2Contact: {
    flags: 4,
    prev: 8,
    next: 12,
    nodeA: 16,
    nodeB: 32,
    fixtureA: 48,
    fixtureB: 52,
    indexA: 56,
    indexB: 60,
    manifold: 64,
    toiCount: 128,
    toi: 132,
    friction: 136,
    restitution: 140,
    tangentSpeed: 144
  },
  b2Fixture: {
    density: 0,
    next: 4,
    body: 8,
    shape: 12,
    friction: 16,
    restitution: 20,
    proxies: 24,
    proxyCount: 28,
    filter: 32,
    isSensor: 38,
    userData: 40
  },
  b2ParticleGroup: {
    system: 0,
    firstIndex: 4,
    lastIndex: 8,
    groupFlags: 12,
    strength: 16,
    prev: 20,
    next: 24,
    timestamp: 28,
    mass: 32,
    inertia: 36,
    center: 40,
    linearVelocity: 48,
    angularVelocity: 56,
    transform: 60,
    userData: 76
  },
  b2WorldManifold: {
    normal: 0,
    points: 8,
    separations: 24
  },
  b2World: {
    bodyList: 102960
  }
};