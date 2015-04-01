// Todo the ray ocassionaly goes through the middle
function TestEdgeShape() {
  camera.position.z = 50;
  this.polygons = [];
  // Ground body
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var x1 = -20.0;
  var y1 = 2.0 * Math.cos(x1 / 10.0 * Math.PI);
  for (var i = 0; i < 80; ++i) {
    var x2 = x1 + 0.5;
    var y2 = 2.0 * Math.cos(x2 / 10.0 * Math.PI);

    var shape = new b2EdgeShape;
    shape.Set(new b2Vec2(x1, y1), new b2Vec2(x2, y2));
    ground.CreateFixtureFromShape(shape, 0.0);

    x1 = x2;
    y1 = y2;
  }

  var s1 = new b2PolygonShape;
  s1.vertices.push(new b2Vec2(-0.5, 0.0));
  s1.vertices.push(new b2Vec2(0.5, 0.0));
  s1.vertices.push(new b2Vec2(0.0, 1.5));
  this.polygons.push(s1);

  var s2 = new b2PolygonShape;
  s2.vertices.push(new b2Vec2(-0.1, 0.0));
  s2.vertices.push(new b2Vec2(0.1, 0.0));
  s2.vertices.push(new b2Vec2(0.0, 1.5));
  this.polygons.push(s2);

  var s3 = new b2PolygonShape;
  var w = 1.0;
  var b = w / (2.0 + Math.sqrt(2.0));
  var s = Math.sqrt(2.0) * b;
  s3.vertices.push(new b2Vec2(0.5 * s, 0.0));
  s3.vertices.push(new b2Vec2(0.5 * w, b));
  s3.vertices.push(new b2Vec2(0.5 * w, b + s));
  s3.vertices.push(new b2Vec2(0.5 * s, w));
  s3.vertices.push(new b2Vec2(-0.5 * s, w));
  s3.vertices.push(new b2Vec2(-0.5 * w, b + s));
  s3.vertices.push(new b2Vec2(-0.5 * w, b));
  s3.vertices.push(new b2Vec2(-0.5 * s, 0.0));
  this.polygons.push(s3);

  var b1 = new b2PolygonShape;
  b1.SetAsBoxXY(0.5, 0.5);
  this.polygons.push(b1);

  this.circle = new b2CircleShape;
  this.circle.radius = 0.5;

  this.bodyIndex = 0;

  this.angle = 0.0;

  this.bodies = [];
  this.maxBodies = 256;
}

TestEdgeShape.prototype.Create = function(index) {
  if (this.bodies[this.bodyIndex] != null) {
    world.DestroyBody(this.bodies[this.bodyIndex]);
    this.bodies[this.bodyIndex] = null;
  }

  var bd = new b2BodyDef;

  var x = RandomFloat(-10.0, 10.0);
  var y = RandomFloat(10.0, 20.0);
  bd.position.Set(x, y);
  bd.angle = RandomFloat(-Math.PI, Math.PI);
  bd.type = b2_dynamicBody;

  if (index == 4) {
    bd.angularDamping = 0.02;
  }

  this.bodies[this.bodyIndex] = world.CreateBody(bd);

  var fd;
  if (index < 4) {
    fd = new b2FixtureDef;
    fd.shape = this.polygons[index];
    fd.friction = 0.3;
    fd.density = 20.0;
    this.bodies[this.bodyIndex].CreateFixtureFromDef(fd);
  } else {
    fd = new b2FixtureDef;
    fd.shape = this.circle;
    fd.friction = 0.3;
    fd.density = 20.0;
    this.bodies[this.bodyIndex].CreateFixtureFromDef(fd);
  }

  this.bodyIndex = (this.bodyIndex + 1) % this.maxBodies;
};

TestEdgeShape.prototype.DestroyBody = function() {
  for (var i = 0, max = this.bodies.length; i < max; i++) {
    if (this.bodies[i] !== null) {
      world.DestroyBody(this.bodies[i]);
      this.bodies[i] = null;
      return;
    }
  }
};

TestEdgeShape.prototype.Keyboard = function(key) {
  switch (key)
  {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
      this.Create(key - '1');
      break;

    case 'd':
      this.DestroyBody();
      break;
  }
};

TestEdgeShape.prototype.Step = function() {
  Step();
  var L = 25.0;
  var point1 = new b2Vec2(0.0, 10.0);
  var d = new b2Vec2(L * Math.cos(this.angle), -L * Math.abs(Math.sin(this.angle)));
  var point2 = new b2Vec2();
  b2Vec2.Add(point2, point1, d);

  var callback = new EdgeShapesCallback;

  world.RayCast(callback, point1, point2);

  if (callback.fixture) {
    renderer.insertLine(point1.x, point1.y, callback.point.x, callback.point.y, 0., 0., 0.);

    var head = new b2Vec2();
    b2Vec2.MulScalar(head, callback.normal, 0.5);
    head.x += callback.point.x;
    head.y += callback.point.y;

    renderer.insertLine(callback.point.x, callback.point.y,
      head.x, head.y, 1, 0.9, 0.9);
  } else {
    renderer.insertLine(point1.x, point1.y, point2.x, point2.y, 0, 0., 0.);
  }
  this.angle += 0.25 * Math.PI / 180.0;
};

function EdgeShapesCallback() {
  this.fixture = null;
}

EdgeShapesCallback.prototype.ReportFixture = function(fixture, point, normal, fraction) {
  this.fixture = fixture;
  this.point = point;
  this.normal = normal;

  return fraction;
};