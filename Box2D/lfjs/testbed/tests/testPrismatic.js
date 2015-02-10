function TestPrismatic() {
  // create ground
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  // create joint
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(2.0, 0.5);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(-10.0, 10.0);
  bd.angle = 0.5 * Math.PI;
  bd.allowSleep = false;
  var body = world.CreateBody(bd);
  body.CreateFixtureFromShape(shape, 5.0);

  var pjd = new b2PrismaticJointDef;

  // Bouncy limit
  var axis = new b2Vec2(2.0, 1.0);
  b2Vec2.Normalize(axis, axis);
  pjd.motorSpeed = 10.0;
  pjd.maxMotorForce = 10000.0;
  pjd.enableMotor = true;
  pjd.lowerTranslation = 0.0;
  pjd.upperTranslation = 20.0;
  pjd.enableLimit = true;
  this.joint = pjd.InitializeAndCreate(ground, body, new b2Vec2(0.0, 0.0), axis);

  // Non-bouncy limit
  //this.joint = pjd.InitializeAndCreate(ground, body, new b2Vec2(-10.0, 10.0), new b2Vec2(1.0, 0.0));

}

TestPrismatic.prototype.Keyboard = function(key) {
  switch (key) {
    case 'l':
      this.joint.EnableLimit(!this.joint.IsLimitEnabled());
      break;
    case 'm':
      this.joint.EnableMotor(!this.joint.IsMotorEnabled());
      break;
    case 's':
      this.joint.SetMotorSpeed(-this.joint.GetMotorSpeed());
      break;
  }
}