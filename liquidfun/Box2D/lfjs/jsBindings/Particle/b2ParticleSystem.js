/**@constructor*/
function b2ParticleSystemDef() {
  // Initialize physical coefficients to the maximum values that
  // maintain numerical stability.
  this.colorMixingStrength = 0.5;
  this.dampingStrength = 1.0;
  this.destroyByAge = true;
  this.ejectionStrength = 0.5;
  this.elasticStrength = 0.25;
  this.lifetimeGranularity = 1.0 / 60.0;
  this.powderStrength = 0.5;
  this.pressureStrength = 0.05;
  this.radius = 1.0;
  this.repulsiveStrength = 1.0;
  this.springStrength = 0.25;
  this.staticPressureIterations = 8;
  this.staticPressureRelaxation = 0.2;
  this.staticPressureStrength = 0.2;
  this.surfaceTensionNormalStrength = 0.2;
  this.surfaceTensionPressureStrength = 0.2;
  this.viscousStrength = 0.25;
}

var b2ParticleSystem_CreateParticle =
  Module.cwrap('b2ParticleSystem_CreateParticle', 'number',
  ['number',
    //particle def
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number', 'number'
  ]);

var b2ParticleSystem_GetColorBuffer =
  Module.cwrap('b2ParticleSystem_GetColorBuffer', 'number', ['number']);

var b2ParticleSystem_GetParticleCount =
  Module.cwrap('b2ParticleSystem_GetParticleCount', 'number', ['number']);

var b2ParticleSystem_GetPositionBuffer =
  Module.cwrap('b2ParticleSystem_GetPositionBuffer', 'number', ['number']);

var b2ParticleSystem_GetVelocityBuffer =
  Module.cwrap('b2ParticleSystem_GetVelocityBuffer', 'number', ['number']);

var b2ParticleSystem_SetDamping =
  Module.cwrap('b2ParticleSystem_SetDamping', 'null', ['number', 'number']);

var b2ParticleSystem_SetDensity =
  Module.cwrap('b2ParticleSystem_SetDensity', 'null', ['number', 'number']);

var b2ParticleSystem_SetRadius =
  Module.cwrap('b2ParticleSystem_SetRadius', 'null', ['number', 'number']);

/** @constructor */
function b2ParticleSystem(ptr) {
  this.dampingStrength = 1.0;
  // is this a sane default for density?
  this.density = 1.0;
  this.ptr = ptr;
  this.particleGroups = [];
  this.radius = 1.0;
}

b2ParticleSystem.prototype.CreateParticle = function(pd) {
  return b2ParticleSystem_CreateParticle(this.ptr,
    pd.color.r, pd.color.g, pd.color.b,
    pd.color.a, pd.flags, pd.group,
    pd.lifetime, pd.position.x, pd.position.y,
    pd.userData, pd.velocity.x, pd.velocity.y);
};

b2ParticleSystem.prototype.CreateParticleGroup = function(pgd) {
  var particleGroup = new b2ParticleGroup(pgd.shape._CreateParticleGroup(this, pgd));
  this.particleGroups.push(particleGroup);
  return particleGroup;
};

b2ParticleSystem.prototype.DestroyParticlesInShape = function(shape, xf) {
  return shape._DestroyParticlesInShape(this, xf);
};

b2ParticleSystem.prototype.GetColorBuffer = function() {
  var count = b2ParticleSystem_GetParticleCount(this.ptr) * 4;
  var offset = b2ParticleSystem_GetColorBuffer(this.ptr);
  return new Uint8Array(Module.HEAPU8.buffer, offset, count);
};

/**@return number*/
b2ParticleSystem.prototype.GetParticleCount = function() {
  return b2ParticleSystem_GetParticleCount(this.ptr) * 2;
};

b2ParticleSystem.prototype.GetPositionBuffer = function() {
  var count = b2ParticleSystem_GetParticleCount(this.ptr) * 2;
  var offset = b2ParticleSystem_GetPositionBuffer(this.ptr);
  return new Float32Array(Module.HEAPU8.buffer, offset, count);
};

b2ParticleSystem.prototype.GetVelocityBuffer = function() {
  var count = b2ParticleSystem_GetParticleCount(this.ptr) * 2;
  var offset = b2ParticleSystem_GetVelocityBuffer(this.ptr);
  return new Float32Array(Module.HEAPU8.buffer, offset, count);
};

b2ParticleSystem.prototype.SetDamping = function(damping) {
  this.dampingStrength = damping;
  b2ParticleSystem_SetDamping(this.ptr, damping);
};

b2ParticleSystem.prototype.SetDensity = function(density) {
  this.density = density;
  b2ParticleSystem_SetDensity(this.ptr, density);
};

b2ParticleSystem.prototype.SetRadius = function(radius) {
  this.radius = radius;
  b2ParticleSystem_SetRadius(this.ptr, radius);
};
