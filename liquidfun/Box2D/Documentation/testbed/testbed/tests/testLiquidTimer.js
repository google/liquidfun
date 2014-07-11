function TestLiquidTimer() {
  camera.position.y = 2;
  camera.position.z = 3;
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2ChainShape;
  shape.vertices.push(new b2Vec2(-2, 0));
  shape.vertices.push(new b2Vec2(2, 0));
  shape.vertices.push(new b2Vec2(2, 4));
  shape.vertices.push(new b2Vec2(-2, 4));
  shape.CreateLoop();
  ground.CreateFixtureFromShape(shape, 0.0);

  var psd = new b2ParticleSystemDef();
  psd.radius = 0.025;
  var particleSystem = world.CreateParticleSystem(psd);

  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(2, 0.4, new b2Vec2(0, 3.6), 0);
  var pd = new b2ParticleGroupDef;
  pd.flags = b2_tensileParticle | b2_viscousParticle;
  pd.shape = shape;
  particleSystem.CreateParticleGroup(pd);


  bd = new b2BodyDef;
  var body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-2, 3.2), new b2Vec2(-1.2, 3.2));
  body.CreateFixtureFromShape(shape, 0.1);

  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-1.1, 3.2), new b2Vec2(2, 3.2));
  body.CreateFixtureFromShape(shape, 0.1);

  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-1.2, 3.2), new b2Vec2(-1.2, 2.8));
  body.CreateFixtureFromShape(shape, 0.1);

  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-1.1, 3.2), new b2Vec2(-1.1, 2.8));
   body.CreateFixtureFromShape(shape, 0.1);

  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-1.6, 2.4), new b2Vec2(0.8, 2));
  body.CreateFixtureFromShape(shape, 0.1);

  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(1.6, 1.6), new b2Vec2(-0.8, 1.2));
  body.CreateFixtureFromShape(shape, 0.1);


  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-1.2, 0.8), new b2Vec2(-1.2, 0));
  body.CreateFixtureFromShape(shape, 0.1);

  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-0.4, 0.8), new b2Vec2(-0.4, 0));
  body.CreateFixtureFromShape(shape, 0.1);


  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(0.4, 0.8), new b2Vec2(0.4, 0));
  body.CreateFixtureFromShape(shape, 0.1);

  bd = new b2BodyDef;
  body = world.CreateBody(bd);
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(1.2, 0.8), new b2Vec2(1.2, 0));
  body.CreateFixtureFromShape(shape, 0.1);
}