function TestApplyForce() {
  world.SetGravity(new b2Vec2(0, 0));

  var restitution = 0.4;

  var bd = new b2BodyDef();
  bd.position.Set(0, 20);
  var ground = world.CreateBody(bd);

  var edge = new b2EdgeShape();

  var sd = new b2FixtureDef();
  sd.shape = edge;
  sd.density = 0;
  sd.restitution = restitution;

  edge.Set(new b2Vec2(-20, -20), new b2Vec2(-20, 20));
  ground.CreateFixtureFromDef(sd);

  edge = new b2EdgeShape();
  sd.shape = edge;
  edge.Set(new b2Vec2(20, -20), new b2Vec2(20, 20));
  ground.CreateFixtureFromDef(sd);

  edge = new b2EdgeShape();
  sd.shape = edge;
  edge.Set(new b2Vec2(-20, 20), new b2Vec2(20, 20));
  ground.CreateFixtureFromDef(sd);

  edge = new b2EdgeShape();
  sd.shape = edge;
  edge.Set(new b2Vec2(-20, -20), new b2Vec2(20, -20));
  ground.CreateFixtureFromDef(sd);

  // poly1
  var xf1 = new b2Transform();
  xf1.q.Set(0.3524 * Math.PI);
  xf1.p = xf1.q.GetXAxis();

  var poly1 = new b2PolygonShape();
  var out = new b2Vec2();
  b2Vec2.Mul(out, xf1, new b2Vec2(-1, 0));
  poly1.vertices[0] = out;

  out = new b2Vec2();
  b2Vec2.Mul(out, xf1, new b2Vec2(1, 0));
  poly1.vertices[1] = out;

  out = new b2Vec2();
  b2Vec2.Mul(out, xf1, new b2Vec2(0, 0.5));
  poly1.vertices[2] = out;

  var sd1 = new b2FixtureDef;
  sd1.shape = poly1;
  sd1.density = 4;

  //poly2

  var xf2 = new b2Transform();
  xf2.q.Set(-0.3524 * Math.PI);
  xf2.p = xf2.q.GetXAxis();
  b2Vec2.MulScalar(xf2.p, xf2.p, -1);

  var poly2 = new b2PolygonShape();
  out = new b2Vec2();
  b2Vec2.Mul(out, xf2, new b2Vec2(-1, 0));
  poly2.vertices[0] = out;

  out = new b2Vec2();
  b2Vec2.Mul(out, xf2, new b2Vec2(1, 0));
  poly2.vertices[1] = out;

  out = new b2Vec2();
  b2Vec2.Mul(out, xf2, new b2Vec2(0, 0.5));
  poly2.vertices[2] = out;

  var sd2 = new b2FixtureDef();
  sd2.shape = poly2;
  sd2.density = 2;

  var bd = new b2BodyDef();
  bd.type = b2_dynamicBody;
  bd.angularDamping = 2;
  bd.linearDamping = 0.5;

  bd.position.Set(0, 2);
  bd.angle = Math.PI;
  bd.allowSleep = false;
  this.body = world.CreateBody(bd);
  this.body.CreateFixtureFromDef(sd1);
  this.body.CreateFixtureFromDef(sd2);

  // create vertical stack
  for (var i = 0; i < 10; i++) {
    var box = new b2PolygonShape();
    box.SetAsBoxXY(0.5, 0.5);
    var fd = new b2FixtureDef();
    fd.shape = box;
    fd.density = 1;
    fd.friction = 0.3;
    var bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(0, 5 + 1.45 * i);
    var body = world.CreateBody(bd);
    body.CreateFixtureFromDef(fd);

    var gravity = 10;
    var I = body.GetInertia();
    var m = body.GetMass();

    var radius = Math.sqrt(2 * I / m);

    // setup friction joint
    var jd = new b2FrictionJointDef;
    jd.bodyA = ground;
    jd.bodyB = body;
    jd.collideConnected = true;
    jd.maxForce = m * gravity;
    jd.maxTorque = m * radius * gravity;
    world.CreateJoint(jd);
  }
}

TestApplyForce.prototype.Keyboard = function(key) {
  switch (key) {
    case 'w':
      var f = this.body.GetWorldVector(new b2Vec2(0, -200));
      var p = this.body.GetWorldPoint(new b2Vec2(0, 2));
      this.body.ApplyForce(f, p, true);
      break;
    case 'a':
      this.body.ApplyTorque(50, true);
      break;
    case 'd':
      this.body.ApplyTorque(-50, true)
      break;
  }
}