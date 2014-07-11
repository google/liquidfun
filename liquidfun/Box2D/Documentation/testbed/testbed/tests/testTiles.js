function TestTiles() {
  camera.position.y = 20;
  camera.position.z = 40;

  var count = 20;
  this.fixtureCount = 0;
  var a = 0.5;
  var bd = new b2BodyDef;
  bd.position.y = -a;
  var ground = world.CreateBody(bd);

  var N = 200;
  var M = 10;
  var position = new b2Vec2;
  position.y = 0.0;
  for (var j = 0; j < M; ++j) {
    position.x = -N * a;
    for (var i = 0; i < N; ++i) {
      var shape = new b2PolygonShape;
      shape.SetAsBoxXYCenterAngle(a, a, position, 0.0);
      ground.CreateFixtureFromShape(shape, 0.0);
      ++this.fixtureCount;
      position.x += 2.0 * a;
    }
    position.y -= 2.0 * a;
  }

  var a = 0.5;
  var shape = new b2PolygonShape;
  shape.SetAsBoxXY(a, a);

  var x = new b2Vec2(-7.0, 0.75);
  var y = new b2Vec2;
  var deltaX = new b2Vec2(0.5625, 1.25);
  var deltaY = new b2Vec2(1.125, 0.0);

  for (var i = 0; i < count; ++i) {
    y = x.Clone();

    for (j = i; j < count; ++j) {
      bd = new b2BodyDef;
      bd.type = b2_dynamicBody;
      bd.position = y;

      var body = world.CreateBody(bd);
      body.CreateFixtureFromShape(shape, 5.0);
      ++this.fixtureCount;
      b2Vec2.Add(y, y, deltaY);
    }
    b2Vec2.Add(x, x, deltaX);
  }
}