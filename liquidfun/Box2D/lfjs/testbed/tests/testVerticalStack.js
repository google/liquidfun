// todo keyboard controls
function TestVerticalStack() {
  var columnCount = 5;
  var rowCount = 16;

  var bd = new b2BodyDef();
  var ground = world.CreateBody(bd);

  var edge1 = new b2EdgeShape();
  edge1.Set(new b2Vec2(-40, 0), new b2Vec2(40, 0));
  ground.CreateFixtureFromShape(edge1, 0);

  var edge2 = new b2EdgeShape();
  edge2.Set(new b2Vec2(20, 0), new b2Vec2(20, 20));
  ground.CreateFixtureFromShape(edge2, 0);

  var xs = [0, -10, -5, 5, 10];

  for (var j = 0; j < columnCount; j++) {
    var box = new b2PolygonShape();
    box.SetAsBoxXY(0.5, 0.5);

    var fd = new b2FixtureDef();
    fd.shape = box;
    fd.density = 1;
    fd.friction = 0.3;
    for (var i = 0; i < rowCount; i++) {
      bd.type = b2_dynamicBody;

      //var n = j * rowCount + i;
      bd.position.Set(xs[j], 0.752 + 1.54* i);
      var body = world.CreateBody(bd);
      body.CreateFixtureFromDef(fd);
    }
  }


}