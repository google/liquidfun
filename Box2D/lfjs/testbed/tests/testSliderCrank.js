function TestSliderCrank() {
  // setup ground
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  var prevBody = ground;

  // Define crank.
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 2.0);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(0.0, 7.0);
  var body = world.CreateBody(bd);
  body.CreateFixtureFromShape(shape, 2.0);

  var rjd = new b2RevoluteJointDef;
  rjd.motorSpeed = Math.PI;
  rjd.maxMotorTorque = 10000.0;
  rjd.enableMotor = true;
  this.joint1 = rjd.InitializeAndCreate(prevBody, body, new b2Vec2(0.0, 5.0));

  prevBody = body;

  // Define follower.
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 4.0);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(0.0, 13.0);
  body = world.CreateBody(bd);
  body.CreateFixtureFromShape(shape, 2.0);

  rjd = new b2RevoluteJointDef;
  rjd.enableMotor = false;
  rjd.InitializeAndCreate(prevBody, body, new b2Vec2(0.0, 9.0));

  prevBody = body;

  // Define piston

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(1.5, 1.5);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.fixedRotation = true;
  bd.position.Set(0.0, 17.0);
  body = world.CreateBody(bd);
  body.CreateFixtureFromShape(shape, 2.0);

  rjd = new b2RevoluteJointDef;
  rjd.InitializeAndCreate(prevBody, body, new b2Vec2(0.0, 17.0));

  var pjd = new b2PrismaticJointDef;

  pjd.maxMotorForce = 1000.0;
  pjd.enableMotor = true;
  this.joint2 =
  pjd.InitializeAndCreate(ground, body, new b2Vec2(0.0, 17.0), new b2Vec2(0.0, 1.0));

  // Create a payload

  shape = new  b2PolygonShape;
  shape.SetAsBoxXY(1.5, 1.5);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(0.0, 23.0);
  body = world.CreateBody(bd);
  body.CreateFixtureFromShape(shape, 2.0);
}

TestSliderCrank.prototype.Keyboard = function(key) {
  switch (key) {
    case 'f':
      this.joint2.EnableMotor(!this.joint2.IsMotorEnabled());
      this.joint2.GetBodyB().SetAwake(true);
      break;
    case 'm':
      this.joint1.EnableMotor(!this.joint1.IsMotorEnabled());
      this.joint1.GetBodyB().SetAwake(true);
      break;
  }
};