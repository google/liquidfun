function TestSparky() {
  camera.position.y = 20;
  camera.position.z = 30;

  this.maxCircles = 6;
  this.shapeHeightOffset = 7;
  this.shapeOffset = 4.5;
  this.VFXIndex = 0;
  this.maxVFX = 50;
  this.VFX = [];

  for (var i = 0; i < this.maxVFX; i++) {
    this.VFX.push(null);
  }

  this.CreateWalls();
  var psd = new b2ParticleSystemDef();
  psd.radius = 0.25;
  this.particleSystem = world.CreateParticleSystem(psd);

  // Create a list of circles that will spark.
  for (var i = 0; i < this.maxCircles; i++) {
    var bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    // Tag this as a sparkable body.
    bd.userData = 1;
    var body = world.CreateBody(bd);
    var shape = new b2CircleShape;
    shape.position.Set(3.0 * RandomFloat(-1, 1),
    this.shapeHeightOffset + this.shapeOffset * i);
    shape.radius = 2;
    body.CreateFixtureFromShape(shape, 0.5);
  }
  world.SetContactListener(this);
}

TestSparky.prototype.CreateWalls = function() {
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(-40, -10));
  shape.vertices.push(new b2Vec2(40, -10));
  shape.vertices.push(new b2Vec2(40, 0));
  shape.vertices.push(new b2Vec2(-40, 0));
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(-40, 40));
  shape.vertices.push(new b2Vec2(40, 40));
  shape.vertices.push(new b2Vec2(40, 50));
  shape.vertices.push(new b2Vec2(-40, 50));
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(-40, -1));
  shape.vertices.push(new b2Vec2(-20, -1));
  shape.vertices.push(new b2Vec2(-20, 40));
  shape.vertices.push(new b2Vec2(-40, 40));
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(20, -1));
  shape.vertices.push(new b2Vec2(40, -1));
  shape.vertices.push(new b2Vec2(40, 40));
  shape.vertices.push(new b2Vec2(20, 40));
  ground.CreateFixtureFromShape(shape, 0.0);

  // testbed specific
  renderer.updateColorParticles = true;
};

TestSparky.prototype.BeginContactBody = function(contact) {
  if (contact.GetFixtureA().GetUserData() !== null ||
    contact.GetFixtureB().GetUserData() !== null) {
    var worldManifold = contact.GetWorldManifold();
    this.contactPoint = worldManifold.GetPoint(0);
    this.contact = true;
  }
};

TestSparky.prototype.Step = function() {
  Step();

  if (this.contact) {
    this.AddVFX(this.contactPoint);
    this.contact = false;
  }
  for (var i = 0; i < this.maxVFX; i++) {
    var vfx = this.VFX[i];
    if (vfx === null) {
      continue;
    }

    vfx.Step(1/60);
    if (vfx.IsDone()) {
      vfx.Destroy();
      this.VFX[i] = null;
    }
  }
};

TestSparky.prototype.AddVFX = function(p) {
  var vfx = this.VFX[this.VFXIndex];
  if (vfx !== null) {
    vfx.Destroy();
    this.VFX[this.VFXIndex] = null;
  }
  this.VFX[this.VFXIndex] = new ParticleVFX(
    this.particleSystem, p, RandomFloat(1, 2), RandomFloat(10, 20), RandomFloat(0.5, 1));

  if (++this.VFXIndex >= this.maxVFX) {
    this.VFXIndex = 0;
  }
};

function ParticleVFX(ps, origin, size, speed, lifetime) {
  var shape = new b2CircleShape;
  shape.position = origin;
  shape.radius = size;

  var pd = new b2ParticleGroupDef;
  pd.shape = shape;
  this.origColor = new b2ParticleColor(RandomFloat(0, 256),
    RandomFloat(0,256),
    RandomFloat(0,256), 255);
  pd.color = this.origColor;
  pd.flags = b2_powderParticle;
  this.particleSystem = ps;
  this.pg = this.particleSystem.CreateParticleGroup(pd);

  this.initialLifetime = this.remainingLifetime = lifetime;
  this.halfLifetime = this.initialLifetime * 0.5;

  var bufferIndex = this.pg.GetBufferIndex();
  var pos = this.particleSystem.GetPositionBuffer();
  var vel = this.particleSystem.GetVelocityBuffer();
  var count = this.pg.GetParticleCount();

  // the array is  2 times count
  for (var i = bufferIndex * 2; i < (bufferIndex + count) * 2; i += 2) {
    vel[i] = (pos[i] - origin.x) * speed;
    vel[i + 1] = (pos[i + 1] - origin.y) * speed;
  }
}

/**@return number*/
ParticleVFX.prototype.ColorCoeff = function() {
  if (this.remainingLifetime >= this.halfLifetime) {
    return 1;
  }
  return 1 - ((this.halfLifetime - this.remainingLifetime) / this.halfLifetime);
};

ParticleVFX.prototype.Step = function(dt) {
  if (this.remainingLifetime > 0.0) {
    this.remainingLifetime = Math.max(this.remainingLifetime - dt, 0.0);
    var coeff = this.ColorCoeff();

    var colors = this.particleSystem.GetColorBuffer();
    var bufferIndex = this.pg.GetBufferIndex();

    // Set particle colors all at once.
    // 4 bytes per color
    for (var i = bufferIndex * 4;
      i < (bufferIndex + this.pg.GetParticleCount())*4; i += 4) {
      colors[i] *= coeff;
      colors[i + 1] *= coeff;
      colors[i + 2] *= coeff;
    }
  }
};

/**@return bool*/
ParticleVFX.prototype.IsDone = function() {
  return this.remainingLifetime <= 0;
};

ParticleVFX.prototype.Destroy = function() {
  this.pg.DestroyParticles(false);
};