function TestDamBreak() {
  camera.position.y = 2;
  camera.position.z = 3;
  var bodyDef = new b2BodyDef();
  var ground = world.CreateBody(bodyDef);

  var chainShape = new b2ChainShape();
  chainShape.vertices.push(new b2Vec2(-2, 0));
  chainShape.vertices.push(new b2Vec2(2, 0));
  chainShape.vertices.push(new b2Vec2(2, 4));
  chainShape.vertices.push(new b2Vec2(-2, 4));

  chainShape.CreateLoop();
  ground.CreateFixtureFromShape(chainShape, 0);

  var shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(0.8, 1, new b2Vec2(-1.2, 1.01), 0);

  var psd = new b2ParticleSystemDef();
  psd.radius = 0.025;
  psd.dampingStrength = 0.2;

  var particleSystem = world.CreateParticleSystem(psd);

  var pd = new b2ParticleGroupDef();
  pd.shape = shape;
  var group = particleSystem.CreateParticleGroup(pd);
}
