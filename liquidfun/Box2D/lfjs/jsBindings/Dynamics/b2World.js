// global call back functions
b2World.BeginContactBody = function(contactPtr) {
  if (world.listener.BeginContactBody === undefined) {
    return;
  }
  var contact = new b2Contact(contactPtr);
  world.listener.BeginContactBody(contact);
};

b2World.EndContactBody = function(contactPtr) {
  if (world.listener.EndContactBody === undefined) {
    return;
  }
  var contact = new b2Contact(contactPtr);
  world.listener.EndContactBody(contact);
};

b2World.PreSolve = function(contactPtr, oldManifoldPtr) {
  if (world.listener.PreSolve === undefined) {
    return;
  }
  world.listener.PreSolve(new b2Contact(contactPtr), new b2Manifold(oldManifoldPtr));
};

b2World.PostSolve = function(contactPtr, impulsePtr) {
  if (world.listener.PostSolve === undefined) {
    return;
  }
  world.listener.PostSolve(new b2Contact(contactPtr),
    new b2ContactImpulse(impulsePtr));
};

b2World.QueryAABB = function(fixturePtr) {
  return world.queryAABBCallback.ReportFixture(world.fixturesLookup[fixturePtr]);
};

b2World.RayCast = function(fixturePtr, pointX, pointY, normalX, normalY, fraction) {
  return world.rayCastCallback.ReportFixture(world.fixturesLookup[fixturePtr],
    new b2Vec2(pointX, pointY), new b2Vec2(normalX, normalY), fraction);
};

// Emscripten exports
var b2World_Create = Module.cwrap('b2World_Create', 'number', ['number', 'number']);
var b2World_CreateBody =
  Module.cwrap('b2World_CreateBody', 'number',
    ['number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number']);

var b2World_CreateParticleSystem =
  Module.cwrap('b2World_CreateParticleSystem', 'number',
    ['number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number', 'number',
     'number', 'number', 'number']);

var b2World_DestroyBody =
  Module.cwrap('b2World_DestroyBody', 'null', ['number', 'number']);

var b2World_DestroyJoint =
  Module.cwrap('b2World_DestroyJoint', 'null', ['number', 'number']);

var b2World_DestroyParticleSystem =
  Module.cwrap('b2World_DestroyParticleSystem', 'null', ['number', 'number']);

var b2World_QueryAABB =
  Module.cwrap('b2World_QueryAABB', 'null',
    ['number', 'number', 'number' ,'number' ,'number']);

var b2World_RayCast =
  Module.cwrap('b2World_RayCast', 'null',
    ['number', 'number', 'number' ,'number' ,'number']);

var b2World_SetContactListener = Module.cwrap('b2World_SetContactListener', 'null', ['number']);
var b2World_SetGravity = Module.cwrap('b2World_SetGravity', 'null',
  ['number', 'number', 'number']);
var b2World_Step = Module.cwrap('b2World_Step', 'null', ['number', 'number', 'number']);

var _transBuf = null;
var _vec2Buf = null;

// Todo move the buffers to native access
/** @constructor */
function b2World(gravity) {
  this.bodies = [];
  this.bodiesLookup = {};
  this.fixturesLookup = {};
  this.joints = [];
  this.listener = null;
  this.particleSystems = [];
  this.ptr = b2World_Create(gravity.x, gravity.y);
  this.queryAABBCallback = null;
  this.rayCastCallback = null;

  this.buffer = new DataView(Module.HEAPU8.buffer, this.ptr);

  // preallocate some buffers to prevent having to constantly reuse
  var nDataBytes = 4 * Float32Array.BYTES_PER_ELEMENT;
  var dataPtr = Module._malloc(nDataBytes);
  _transBuf = new Uint8Array(Module.HEAPU8.buffer, dataPtr, nDataBytes);

  nDataBytes = 2 * Float32Array.BYTES_PER_ELEMENT;
  dataPtr = Module._malloc(nDataBytes);
  _vec2Buf = new Uint8Array(Module.HEAPU8.buffer, dataPtr, nDataBytes);
}

b2World._Push = function(item, list) {
  item.lindex = list.length;
  list.push(item);
};

b2World._RemoveItem = function(item, list) {
  var length = list.length;
  var lindex = item.lindex;
  if (length > 1) {
    list[lindex] = list[length - 1];
    list[lindex].lindex = lindex;
  }
  list.pop();
};

b2World.prototype.CreateBody = function(bodyDef) {
  var body = new b2Body(b2World_CreateBody(
    this.ptr, bodyDef.active, bodyDef.allowSleep,
    bodyDef.angle, bodyDef.angularVelocity, bodyDef.angularDamping,
    bodyDef.awake, bodyDef.bullet, bodyDef.fixedRotation,
    bodyDef.gravityScale, bodyDef.linearDamping, bodyDef.linearVelocity.x,
    bodyDef.linearVelocity.y, bodyDef.position.x, bodyDef.position.y,
    bodyDef.type, bodyDef.userData));
  b2World._Push(body, this.bodies);
  this.bodiesLookup[body.ptr] = body;
  return body;
};

b2World.prototype.CreateJoint = function(jointDef) {
  var joint = jointDef.Create(this);
  b2World._Push(joint, this.joints);
  return joint;
};

b2World.prototype.CreateParticleSystem = function(psd) {
  var ps = new b2ParticleSystem(b2World_CreateParticleSystem(
    this.ptr, psd.colorMixingStrength,
    psd.dampingStrength, psd.destroyByAge, psd.ejectionStrength,
    psd.elasticStrength, psd.lifetimeGranularity, psd.powderStrength,
    psd.pressureStrength, psd.radius, psd.repulsiveStrength,
    psd.springStrength, psd.staticPressureIterations, psd.staticPressureRelaxation,
    psd.staticPressureStrength, psd.surfaceTensionNormalStrength, psd.surfaceTensionPressureStrength,
    psd.viscousStrength));
  b2World._Push(ps, this.particleSystems);
  ps.dampingStrength = psd.dampingStrength;
  ps.radius = psd.radius;
  return ps;
};

b2World.prototype.DestroyBody = function(body) {
  b2World_DestroyBody(this.ptr, body.ptr);
  b2World._RemoveItem(body, this.bodies);
};

b2World.prototype.DestroyJoint = function(joint) {
  b2World_DestroyJoint(this.ptr, joint.ptr);
  b2World._RemoveItem(joint, this.joints);
};

b2World.prototype.DestroyParticleSystem = function(particleSystem) {
  b2World_DestroyParticleSystem(this.ptr, particleSystem.ptr);
  b2World._RemoveItem(particleSystem, this.particleSystems);
};

b2World.prototype.QueryAABB = function(callback, aabb) {
  this.queryAABBCallback = callback;
  b2World_QueryAABB(this.ptr, aabb.lowerBound.x, aabb.lowerBound.y,
    aabb.upperBound.x, aabb.upperBound.y);
};

b2World.prototype.RayCast = function(callback, point1, point2) {
  this.rayCastCallback = callback;
  b2World_RayCast(this.ptr, point1.x, point1.y, point2.x, point2.y);
};

b2World.prototype.SetContactListener = function(listener) {
  this.listener = listener;
  b2World_SetContactListener(this.ptr);
};

b2World.prototype.SetGravity = function(gravity) {
  b2World_SetGravity(this.ptr, gravity.x, gravity.y);
};

b2World.prototype.Step = function(steps, vIterations, pIterations) {
  b2World_Step(this.ptr, steps, vIterations, pIterations);
};
