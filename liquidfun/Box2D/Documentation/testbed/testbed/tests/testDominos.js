function TestDominos() {
  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));

  var bd = new b2BodyDef;
  var b1 = world.CreateBody(bd);
  b1.CreateFixtureFromShape(shape, 0.0);


  shape = new b2PolygonShape ;
  shape.SetAsBoxXY(6.0, 0.25);

  bd = new b2BodyDef;
  bd.position.Set(-1.5, 10.0);
  var ground = world.CreateBody(bd);
  ground.CreateFixtureFromShape(shape, 0.0);

  for (var i = 0; i < 10; ++i) {
    shape = new b2PolygonShape;
    shape.SetAsBoxXY(0.1, 1.0);

    var fd = new b2FixtureDef;
    fd.shape = shape;
    fd.density = 20.0;
    fd.friction = 0.1;

    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(-6.0 + i, 11.25);
    var body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);
  }

  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(7.0, 0.25, new b2Vec2(), 0.3);

  bd = new b2BodyDef;
  bd.position.Set(1.0, 6.0);
  ground = world.CreateBody(bd);
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.25, 1.5);

  bd = new b2BodyDef;
  bd.position.Set(-7.0, 4.0);
  var b2 = world.CreateBody(bd);
  b2.CreateFixtureFromShape(shape, 0.0);

  //b3
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(6, 0.125);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(-0.9, 1.0);
  bd.angle = -0.15;
  var b3 = world.CreateBody(bd);
  b3.CreateFixtureFromShape(shape, 10.0);

  var jd = new b2RevoluteJointDef;
  anchor = new b2Vec2;

  anchor.Set(-2.0, 1.0);
  jd.collideConnected = true;
  jd.InitializeAndCreate(b1, b3, anchor);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.25, 0.25);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(-10.0, 15.0);
  var b4 = world.CreateBody(bd);
  b4.CreateFixtureFromShape(shape, 10.0);


  anchor.Set(-7.0, 15.0);
  jd.InitializeAndCreate(b2, b4, anchor);

  // create B5
  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(6.5, 3.0);
  var b5 = world.CreateBody(bd);

  shape = new b2PolygonShape;
  fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 10.0;
  fd.friction = 0.1;
  shape.SetAsBoxXYCenterAngle(1.0, 0.1, new b2Vec2(0.0, -0.9), 0.0);
  b5.CreateFixtureFromDef(fd);

  shape = new b2PolygonShape;
  fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 10.0;
  fd.friction = 0.1;
  shape.SetAsBoxXYCenterAngle(0.1, 1.0, new b2Vec2(-0.9, 0.0), 0.0);
  b5.CreateFixtureFromDef(fd);

  shape = new b2PolygonShape;
  fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 10.0;
  fd.friction = 0.1;
  shape.SetAsBoxXYCenterAngle(0.1, 1.0, new b2Vec2(0.9, 0.0), 0.0);
  b5.CreateFixtureFromDef(fd);

  var anchor = new b2Vec2();
  anchor.Set(6.0, 2.0);
  jd.InitializeAndCreate(b1, b5, anchor);

  // b6
  shape = new b2PolygonShape ;
  shape.SetAsBoxXY(1.0, 0.1);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(6.5, 4.1);
  var b6 = world.CreateBody(bd);
  b6.CreateFixtureFromShape(shape, 30.0);

  //b5-b6 joint
  anchor = new b2Vec2();
  anchor.Set(7.5, 4.0);
  jd.InitializeAndCreate(b5, b6, anchor);

  // b7
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.1, 1.0);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(7.4, 1.0);

  var b7 = world.CreateBody(bd);
  b7.CreateFixtureFromShape(shape, 10.0);


  // distance joint b3-b7
  var djd = new b2DistanceJointDef;
  djd.bodyA = b3;
  djd.bodyB = b7;
  djd.localAnchorA.Set(6.0, 0.0);
  djd.localAnchorB.Set(0.0, -1.0);
  var d = new b2Vec2();
  b2Vec2.Sub(d, djd.bodyB.GetWorldPoint(djd.localAnchorB), djd.bodyA.GetWorldPoint(djd.localAnchorA));
  djd.length = d.Length();
  world.CreateJoint(djd);

  // circles
  var radius = 0.2;

  for (var i = 0; i < 4; ++i) {
    var circle = new b2CircleShape;
    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(5.9 + 2.0 * radius * i, 2.4);
    body = world.CreateBody(bd);
    body.CreateFixtureFromShape(circle, 10.0);
  }
}