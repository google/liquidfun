function TestConfined() {
  var rowCount = 10;
  var columnCount = 10;

  // create box
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  // Floor
  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-10.0, 0.0), new b2Vec2(10.0, 0.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  // Left wall
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-10.0, 0.0), new b2Vec2(-10.0, 20.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  // Right wall
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(10.0, 0.0), new b2Vec2(10.0, 20.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  // Roof
  shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-10.0, 20.0), new b2Vec2(10.0, 20.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  world.SetGravity(new b2Vec2(0.0, 0.0));
}

function CreateCircle() {


  var radius = 2.0;
  var shape = new b2CircleShape;
  shape.radius = radius;

  var fd = new b2FixtureDef;
  fd.shape = shape;
  fd.density = 1.0;
  fd.friction = 0.0;

  var p = new b2Vec2(RandomFloat(-8, 2), RandomFloat(8, 18));
  var bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position = p;
  var body = world.CreateBody(bd);

  body.CreateFixtureFromDef(fd);
}

TestConfined.prototype.Keyboard = function(key) {
  switch (key) {
    case "c":
      CreateCircle();
      break;
  }
};
