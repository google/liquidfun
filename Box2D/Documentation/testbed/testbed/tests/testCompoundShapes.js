function TestCompoundShapes() {
  var bd = new b2BodyDef;
  bd.position.Set(0.0, 0.0);
  var body = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(50.0, 0.0), new b2Vec2(-50.0, 0.0));

  body.CreateFixtureFromShape(shape, 0.0);

  // compound circle shapes
  for (var i = 0; i < 10; ++i) {

    var circle1 = new b2CircleShape;
    circle1.radius = 0.5;
    circle1.position.Set(-0.5, 0.5);

    var circle2 = new b2CircleShape;
    circle2.radius = 0.5;
    circle2.position.Set(0.5, 0.5);

    var x = RandomFloat(-0.1, 0.1);
    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(x + 5.0, 1.05 + 2.5 * i);
    bd.angle = RandomFloat(-Math.PI, Math.PI);
    body = world.CreateBody(bd);
    body.CreateFixtureFromShape(circle1, 2.0);
    body.CreateFixtureFromShape(circle2, 0.0);
  }

  for (i = 0; i < 10; ++i) {
    var polygon1 = new b2PolygonShape;
    polygon1.SetAsBoxXY(0.25, 0.5);

    var polygon2 = new b2PolygonShape;
    polygon2.SetAsBoxXYCenterAngle(0.25, 0.5, new b2Vec2(0.0, -0.5), 0.5 * Math.PI);

    x = RandomFloat(-0.1, 0.1);
    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(x - 5.0, 1.05 + 2.5 * i);
    bd.angle = RandomFloat(-Math.PI, Math.PI);
    body = world.CreateBody(bd);
    body.CreateFixtureFromShape(polygon1, 2.0);
    body.CreateFixtureFromShape(polygon2, 2.0);
  }

  var xf1 = new b2Transform;
  xf1.q.Set(0.3524 * Math.PI);
  xf1.p = xf1.q.GetXAxis();

  var xf2 = new b2Transform;
  xf2.q.Set(-0.3524 * Math.PI);
  xf2.p = xf2.q.GetXAxis();
  b2Vec2.MulScalar(xf2.p, xf2.p, -1);

  for (i = 0; i < 10; ++i) {
    var triangle1 = new b2PolygonShape;
    var vertex = new b2Vec2();
    b2Vec2.Mul(vertex, xf1, new b2Vec2(-1.0, 0.0));
    triangle1.vertices.push(new b2Vec2(vertex.x, vertex.y));
    b2Vec2.Mul(vertex, xf1, new b2Vec2(1.0, 0.0));
    triangle1.vertices.push(new b2Vec2(vertex.x, vertex.y));
    b2Vec2.Mul(vertex, xf1, new b2Vec2(0.0, 0.5));
    triangle1.vertices.push(new b2Vec2(vertex.x, vertex.y));

    var triangle2 = new b2PolygonShape;
    b2Vec2.Mul(vertex, xf2, new b2Vec2(-1.0, 0.0));
    triangle2.vertices.push(new b2Vec2(vertex.x, vertex.y));
    b2Vec2.Mul(vertex, xf2, new b2Vec2(1.0, 0.0));
    triangle2.vertices.push(new b2Vec2(vertex.x, vertex.y));
    b2Vec2.Mul(vertex, xf2, new b2Vec2(0.0, 0.5));
    triangle2.vertices.push(new b2Vec2(vertex.x, vertex.y));


    x = RandomFloat(-0.1, 0.1);
    bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(x, 2.05 + 2.5 * i);
    bd.angle = 0.0;
    body = world.CreateBody(bd);
    body.CreateFixtureFromShape(triangle1, 2.0);
    body.CreateFixtureFromShape(triangle2, 2.0);
  }


  var bottom = new b2PolygonShape;
  bottom.SetAsBoxXY( 1.5, 0.15 );

  var left = new b2PolygonShape;
  left.SetAsBoxXYCenterAngle(0.15, 2.7, new b2Vec2(-1.45, 2.35), 0.2);

  var right = new b2PolygonShape;
  right.SetAsBoxXYCenterAngle(0.15, 2.7, new b2Vec2(1.45, 2.35), -0.2);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set( 0.0, 2.0 );
  body = world.CreateBody(bd);
  body.CreateFixtureFromShape(bottom, 4.0);
  body.CreateFixtureFromShape(left, 4.0);
  body.CreateFixtureFromShape(right, 4.0);
}