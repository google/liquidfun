/* TODO this file is NOT tested and NOT finished and will not be part of the
   first release of liquidfun.js
 */

function ParticleEmitter() {
  this.callback = null;
  this.color = new b2ParticleColor();
  this.emitRate = 1;
  this.emitRemainder = 0;
  this.flags = b2_waterParticle;
  this.group = null;
  this.origin = new b2Vec2();
  this.particleSystem = null;
  this.halfSize = new b2Vec2();
  this.speed = 0;
  this.velocity = new b2Vec2();
}

ParticleEmitter.prototype.SetGroup = function(group) {
  if (this.group) {
    this.group.SetGroupFlags(this.group.GetGroupFlags() & ~b2_particleGroupCanBeEmpty);
  }
  this.group = group;
  if (this.group) {
    this.group.SetGroupFlags(this.group.GetGroupFlags() | b2_particleGroupCanBeEmpty);
  }
};

ParticleEmitter.prototype = {
  get size() {
    var out = new b2Vec2();
    b2Vec2.MulScalar(out, this.halfSize, 2);
    return out;
  },
  set size(size) {
    b2Vec2.MulScalar(this.halfSize, size, 0.5);
  }
};

ParticleEmitter.prototype.Step = function(dt, indices) {
  var numberOfParticlesCreated = 0;

  this.emitRemainder += this.emitRate * dt;

  var pd = new b2ParticleDef;
  pd.color = this.color;
  pd.flags = this.flags;
  pd.group = this.group;

  while (this.emitRemainder > 1.0) {
    this.emitRemainder -= 1.0;

    var angle = RandomFloat(0, 1) * 2 * Math.PI;
    var distance = RandomFloat(0, 1);
    var positionOnUnitCircle = new b2Vec2(Math.sin(angle), Math.cos(angle));

    pd.position.Set(
        this.origin.x + positionOnUnitCircle.x * distance * this.halfSize.x,
        this.origin.y + positionOnUnitCircle.y * distance * this.halfSize.y);

    pd.velocity = this.velocity;

    if (this.speed !== 0) {
      b2Vec2.MulScalar(positionOnUnitCircle, positionOnUnitCircle, this.speed);
      b2Vec2.Add(pd.velocity, pd.velocity, positionOnUnitCircle);
    }

    var index = this.particleSystem.CreateParticle(pd);
    if (this.callback) {
      this.callback.ParticleCreated(this.particleSystem, index);
    }

    if (indices) {
      indices.push(index);
    }
    numberOfParticlesCreated++;
  }
  return numberOfParticlesCreated;
};