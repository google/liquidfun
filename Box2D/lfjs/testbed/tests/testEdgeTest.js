function TestEdgeTest() {
  camera.position.z = 50;
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var v1 = new b2Vec2(-10.0, 0.0);
  var v2 = new b2Vec2(-7.0, -2.0);
  var v3 = new b2Vec2(-4.0, 0.0);
  var v4 = new b2Vec2(0.0, 0.0);
  var v5 = new b2Vec2(4.0, 0.0);
  var v6 = new b2Vec2(7.0, 2.0);
  var v7 = new b2Vec2(10.0, 0.0);

  var shape = new b2EdgeShape;
  shape.Set(v1, v2);
  shape.hasVertex3 = true;
  shape.vertex3 = v3;
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2EdgeShape;
  shape.Set(v2, v3);
  shape.hasVertex0 = true;
  shape.hasVertex3 = true;
  shape.vertex0 = v1;
  shape.vertex3 = v4;
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2EdgeShape;
  shape.Set(v3, v4);
  shape.hasVertex0 = true;
  shape.hasVertex3 = true;
  shape.vertex0 = v2;
  shape.vertex3 = v5;
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2EdgeShape;
  shape.Set(v4, v5);
  shape.hasVertex0 = true;
  shape.hasVertex3 = true;
  shape.vertex0 = v3;
  shape.vertex3 = v6;
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2EdgeShape;
  shape.Set(v5, v6);
  shape.hasVertex0 = true;
  shape.hasVertex3 = true;
  shape.vertex0 = v4;
  shape.vertex3 = v7;
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2EdgeShape;
  shape.Set(v6, v7);
  shape.hasVertex0 = true;
  shape.vertex0 = v5;
  ground.CreateFixtureFromShape(shape, 0.0);

  // create circle
  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(-0.5, 0.6);
  bd.allowSleep = false;
  var body = world.CreateBody(bd);

  shape = new b2CircleShape;
  shape.radius = 0.5;

  body.CreateFixtureFromShape(shape, 1.0);

  // box
  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(1.0, 0.6);
  bd.allowSleep = false;
  body = world.CreateBody(bd);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 0.5);

  body.CreateFixtureFromShape(shape, 1.0);
}