function TestChain() {
  var bodyDef = new b2BodyDef();
  var ground = world.CreateBody(bodyDef);

  var edge = new b2EdgeShape();
  edge.Set(new b2Vec2(-40, 0), new b2Vec2(40, 0));
  ground.CreateFixtureFromShape(edge, 0);

  var y = 25;
  var prevBody = ground;
  for (var i = 0; i < 30; i++) {
    var jd = new b2RevoluteJointDef();
    jd.collideConnected = false;
    var box = new b2PolygonShape();
    box.SetAsBoxXY(0.6, 0.125);
    var fixtureDef = new b2FixtureDef();
    fixtureDef.shape = box;
    fixtureDef.density = 20;
    fixtureDef.friction = 0.2;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.5 + i, y);
    var body = world.CreateBody(bodyDef);
    body.CreateFixtureFromDef(fixtureDef);

    var anchor = new b2Vec2(i, y);
    jd.InitializeAndCreate(prevBody, body, anchor);
    prevBody = body;
  }
}