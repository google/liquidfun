function TestMotorJoint() {
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-20.0, 0.0), new b2Vec2(20.0, 0.0));

  var fd = new b2FixtureDef;
  fd.shape = shape;

  ground.CreateFixtureFromDef(fd);

  // Define motorized body
  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(0.0, 8.0);
  var body = world.CreateBody(bd);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(2.0, 0.5);

  fd = new b2FixtureDef;
  fd.shape = shape;
  fd.friction = 0.6;
  fd.density = 2.0;
  body.CreateFixtureFromDef(fd);

  var mjd = new b2MotorJointDef;
  mjd.maxForce = 1000.0;
  mjd.maxTorque = 1000.0;
  this.joint = mjd.InitializeAndCreate(ground, body);
  this.go = false;
  this.time = 0.0;
}

TestMotorJoint.prototype.Keyboard = function(key) {
  if (key === 's') {
    this.go = !this.go;
  }
};

TestMotorJoint.prototype.Step = function() {
  if (this.go) {
    this.time += 1 / 60;
  }

  var linearOffset = new b2Vec2;
  linearOffset.x = 6 * Math.sin(2 * this.time);
  linearOffset.y = 8 + 4 * Math.sin(this.time);

  var angularOffset = 4 * this.time;

  this.joint.SetLinearOffset(linearOffset);
  this.joint.SetAngularOffset(angularOffset);

  Step();
};