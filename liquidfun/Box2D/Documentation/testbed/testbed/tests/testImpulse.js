function TestImpulse() {
  camera.position.y = 2;
  camera.position.z = 3;
  this.boxLeft = -2;
  this.boxRight = 2;
  this.boxBottom = 0;
  this.boxTop = 4;
  this.useLinearImpulse = false;

  // Create the containing box.
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2ChainShape;
  shape.vertices.push(new b2Vec2(this.boxLeft, this.boxBottom));
  shape.vertices.push(new b2Vec2(this.boxRight, this.boxBottom));
  shape.vertices.push(new b2Vec2(this.boxRight,this.boxTop));
  shape.vertices.push(new b2Vec2(this.boxLeft, this.boxTop));
  shape.CreateLoop();
  ground.CreateFixtureFromShape(shape, 0.0);

  var psd = new b2ParticleSystemDef();
  psd.radius = 0.025;
  psd.damping = 0.2;
  this.particleSystem = world.CreateParticleSystem(psd);
  // Create the particles.
  shape = new b2PolygonShape ;
  shape.SetAsBoxXYCenterAngle(0.8, 1.0, new b2Vec2(0.0, 1.01), 0);
  var pd = new b2ParticleGroupDef;
  pd.shape = shape;
  this.group = this.particleSystem.CreateParticleGroup(pd);
}

TestImpulse.prototype.MouseUp = function(p) {
  var isInsideBox = this.boxLeft <= p.x &&
    p.x <= this.boxRight &&
    this.boxBottom <= p.y &&
    p.y <= this.boxTop;

  if (isInsideBox) {
    var boxCenter = new b2Vec2(0.5 * (this.boxLeft + this.boxRight),
      0.5 * (this.boxBottom + this.boxTop));
    var direction = new b2Vec2();
    b2Vec2.Sub(direction, p, boxCenter);
    b2Vec2.Normalize(direction, direction);
    this.ApplyImpulseOrForce(direction);
  }
};

TestImpulse.prototype.Keyboard = function(key) {
  switch (key) {
    case 'l':
      this.useLinearImpulse = true;
      break;

    case 'f':
      this.useLinearImpulse = false;
      break;
  }
};

TestImpulse.prototype.ApplyImpulseOrForce = function(direction) {
  var particleGroup = this.particleSystem.particleGroups[0];
  var numParticles = particleGroup.GetParticleCount();

  if (this.useLinearImpulse) {
    var kImpulseMagnitude = 0.005;
    var impulse = new b2Vec2();
    b2Vec2.MulScalar(impulse, direction, kImpulseMagnitude * numParticles);
    this.group.ApplyLinearImpulse(impulse);
  }
  else {
    var kForceMagnitude = 1.0;
    var force = new b2Vec2();
    b2Vec2.MulScalar(force, direction, kForceMagnitude * numParticles);
    this.group.ApplyForce(force);
  }
};