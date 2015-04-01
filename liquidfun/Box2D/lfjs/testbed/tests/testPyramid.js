function TestPyramid() {
  var bodyDef = new b2BodyDef();
  var ground = world.CreateBody(bodyDef);

  var edgeShape = new b2EdgeShape();
  edgeShape.Set(new b2Vec2(-40, 0), new b2Vec2(40, 0));
  ground.CreateFixtureFromShape(edgeShape, 0);

  var x = new b2Vec2(-7, 0.75);
  var y = new b2Vec2();
  var dx = new b2Vec2(0.5625, 1.25);
  var dy = new b2Vec2(1.125, 0);

  for (var i = 0; i < 20; i++) {
    y = new b2Vec2(x.x, x.y);
    for (var j = i; j < 20; j++) {
      var bodyDef = new b2BodyDef();
      bodyDef.type = b2_dynamicBody;
      bodyDef.position = y;
      var box = new b2PolygonShape();
      box.SetAsBoxXY(0.5, 0.5);
      bodyDef.shape = box;

      var body = world.CreateBody(bodyDef);
      body.CreateFixtureFromShape(box, 5);
      b2Vec2.Add(y, y, dy);
    }
    b2Vec2.Add(x, x, dx);
  }
}