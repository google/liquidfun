function TestBridge() {
  var count = 30;

  var bd = new b2BodyDef();
  var ground = world.CreateBody(bd);

  var edge = new b2EdgeShape();
  edge.Set(new b2Vec2(-40, 0), new b2Vec2(40, 0));
  ground.CreateFixtureFromShape(edge, 0);

  var box = new b2PolygonShape();
  box.SetAsBoxXY(0.5, 0.125);

  var fd = new b2FixtureDef();
  fd.shape = box;
  fd.density = 20;
  fd.friction = 0.2;

  var rjoint = new b2RevoluteJointDef();

  var prevBody = ground;
  var anchor;
  for (var i = 0; i < count; i++) {
    bd.type = b2_dynamicBody;
    bd.position.Set(-14.5 + 1 * i, 5);
    var body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);

    anchor = new b2Vec2(-15 + 1 * i, 5);
    rjoint.InitializeAndCreate(prevBody, body, anchor);
    prevBody = body;
  }
  anchor = new b2Vec2(-15 + 1 * count, 5);
  rjoint.InitializeAndCreate(prevBody, ground, anchor);

  for (var i = 0; i < 2; i++) {
    var shape = new b2PolygonShape();
    shape.vertices[0] = new b2Vec2(-0.5, 0);
    shape.vertices[1] = new b2Vec2(0.5, 0);
    shape.vertices[2] = new b2Vec2(0, 1.5);

    fd = new b2FixtureDef();
    fd.shape = shape;
    fd.density = 1;

    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(-8 + 8 * i, 12);
    body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);
  }

  for (var i = 0; i < 3; i++) {
    var circle = new b2CircleShape();
    circle.radius = 0.5;

    fd = new b2FixtureDef();
    fd.shape = circle;
    fd.density = 1;

    bd = new b2BodyDef();
    bd.type = b2_dynamicBody;
    bd.position.Set(-6 + 6 * i, 10);
    body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);
  }

}