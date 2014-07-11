function TestSoup() {
  camera.position.y = 2;
  camera.position.z = 3;
  // create groupd
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(-4, -1));
  shape.vertices.push(new b2Vec2(4, -1));
  shape.vertices.push(new b2Vec2(4, 0));
  shape.vertices.push(new b2Vec2(-4, 0));
  ground.CreateFixtureFromShape(shape, 0.0);


  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(-4, -0.1));
  shape.vertices.push(new b2Vec2(-2, -0.1));
  shape.vertices.push(new b2Vec2(-2, 2));
  shape.vertices.push(new b2Vec2(-4, 3));
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(2, -0.1));
  shape.vertices.push(new b2Vec2(4, -0.1));
  shape.vertices.push(new b2Vec2(4, 3));
  shape.vertices.push(new b2Vec2(2, 2));
  ground.CreateFixtureFromShape(shape, 0.0);

  var psd = new b2ParticleSystemDef();
  psd.radius = 0.035;
  var particleSystem = world.CreateParticleSystem(psd);

  // create particles
  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(2, 1, new b2Vec2(0, 1), 0);
  var pd = new b2ParticleGroupDef;
  pd.shape = shape;
  var group = particleSystem.CreateParticleGroup(pd);

  // create soup innards
  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  var body = world.CreateBody(bd);
  shape = new b2CircleShape;
  shape.position.Set(0, 0.5);
  shape.radius = 0.1;
  body.CreateFixtureFromShape(shape, 0.1);
  var amount = particleSystem.DestroyParticlesInShape(shape,
      body.GetTransform());

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  body = world.CreateBody(bd);
  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(0.1, 0.1, new b2Vec2(-1, 0.5), 0);
  body.CreateFixtureFromShape(shape, 0.1);
  particleSystem.DestroyParticlesInShape(shape,
      body.GetTransform());

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  body = world.CreateBody(bd);
  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(0.1, 0.1, new b2Vec2(1, 0.5), 0.5);
  body.CreateFixtureFromShape(shape, 0.1);
  particleSystem.DestroyParticlesInShape(shape,
      body.GetTransform());

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(0, 2), new b2Vec2(0.1, 2.1));
  body.CreateFixtureFromShape(shape, 1);
  var t = new b2Vec2();
  b2Vec2.Add(t, shape.vertex1, shape.vertex2);
  b2Vec2.MulScalar(t, t, 0.5);
  var massData = new b2MassData(0.1, t, 0.0);
  body.SetMassData(massData);


  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(0.3, 2.0), new b2Vec2(0.4, 2.1));
  body.CreateFixtureFromShape(shape, 1);
  t = new b2Vec2();
  b2Vec2.Add(t, shape.vertex1, shape.vertex2);
  b2Vec2.MulScalar(t, t, 0.5);
  massData = new b2MassData(0.1, t, 0.0);
  body.SetMassData(massData);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-0.3, 2.1), new b2Vec2(-0.2, 2.0));
  body.CreateFixtureFromShape(shape, 1);
  t = new b2Vec2();
  b2Vec2.Add(t, shape.vertex1, shape.vertex2);
  b2Vec2.MulScalar(t, t, 0.5);
  massData = new b2MassData(0.1, t, 0.0);
  body.SetMassData(massData);
}