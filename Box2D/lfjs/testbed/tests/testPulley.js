function TestPulley() {
  var y = 16.0;
  var L = 12.0;
  var a = 1.0;
  var b = 2.0;

  // ground
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var edge = new b2EdgeShape;
  edge.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));

  var circle = new b2CircleShape;
  circle.radius = 2.0;

  circle.position.Set(-10.0, y + b + L);
  ground.CreateFixtureFromShape(circle, 0.0);

  var circle = new b2CircleShape;
  circle.radius = 2.0;

  circle.position.Set(10.0, y + b + L);
  ground.CreateFixtureFromShape(circle, 0.0);


  // create pulley
  var shape = new b2PolygonShape;
  shape.SetAsBoxXY(a, b);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;

  //bd.fixedRotation = true;
  bd.position.Set(-10.0, y);
  var body1 = world.CreateBody(bd);
  body1.CreateFixtureFromShape(shape, 5.0);


  var shape = new b2PolygonShape;
  shape.SetAsBoxXY(a, b);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(10.0, y);
  var body2 = world.CreateBody(bd);
  body2.CreateFixtureFromShape(shape, 5.0);

  var pulleyDef = new b2PulleyJointDef;
  var anchor1 = new b2Vec2(-10.0, y + b);
  var anchor2 = new b2Vec2(10.0, y + b);
  var groundAnchor1 = new b2Vec2(-10.0, y + b + L);
  var groundAnchor2 = new b2Vec2(10.0, y + b + L);
  this.joint1 =
    pulleyDef.InitializeAndCreate(body1, body2, groundAnchor1, groundAnchor2, anchor1, anchor2, 1.5);
}

// todo get lengthA, lnegthB and print them out with ratio