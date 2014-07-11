/// Prevents overlapping or leaking.
var b2_solidParticleGroup = 1 << 0;
/// Keeps its shape.
var b2_rigidParticleGroup = 1 << 1;
/// Won't be destroyed if it gets empty.
var b2_particleGroupCanBeEmpty = 1 << 2;
/// Will be destroyed on next simulation step.
var b2_particleGroupWillBeDestroyed = 1 << 3;
/// Updates depth data on next simulation step.
var b2_particleGroupNeedsUpdateDepth = 1 << 4;
var b2_particleGroupInternalMask =
    b2_particleGroupWillBeDestroyed |
    b2_particleGroupNeedsUpdateDepth;

var b2ParticleGroup_ApplyForce =
  Module.cwrap('b2ParticleGroup_ApplyForce', 'null',
    ['number', 'number', 'number']);
var b2ParticleGroup_ApplyLinearImpulse =
  Module.cwrap('b2ParticleGroup_ApplyLinearImpulse', 'null',
    ['number', 'number', 'number']);
var b2ParticleGroup_DestroyParticles =
  Module.cwrap('b2ParticleGroup_DestroyParticles', 'null',
    ['number', 'number']);
var b2ParticleGroup_GetBufferIndex =
  Module.cwrap('b2ParticleGroup_GetBufferIndex', 'number',
    ['number']);
var b2ParticleGroup_GetParticleCount =
  Module.cwrap('b2ParticleGroup_GetParticleCount', 'number',
    ['number']);

var b2ParticleGroup_groupFlags_offset = Offsets.b2ParticleGroup.groupFlags;

/** @constructor */
function b2ParticleGroup(ptr) {
  this.buffer = new DataView(Module.HEAPU8.buffer, ptr);
  this.ptr = ptr;
}

b2ParticleGroup.prototype.ApplyForce = function(force) {
  b2ParticleGroup_ApplyForce(this.ptr, force.x, force.y);
};

b2ParticleGroup.prototype.ApplyLinearImpulse = function(impulse) {
  b2ParticleGroup_ApplyLinearImpulse(this.ptr, impulse.x, impulse.y);
};

b2ParticleGroup.prototype.DestroyParticles = function(flag) {
  b2ParticleGroup_DestroyParticles(this.ptr, flag);
};

b2ParticleGroup.prototype.GetBufferIndex = function() {
  return b2ParticleGroup_GetBufferIndex(this.ptr);
};

b2ParticleGroup.prototype.GetGroupFlags = function() {
  return this.buffer.getUint32(b2ParticleGroup_groupFlags_offset, true);
};

b2ParticleGroup.prototype.GetParticleCount = function() {
  return b2ParticleGroup_GetParticleCount(this.ptr);
};

b2ParticleGroup.prototype.SetGroupFlags = function(flags) {
  this.buffer.setUint32(b2ParticleGroup_groupFlags_offset, flags, true);
};

/**@constructor*/
function b2ParticleGroupDef() {
  this.angle = 0;
  this.angularVelocity = 0;
  this.color = new b2ParticleColor(0, 0, 0, 0);
  this.flags = 0;
  this.group = new b2ParticleGroup(null);
  this.groupFlags = 0;
  this.lifetime = 0.0;
  this.linearVelocity = new b2Vec2();
  this.position = new b2Vec2();
  this.positionData = null;
  this.particleCount = 0;
  this.shape = null;
  //this.shapeCount = 0;
  //this.shapes = null; // not supported currently
  this.strength = 1;
  this.stride = 0;
  this.userData = null;
}
