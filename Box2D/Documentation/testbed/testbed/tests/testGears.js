function TestGears() {
  // setup ground
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(50.0, 0.0), new b2Vec2(-50.0, 0.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  // setup gears
  var circle1 = new b2CircleShape;
  circle1.radius = 1.0;

  var box = new b2PolygonShape;
  box.SetAsBoxXY(0.5, 5.0);

  var circle2 = new b2CircleShape;
  circle2.radius = 2.0;

  var bd1 = new b2BodyDef;
  bd1.type = b2_staticBody;
  bd1.position.Set(10.0, 9.0);
  var body1 = world.CreateBody(bd1);
  body1.CreateFixtureFromShape(circle1, 5.0);

  var bd2 = new b2BodyDef;
  bd2.type = b2_dynamicBody;
  bd2.position.Set(10.0, 8.0);
  var body2 = world.CreateBody(bd2);
  body2.CreateFixtureFromShape(box, 5.0);

  var bd3 = new b2BodyDef;
  bd3.type = b2_dynamicBody;
  bd3.position.Set(10.0, 6.0);
  var body3 = world.CreateBody(bd3);
  body3.CreateFixtureFromShape(circle2, 5.0);

  var jd1 = new b2RevoluteJointDef;
  var joint1 = jd1.InitializeAndCreate(body2, body1, bd1.position);

  var jd2 = new b2RevoluteJointDef;
  var joint2 = jd2.InitializeAndCreate(body2, body3, bd3.position);

  jd4 = new b2GearJointDef;
  jd4.bodyA = body1;
  jd4.bodyB = body3;
  jd4.joint1 = joint1;
  jd4.joint2 = joint2;
  jd4.ratio = circle2.radius / circle1.radius;
  world.CreateJoint(jd4);

  // gear 2
  circle1 = new b2CircleShape;
  circle1.radius = 1.0;

  circle2 = new b2CircleShape;
  circle2.radius = 2.0;

  box = new b2PolygonShape;
  box.SetAsBoxXY(0.5, 5.0);

  bd1 = new b2BodyDef;
  bd1.type = b2_dynamicBody;
  bd1.position.Set(-3.0, 12.0);
  body1 = world.CreateBody(bd1);
  body1.CreateFixtureFromShape(circle1, 5.0);

  jd1 = new b2RevoluteJointDef;
  jd1.bodyA = ground;
  jd1.bodyB = body1;
  jd1.localAnchorA = ground.GetLocalPoint(bd1.position);
  jd1.localAnchorB = body1.GetLocalPoint(bd1.position);
  jd1.referenceAngle = body1.GetAngle() - ground.GetAngle();
  this.joint1 = world.CreateJoint(jd1);

  bd2 = new b2BodyDef;
  bd2.type = b2_dynamicBody;
  bd2.position.Set(0.0, 12.0);
  body2 = world.CreateBody(bd2);
  body2.CreateFixtureFromShape(circle2, 5.0);

  jd2 = new b2RevoluteJointDef;
  this.joint2 = jd2.InitializeAndCreate(ground, body2, bd2.position);

  bd3 = new b2BodyDef;
  bd3.type = b2_dynamicBody;
  bd3.position.Set(2.5, 12.0);
  body3 = world.CreateBody(bd3);
  body3.CreateFixtureFromShape(box, 5.0);

  var jd3 = new b2PrismaticJointDef;
  jd3.lowerTranslation = -5.0;
  jd3.upperTranslation = 5.0;
  jd3.enableLimit = true;

  this.joint3 = jd3.InitializeAndCreate(ground, body3, bd3.position, new b2Vec2(0.0, 1.0));

  var jd4 = new b2GearJointDef;
  jd4.bodyA = body1;
  jd4.bodyB = body2;
  jd4.joint1 = this.joint1;
  jd4.joint2 = this.joint2;
  jd4.ratio = circle2.radius / circle1.radius;
  this.joint4 = world.CreateJoint(jd4);

  var jd5 = new b2GearJointDef;
  jd5.bodyA = body2;
  jd5.bodyB = body3;
  jd5.joint1 = this.joint2;
  jd5.joint2 = this.joint3;
  jd5.ratio = -1.0 / circle2.radius;
  this.joint5 = world.CreateJoint(jd5);
}