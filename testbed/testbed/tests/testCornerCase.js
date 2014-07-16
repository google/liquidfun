function TestCornerCase() {
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);
  var shape;

    // Construct a pathological corner intersection out of many
    // polygons to ensure there's no issue with particle oscillation
    // from many fixture contact impulses at the corner

    // left edge
  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(-20.0, 30.0));
  shape.vertices.push(new b2Vec2(-20.0, 0.0));
  shape.vertices.push(new b2Vec2(-25.0, 0.0));
  shape.vertices.push(new b2Vec2(-25.0, 30.));

  ground.CreateFixtureFromShape(shape, 0.0);

  var x, y;
  var yrange=30.0, ystep = yrange/10.0,
    xrange=20.0, xstep=xrange/2.0;


  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(-25.0, 0.0));
  shape.vertices.push(new b2Vec2(20.0, 15.0));
  shape.vertices.push(new b2Vec2(25.0, 0.0));

  ground.CreateFixtureFromShape(shape, 0.0);

  for (x = -xrange; x < xrange; x += xstep) {
    shape = new b2PolygonShape;
    shape.vertices.push(new b2Vec2(-25.0, 0.0));
    shape.vertices.push(new b2Vec2(x, 15.0));
    shape.vertices.push(new b2Vec2(x + xstep, 15.0));
    ground.CreateFixtureFromShape(shape, 0.0);
  }

  for (y = 0.0; y < yrange; y += ystep) {
    shape = new b2PolygonShape;
    shape.vertices.push(new b2Vec2(25.0, y));
    shape.vertices.push(new b2Vec2(25.0, y+ystep));
    shape.vertices.push(new b2Vec2(20.0, 15.0));

    ground.CreateFixtureFromShape(shape, 0.0);
  }

  var psd = new b2ParticleSystemDef();
  psd.radius = 1.0;

  var particleSystem = world.CreateParticleSystem(psd);

  shape = new b2CircleShape;
  shape.position.Set(0, 35);
  shape.radius = 12;
  var pd = new b2ParticleGroupDef;
  pd.shape = shape;
  particleSystem.CreateParticleGroup(pd);
}