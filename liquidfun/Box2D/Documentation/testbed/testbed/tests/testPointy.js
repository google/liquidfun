function TestPointy() {
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  // Construct a triangle out of many polygons to ensure there's no
  // issue with particles falling directly on an ambiguous corner

  var xstep = 1.0;
  for (var x = -10.0; x < 10.0; x += xstep) {
    var shape = new b2PolygonShape;
    shape.vertices.push(new b2Vec2(x, -10));
    shape.vertices.push(new b2Vec2(x+xstep, -10));
    shape.vertices.push(new b2Vec2(0, 25));
    ground.CreateFixtureFromShape(shape, 0.0);
  }

  var psd = new b2ParticleSystemDef();
  psd.radius = 0.25;
  this.particleSystem = world.CreateParticleSystem(psd);

  // Create killfield shape and transform
  this.killfieldShape = new b2PolygonShape;
  this.killfieldShape.SetAsBoxXY(50.0, 1.0);

  // Put this at the bottom of the world
  this.killfieldTransform = new b2Transform(new b2Vec2(-25, 1), 0);
}

TestPointy.prototype.Step = function() {
  Step();

  var pd = new b2ParticleDef;

  pd.position.Set(0.0, 33.0);
  pd.velocity.Set(0.0, -1.0);

  this.particleSystem.CreateParticle(pd);
  this.particleSystem.DestroyParticlesInShape(this.killfieldShape, this.killfieldTransform);
};