function TestRamp() {
  var bd = new b2BodyDef();
  var ground = world.CreateBody(bd);

  var xstep = 5.0,
    ystep = 5.0;

  for (var y = 30; y > 0; y -= ystep) {
    var shape = new b2PolygonShape();
    shape.vertices[0] = new b2Vec2(-25, y);
    shape.vertices[1] = new b2Vec2(-25, y - ystep),
    shape.vertices[2] = new b2Vec2(0, 15);

    ground.CreateFixtureFromShape(shape, 0);
  }

  for (var x = -25; x < 25; x += xstep) {
    var shape = new b2PolygonShape();
    shape.vertices[0] = new b2Vec2(x, 0);
    shape.vertices[1] = new b2Vec2(x + xstep, 0),
    shape.vertices[2] = new b2Vec2(0, 15);

    ground.CreateFixtureFromShape(shape, 0);
  }

  var psd = new b2ParticleSystemDef();
  psd.radius = 0.025;
  var particleSystem = world.CreateParticleSystem(psd);

  var circle = new b2CircleShape();
  circle.position.Set(-20, 33);
  circle.radius = 1.5;
  var pd = new b2ParticleGroupDef();
  pd.flags = b2_waterParticle;
  pd.shape = circle;
  particleSystem.CreateParticleGroup(pd);
}