function TestMaxwell() {
  camera.position.z = 7;
  this.k_containerWidth = 2.0;
  this.k_containerHeight = 4.0;
  this.k_containerHalfWidth = this.k_containerWidth / 2.;
  this.k_containerHalfHeight = this.k_containerHeight / 2.0;
  this.k_barrierHeight = this.k_containerHalfHeight / 50.0;
  this.k_barrierMovementIncrement = this.k_containerHalfHeight * 0.1;
  this.k_densityStep = 1.25;
  this.k_densityMin = 0.01;
  this.k_densityMax = 0.8;
  this.k_densityDefault = 0.25;
  this.k_temperatureStep = 0.2;
  this.k_temperatureMin = 0.4;
  this.k_temperatureMax = 10.0;
  this.k_temperatureDefault = 5.0;

  this.density = this.k_densityDefault;
  this.position = this.k_containerHalfHeight;
  this.particleGroup = null;
  this.temperature = this.k_temperatureDefault;
  this.barrierBody = null;

  world.SetGravity(new b2Vec2(0, 0));

  // Create the container
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);
  var shape = new b2ChainShape;
  shape.vertices.push(new b2Vec2(-this.k_containerHalfWidth, 0));
  shape.vertices.push(new b2Vec2(this.k_containerHalfWidth, 0));
  shape.vertices.push(new b2Vec2(this.k_containerHalfWidth, this.k_containerHeight));
  shape.vertices.push(new b2Vec2(-this.k_containerHalfWidth, this.k_containerHeight));
  shape.CreateLoop();
  var def = new b2FixtureDef;
  def.shape = shape;
  def.density = 0;
  def.restitution = 1.0;
  ground.CreateFixtureFromDef(def);

  // enable particles
  var psd = new b2ParticleSystemDef();
  psd.radius = 0.05;
  psd.dampingStrength = 0.2;

  this.particleSystem = world.CreateParticleSystem(psd);

  // Enable the barrier.
  this.EnableBarrier();
  // Create the particles.
  this.ResetParticles();
}

TestMaxwell.prototype.DisableBarrier = function() {
  if (this.barrierBody) {
    world.DestroyBody(this.barrierBody);
    this.barrierBody = null;
  }
};

TestMaxwell.prototype.EnableBarrier = function() {
  if (this.barrierBody === null) {
    var bd = new b2BodyDef;
    this.barrierBody = world.CreateBody(bd);
    var barrierShape = new b2PolygonShape;
    barrierShape.SetAsBoxXYCenterAngle(this.k_containerHalfWidth, this.k_barrierHeight,
      new b2Vec2(0, this.position), 0);
    var def = new b2FixtureDef;
    def.shape = barrierShape;
    def.density = 0;
    def.restitution = 1.0;
    this.barrierBody.CreateFixtureFromDef(def);
  }
};

TestMaxwell.prototype.ToggleBarrier = function() {
  if (this.barrierBody) {
    this.DisableBarrier();
  } else {
    this.EnableBarrier();
  }
};

TestMaxwell.prototype.ResetParticles = function() {
  if (this.particleGroup !== null) {
    this.particleGroup.DestroyParticles(false);
    this.particleGroup = null;
  }

  this.particleSystem.SetRadius(this.k_containerHalfWidth / 20);

  var shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(this.density * this.k_containerHalfWidth,
      this.density * this.k_containerHalfHeight,
    new b2Vec2(0, this.k_containerHalfHeight), 0);
  var pd = new b2ParticleGroupDef;
  pd.flags = b2_powderParticle;
  pd.shape = shape;
  this.particleGroup = this.particleSystem.CreateParticleGroup(pd);

  var vBuffer = this.particleSystem.GetVelocityBuffer();
  var index = this.particleGroup.GetBufferIndex();
  var max = this.particleGroup.GetParticleCount() * 2;

  // the array is 2x count
  for (var i = index * 2; i < max; i += 2) {
    var v = new b2Vec2(RandomFloat(-1, 1) + 1.0, RandomFloat(-1, 1) + 1.0);
    b2Vec2.Normalize(v, v);
    b2Vec2.MulScalar(v, v, this.temperature);

    vBuffer[i] = v.x;
    vBuffer[i + 1] = v.y;
  }
};

TestMaxwell.prototype.Step = function() {
  Step();
  var top = 0;
  var bottom = 0;
  var index = this.particleGroup.GetBufferIndex();
  var velocities =
    this.particleSystem.GetVelocityBuffer();
  var positions =
    this.particleSystem.GetPositionBuffer();

  var max = this.particleGroup.GetParticleCount() * 2;

  for (var i = index * 2; i < max; i += 2) {
    // Add energy to particles based upon the temperature.
    var v = new b2Vec2(velocities[i], velocities[i + 1]);
    b2Vec2.Normalize(v, v);
    b2Vec2.MulScalar(v, v, this.temperature);
    velocities[i] = v.x;
    velocities[i + 1] = v.y;

    // Keep track of the number of particles above / below the
    // divider / barrier position.
    var py = positions[i + 1];
    if (py > this.position)
      top++;
    else
      bottom++;
  }

  // score is the ratio
};

TestMaxwell.prototype.Reset = function() {
  this.DisableBarrier();
  this.ResetParticles();
  this.EnableBarrier();
};

TestMaxwell.prototype.MoveDivider = function(newPosition) {
  this.position = Math.min(Math.max(newPosition, this.k_barrierMovementIncrement),
      this.k_containerHeight - this.k_barrierMovementIncrement);
  this.Reset();
};

TestMaxwell.prototype.Keyboard = function(key) {
  switch(key) {
    case 'a':
      // Enable / disable the barrier.
      this.ToggleBarrier();
      break;
    case '=':
      // Increase the particle density.
      this.density = Math.min(this.density * this.k_densityStep, this.k_densityMax);
      this.Reset();
      break;
    case '-':
      // Reduce the particle density.
      this.density = Math.max(this.density / this.k_densityStep, this.k_densityMin);
      this.Reset();
      break;
    case '.':
      // Move the location of the divider up.
      this.MoveDivider(this.position + this.k_barrierMovementIncrement);
      break;
    case ',':
      // Move the location of the divider down.
      this.MoveDivider(this.position - this.k_barrierMovementIncrement);
      break;
    case ';':
      // Reduce the temperature (velocity of particles).
      this.temperature = Math.max(this.temperature - this.k_temperatureStep,
        this.k_temperatureMin);
      this.Reset();
      break;
    case '\'':
      // Increase the temperature (velocity of particles).
      this.temperature = Math.min(this.temperature + this.k_temperatureStep,
        this.k_temperatureMax);
      this.Reset();
      break;
  }
};

TestMaxwell.prototype.InContainer = function(p) {
  return p.x >= -this.k_containerHalfWidth && p.x <= this.k_containerHalfWidth &&
    p.y >= 0.0 && p.y <= this.k_containerHalfHeight * 2.0;
};

TestMaxwell.prototype.MouseUp = function(p) {
  // If the pointer is in the container.
  if (this.InContainer(p)) {
    // Enable / disable the barrier.
    this.ToggleBarrier();
  }
  else {
    // Move the barrier to the touch position.
    this.MoveDivider(p.y);
  }
};