function TestSurfaceTension() {
  camera.position.y = 2;
  camera.position.z = 4;

  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2PolygonShape();
  shape.vertices[0] = new b2Vec2(-4, -1);
  shape.vertices[1] = new b2Vec2(4, -1);
  shape.vertices[2] = new b2Vec2(4, 0);
  shape.vertices[3] = new b2Vec2(-4, 0);
  ground.CreateFixtureFromShape(shape, 0.0);

  var shape = new b2PolygonShape();
  shape.vertices[0] = new b2Vec2(-4, -0.1);
  shape.vertices[1] = new b2Vec2(-2, -0.1);
  shape.vertices[2] = new b2Vec2(-2, 2);
  shape.vertices[3] = new b2Vec2(-4, 2);
  ground.CreateFixtureFromShape(shape, 0.0);

  var shape = new b2PolygonShape();
  shape.vertices[0] = new b2Vec2(2, -0.1);
  shape.vertices[1] = new b2Vec2(4, -0.1);
  shape.vertices[2] = new b2Vec2(4, 2);
  shape.vertices[3] = new b2Vec2(2, 2);
  ground.CreateFixtureFromShape(shape, 0.0);

  var psd = new b2ParticleSystemDef();
  psd.radius = 0.035;
  psd.dampingStrength = 0.2;

  var particleSystem = world.CreateParticleSystem(psd);

  // one group
  var circle = new b2CircleShape();
  circle.position.Set(0, 2);
  circle.radius = 0.5;
  var pgd = new b2ParticleGroupDef();
  pgd.flags = b2_tensileParticle | b2_colorMixingParticle;
  pgd.shape = circle;
  pgd.color.Set(255, 0, 0, 255);
  particleSystem.CreateParticleGroup(pgd);

  // two group
  circle = new b2CircleShape();
  circle.position.Set(-1, 2);
  circle.radius = 0.5;
  pgd = new b2ParticleGroupDef();
  pgd.flags = b2_tensileParticle | b2_colorMixingParticle;
  pgd.shape = circle;
  pgd.color.Set(0, 255, 0, 255);
  particleSystem.CreateParticleGroup(pgd);

  // third group
  var box = new b2PolygonShape();
  var pgd = new b2ParticleGroupDef();
  box.vertices[0] = new b2Vec2(0, 3);
  box.vertices[1] = new b2Vec2(2, 3);
  box.vertices[2] = new b2Vec2(2, 3.5);
  box.vertices[3] = new b2Vec2(0, 3.5);
  pgd.flags = b2_tensileParticle | b2_colorMixingParticle;
  pgd.shape = box;
  pgd.color.Set(0, 0, 255, 255);
  particleSystem.CreateParticleGroup(pgd);


  // circle
  bd = new b2BodyDef()
  var circle = new b2CircleShape();
  bd.type = b2_dynamicBody;
  var body = world.CreateBody(bd);
  circle.position.Set(0, 8);
  circle.radius = 0.5;
  body.CreateFixtureFromShape(circle, 0.5);

  // testbed specific
  renderer.updateColorParticles = true;
}