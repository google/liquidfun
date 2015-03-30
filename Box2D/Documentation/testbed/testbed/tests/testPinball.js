function TestPinball() {
  // Ground body
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var loop = new b2ChainShape;
  loop.vertices.push(new b2Vec2(0.0, -2.0));
  loop.vertices.push(new b2Vec2(8.0, 6.0));
  loop.vertices.push(new b2Vec2(8.0, 20.0));
  loop.vertices.push(new b2Vec2(-8.0, 20.0));
  loop.vertices.push(new b2Vec2(-8.0, 6.0));
  loop.CreateLoop();
  var fd = new b2FixtureDef;
  fd.shape = loop;
  fd.density = 0.0;
  ground.CreateFixtureFromDef(fd);

  // Flippers
  var p1 = new b2Vec2(-2.0, 0.0);
  var p2 = new b2Vec2(2.0, 0.0);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;

  bd.position = p1;
  var leftFlipper = world.CreateBody(bd);

  bd.position = p2;
  var rightFlipper = world.CreateBody(bd);

  var box = new b2PolygonShape;
  box.SetAsBoxXY(1.75, 0.1);

  fd = new b2FixtureDef;
  fd.shape = box;
  fd.density = 1.0;

  leftFlipper.CreateFixtureFromDef(fd);

  box = new b2PolygonShape;
  box.SetAsBoxXY(1.75, 0.1);

  fd = new b2FixtureDef;
  fd.shape = box;
  fd.density = 1.0;
  rightFlipper.CreateFixtureFromDef(fd);

  var jd = new b2RevoluteJointDef;
  jd.bodyA = ground;
  jd.enableMotor = true;
  jd.maxMotorTorque = 1000.0;
  jd.enableLimit = true;

  jd.motorSpeed = 0.0;
  jd.localAnchorA = p1;
  jd.bodyB = leftFlipper;
  jd.lowerAngle = -30.0 * Math.PI / 180.0;
  jd.upperAngle = 5.0 * Math.PI / 180.0;
  this.leftJoint = world.CreateJoint(jd);

  jd.motorSpeed = 0.0;
  jd.localAnchorA = p2;
  jd.bodyB = rightFlipper;
  jd.lowerAngle = -5.0 * Math.PI / 180.0;
  jd.upperAngle = 30.0 * Math.PI / 180.0;
  this.rightJoint = world.CreateJoint(jd);


  // Circle character
  var bd = new b2BodyDef;
  bd.position.Set(1.0, 15.0);
  bd.type = b2_dynamicBody;
  bd.bullet = true;

  this.ball = world.CreateBody(bd);

  var shape = new b2CircleShape;
  shape.radius = 0.2;

  fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 1.0;
  this.ball.CreateFixtureFromDef(fd);
  this.button = false;
}

TestPinball.prototype.Keyboard = function(key) {
  switch(key) {
    case 'A':
    case 'a':
      this.button = true;
      break;
  }
};

TestPinball.prototype.KeyboardUp = function(key) {
  switch(key) {
    case 'A':
    case 'a':
      this.button = false;
      break;
  }
};

TestPinball.prototype.Step = function() {
  if (this.button) {
    this.leftJoint.SetMotorSpeed(20);
    this.rightJoint.SetMotorSpeed(-20);
  } else {
    this.leftJoint.SetMotorSpeed(-10);
    this.rightJoint.SetMotorSpeed(10);
  }
  Step();
};