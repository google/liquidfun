function TestCantilever() {
  var e_count = 8;
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  // one joint
  shape = new b2PolygonShape();
  shape.SetAsBoxXY(0.5, 0.125);

  var fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 20.0;

  var jd = new b2WeldJointDef;

  var prevBody = ground;
  for (var i = 0; i < e_count; ++i) {
    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(-14.5 + 1.0 * i, 5.0);
    var body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);

    var anchor = new b2Vec2(-15.0 + 1.0 * i, 5.0);
    jd.InitializeAndCreate(prevBody, body, anchor);
    prevBody = body;
  }

  // two joint
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(1.0, 0.125);

  var fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 20.0;

  jd = new b2WeldJointDef;
  jd.frequencyHz = 5.0;
  jd.dampingRatio = 0.7;

  prevBody = ground;
  for (var i = 0; i < 3; ++i) {
    var bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(-14.0 + 2.0 * i, 15.0);
    var body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);

    var anchor = new b2Vec2(-15.0 + 2.0 * i, 15.0);
    jd.InitializeAndCreate(prevBody, body, anchor);
    prevBody = body;
  }

  // three joint
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 0.125);

  var fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 20.0;

  jd = new b2WeldJointDef;

  prevBody = ground;
  for (var i = 0; i < e_count; ++i) {
    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(-4.5 + 1.0 * i, 5.0);
    body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);

    if (i > 0) {
      anchor = new b2Vec2(-5.0 + 1.0 * i, 5.0);
      jd.InitializeAndCreate(prevBody, body, anchor);
    }

    prevBody = body;
  }

  // four joint
  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 0.125);

  fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 20.0;

  jd = new b2WeldJointDef;
  jd.frequencyHz = 8.0;
  jd.dampingRatio = 0.7;

  prevBody = ground;
  for (var i = 0; i < e_count; ++i) {
    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(5.5 + 1.0 * i, 10.0);
    body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);

    if (i > 0) {
      anchor = new b2Vec2(5.0 + 1.0 * i, 10.0);
      jd.InitializeAndCreate(prevBody, body, anchor);
    }

    prevBody = body;
  }

  // triangle
  for (var i = 0; i < 2; ++i) {
    shape = new b2PolygonShape;
    shape.vertices[0] = new b2Vec2(-0.5, 0.0);
    shape.vertices[1] = new b2Vec2(0.5, 0.0);
    shape.vertices[2] = new b2Vec2(0.0, 1.5);

    fd = new b2FixtureDef;
    fd.shape = shape;
    fd.density = 1.0;

    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(-8.0 + 8.0 * i, 12.0);
    body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);
  }

  // circles

  for (var i = 0; i < 2; ++i) {
    shape = new b2CircleShape;
    shape.radius = 0.5;

    fd = new b2FixtureDef;
    fd.shape = shape;
    fd.density = 1.0;

    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(-6.0 + 6.0 * i, 10.0);
    body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);
  }
}