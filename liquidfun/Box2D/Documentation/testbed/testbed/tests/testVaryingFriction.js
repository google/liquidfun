function TestVaryingFriction() {
  var bd = new b2BodyDef();
  var ground = world.CreateBody(bd);

  var edge = new b2EdgeShape();
  edge.Set(new b2Vec2(-40, 0), new b2Vec2(40, 0));
  ground.CreateFixtureFromShape(edge, 0);

  // s1
  var shape = new b2PolygonShape();
  shape.SetAsBoxXY(13, 0.25);

  bd.position.Set(-4, 22);
  bd.angle = -0.25;

  ground = world.CreateBody(bd);
  ground.CreateFixtureFromShape(shape, 0);

  // s1
  shape = new b2PolygonShape();
  shape.SetAsBoxXY(0.25, 0.1);

  bd.position.Set(10.5, 19);
  bd.angle = -0.25;

  ground = world.CreateBody(bd);
  ground.CreateFixtureFromShape(shape, 0);

  // s3
  shape = new b2PolygonShape();
  shape.SetAsBoxXY(13, 0.25);

  bd.position.Set(4, 14);
  bd.angle = 0.25;

  ground = world.CreateBody(bd);
  ground.CreateFixtureFromShape(shape, 0);

  // s4
  shape = new b2PolygonShape();
  shape.SetAsBoxXY(0.25, 0.1);

  bd.position.Set(-10.5, 11);
  bd.angle = 0;

  ground = world.CreateBody(bd);
  ground.CreateFixtureFromShape(shape, 0);

  // s5
  shape = new b2PolygonShape();
  shape.SetAsBoxXY(13, 0.25);

  bd.position.Set(-4, 6);
  bd.angle = -0.25;

  ground = world.CreateBody(bd);
  ground.CreateFixtureFromShape(shape, 0);

  shape = new b2PolygonShape();
  shape.SetAsBoxXY(0.5, 0.5);

  var fd = new b2FixtureDef();
  fd.shape = shape;
  fd.density = 25;

  var friction = [0.75, 0.5, 0.35, 0.1, 0];

  for (var i = 0; i < 5; i++) {
    bd.type = b2_dynamicBody;
    bd.position.Set(-15 + 4 * i, 28);
    var body = world.CreateBody(bd);

    fd.friction = friction[i];
    body.CreateFixtureFromDef(fd);
  }

}