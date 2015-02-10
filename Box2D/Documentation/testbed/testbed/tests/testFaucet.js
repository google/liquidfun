/* Todo finish this test and verify particle emitter*/
function TestFaucet() {
  this.k_particleLifetimeMin = 30.0;
  this.k_particleLifetimeMax = 50.0;
  this.k_containerHeight = 0.2;
  this.k_containerWidth = 1.0;
  this.k_containerThickness = 0.05;
  this.k_faucetWidth = 0.1;
  this.k_faucetHeight = 15.0;
  this.k_faucetLength = 2.0;
  this.k_spoutWidth = 1.1;
  this.k_spoutLength = 2.0;
  this.k_maxParticleCount = 1000;
  this.k_emitRateChangeFactor = 1.05;
  this.k_emitRateMin = 1.0;
  this.k_emitRateMax = 240.0;

  // Configure particle system parameters.
  var psd = new b2ParticleSystemDef();
  psd.radius = 0.035;
  psd.damping = 0.2;
  psd.destroyByAge = true;
  this.particleSystem = world.CreateParticleSystem(psd);
  this.particleSystem.SetMaxParticleCount(this.k_maxParticleCount);

  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  // Create the container / trough style sink.
  var shape = new b2PolygonShape;
  var height = this.k_containerHeight + this.k_containerThickness;
  shape.SetAsBoxXYCenterAngle(this.k_containerWidth - this.k_containerThickness,
    this.k_containerThickness, new b2Vec2(0.0, 0.0), 0.0);
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape();
  shape.SetAsBoxXYCenterAngle(this.k_containerThickness, height,
    new b2Vec2(-this.k_containerWidth, this.k_containerHeight), 0.0);
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape();
  shape.SetAsBoxXYCenterAngle(this.k_containerThickness, height,
    new b2Vec2(this.k_containerWidth, this.k_containerHeight), 0.0);
  ground.CreateFixtureFromShape(shape, 0.0);

  // Create ground under the container to catch overflow
  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(this.k_containerWidth * 5.0, this.k_containerThickness,
    new b2Vec2(0.0, this.k_containerThickness * -2.0), 0.0);
  ground.CreateFixtureFromShape(shape, 0.0);

  // Create the faucet spout.
  shape = new b2PolygonShape;
  var particleDiameter =
    this.particleSystem.GetRadius() * 2.0;
  var faucetLength = this.k_faucetLength * particleDiameter;
  // Dimensions of the faucet in world units.
  var length = faucetLength * this.k_spoutLength;
  var width = this.k_containerWidth * this.k_faucetWidth *
    this.k_spoutWidth;
  // Height from the bottom of the container.
  height = (this.k_containerHeight * this.k_faucetHeight) +
    (length * 0.5);

  shape.SetAsBoxXYCenterAngle(particleDiameter, length,
    new b2Vec2(-width, height), 0.0);
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape();
  shape.SetAsBoxXYCenterAngle(particleDiameter, length,
    new b2Vec2(width, height), 0.0);
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape();
  shape.SetAsBoxXYCenterAngle(width - particleDiameter, particleDiameter,
    new b2Vec2(0.0, height + length -
    particleDiameter), 0.0);
  ground.CreateFixtureFromShape(shape, 0.0);

  // Initialize the particle emitter
  this.lifetimeRandomizer =
    new ParticleLifetimeRandomizer(this.k_particleLifetimeMin, this.k_particleLifetimeMax);
  this.parameter = b2_waterParticle;
  faucetLength =
    this.particleSystem.GetRadius() * 2.0 * this.k_faucetLength;
  this.emitter = new ParticleEmitter();
  this.emitter.particleSystem = this.particleSystem;
  this.emitter.callback = this.lifetimeRandomizer;
  this.emitter.origin = new b2Vec2(this.k_containerWidth * this.k_faucetWidth,
      this.k_containerHeight * this.k_faucetHeight +
      (faucetLength * 0.5));
  this.emitter.velocity = new b2Vec2(0.0, 0.0);
  this.emitter.size = new b2Vec2(0.0, faucetLength);
  this.emitter.color = new b2ParticleColor(255, 255, 255, 255);
  this.emitter.emitRate = 120.0;
  this.emitter.particleFlags = this.parameter;
}

TestFaucet.prototype.Step = function() {
  var dt = 1.0 / 60;
  this.particleColorOffset += dt;
  // Keep m_particleColorOffset in the range 0.0f..k_ParticleColorsCount.
  if (this.particleColorOffset >= particleColors.length) {
    this.particleColorOffset -= particleColors.length;
  }

  // Propagate the currently selected particle flags.
  this.emitter.particleFlags = this.parameter;

  // If this is a color mixing particle, add some color.
  var color = new b2ParticleColor(255, 255, 255, 255);
  if (this.emitter.particleFlags & b2_colorMixingParticle) {
    // Each second, select a different color.
    this.emitter.color = particleColors[this.particleColorOffset %
      particleColors.length];
  } else {
    this.emitter.color = new b2ParticleColor(255, 255, 255, 255);
  }

  // Create the particles.
  this.emitter.Step(dt, null, 0);

};

TestFaucet.prototype.Keyboard = function() {
  var emitRate;
  switch (key) {
    case 'w':
      this.parameter = b2_waterParticle;
      break;
    case 'q':
      this.parameter = b2_powderParticle;
      break;
    case 't':
      this.parameter = b2_tensileParticle;
      break;
    case 'v':
      this.parameter = b2_viscousParticle;
      break;
    case 'c':
      this.parameter = b2_colorMixingParticle;
      break;
    case 's':
      this.parameter = b2_staticPressureParticle;
      break;
    case '+': {
      emitRate = this.emitter.emitRate;
      emitRate *= this.k_emitRateChangeFactor;
      emitRate = Math.max(emitRate, this.k_emitRateMin);
      this.emitter.emitRate = emitRate;
      break;
    }
    case '-': {
      emitRate = this.emitter.emitRate;
      emitRate *= 1.0 / this.k_emitRateChangeFactor;
      emitRate = Math.min(emitRate, this.k_emitRateMax);
      this.emitter.emitRate = emitRate;
    }
      break;
    default:
      // Nothing.
      return;
  }
};

function ParticleLifetimeRandomizer(minLifetime, maxLifetime) {
  this.minLifetime = minLifetime;
  this.maxLifetime = maxLifetime;
}

ParticleLifetimeRandomizer.ParticleCreated = function(system, index) {
  system.SetParticleLifetime(index, RandomFloat(this.minLifetime, this.maxLifetime));
};