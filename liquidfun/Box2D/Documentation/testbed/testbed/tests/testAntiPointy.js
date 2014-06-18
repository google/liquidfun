function TestAntiPointy() {
  var bd = new b2BodyDef();
  var ground = world.CreateBody(bd);

  var step = 1.0;
  for (var i = -10; i < 10; i += step) {
    var shape = new b2PolygonShape();
    shape.vertices[0] = new b2Vec2(i, -10);
    shape.vertices[1] = new b2Vec2(i + step, -10),
    shape.vertices[2] = new b2Vec2(0, 15);

    ground.CreateFixtureFromShape(shape, 0);
  }
  for (var i = -10; i < 35; i += step) {
    var shape = new b2PolygonShape();
    shape.vertices[0] = new b2Vec2(-10, i);
    shape.vertices[1] = new b2Vec2(-10, i + step),
    shape.vertices[2] = new b2Vec2(0, 15);

    ground.CreateFixtureFromShape(shape, 0);

    shape = new b2PolygonShape();
    shape.vertices[0] = new b2Vec2(10, i);
    shape.vertices[1] = new b2Vec2(10, i + step),
    shape.vertices[2] = new b2Vec2(0, 15);

    ground.CreateFixtureFromShape(shape, 0);
  }

  this.maxParticlesToCreate = 300;
  var psd = new b2ParticleSystemDef();
  psd.radius = 0.025;
  this.particleSystem = world.CreateParticleSystem(psd);

}

TestAntiPointy.prototype.Step = function() {
  Step();
  if (this.maxParticlesToCreate <= 0) {
    return;
  }
  --this.maxParticlesToCreate;

  var pd = new b2ParticleDef();
  pd.position.Set(RandomFloat(-10, 10), 40);
  pd.velocity.Set(0, -1);
  this.particleSystem.CreateParticle(pd);
}