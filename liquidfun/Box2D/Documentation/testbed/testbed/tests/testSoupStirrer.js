function TestSoupStirrer() {
  camera.position.y = 2;
  camera.position.z = 3;

  // not real inheritance but it will do
  var parent = new TestSoup();
  this.oscillationOffset = 0;

  this.particleSystem = world.particleSystems[0];

  this.particleSystem.SetDamping(1.0);

  // Shape of the stirrer.
  var shape = new b2CircleShape;
  shape.position.Set(0, 0.7);
  shape.radius = 0.4;

  // Create the stirrer.
  var bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  this.stirrer = world.CreateBody(bd);
  this.stirrer.CreateFixtureFromShape(shape, 1.0);

  // Destroy all particles under the stirrer.
  var xf = new b2Transform;
  xf.SetIdentity();
  this.particleSystem.DestroyParticlesInShape(shape, xf);

  this.CreateJoint();
}

TestSoupStirrer.prototype.CreateJoint = function() {
  var prismaticJointDef = new b2PrismaticJointDef();
  prismaticJointDef.bodyA = g_groundBody;
  prismaticJointDef.bodyB = this.stirrer;
  prismaticJointDef.collideConnected = true;
  prismaticJointDef.localAxisA.Set(1,0);
  prismaticJointDef.localAnchorA = this.stirrer.GetPosition();
  this.joint = world.CreateJoint(prismaticJointDef);
};

TestSoupStirrer.prototype.ToggleJoint = function() {
  if (this.joint) {
    world.DestroyJoint(this.joint);
    this.joint = null;
  } else {
    this.CreateJoint();
  }
};

TestSoupStirrer.prototype.Keyboard = function(key) {
  switch (key) {
    case 't':
      this.ToggleJoint();
      break;
  }
};

TestSoupStirrer.prototype.MouseUp = function(p) {
  if (this.InSoup(p)) {
    this.ToggleJoint();
  }
};

/**@return bool*/
TestSoupStirrer.prototype.InSoup = function(pos) {
  return pos.y > -1.0 && pos.y < 2.0 && pos.x > -3.0 && pos.x < 3.0;
};

TestSoupStirrer.prototype.Step = function() {
  // Magnitude of the force applied to the body.
  var k_forceMagnitude = 10.0;
  // How often the force vector rotates.
  var k_forceOscillationPerSecond = 0.2;
  var k_forceOscillationPeriod =
    1.0 / k_forceOscillationPerSecond;
  // Maximum speed of the body.
  var k_maxSpeed = 2.0;

  this.oscillationOffset += (1.0 / 60.0);
  if (this.oscillationOffset > k_forceOscillationPeriod) {
    this.oscillationOffset -= k_forceOscillationPeriod;
  }

  // Calculate the force vector.
  var forceAngle = this.oscillationOffset *
    k_forceOscillationPerSecond * 2.0 * Math.PI;
  var forceVector =
    new b2Vec2(Math.sin(forceAngle), Math.cos(forceAngle));
  b2Vec2.MulScalar(forceVector, forceVector, k_forceMagnitude);

  // Only apply force to the body when it's within the soup.
  if (this.InSoup(this.stirrer.GetPosition()) &&
    this.stirrer.GetLinearVelocity().Length() < k_maxSpeed) {
    this.stirrer.ApplyForceToCenter(forceVector, true);
  }

  Step();
};