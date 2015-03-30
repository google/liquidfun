function TestBullet() {
  var bd = new b2BodyDef();
  bd.position.Set(0, 0);
  var body = world.CreateBody(bd);

  var edge = new b2EdgeShape();
  edge.Set(new b2Vec2(-10, 0), new b2Vec2(10, 0));
  body.CreateFixtureFromShape(edge, 0);

  var shape = new b2PolygonShape();
  shape.SetAsBoxXYCenterAngle(0.2, 1.0, new b2Vec2(0.5, 1), 0);
  body.CreateFixtureFromShape(shape, 0)

  bd = new b2BodyDef();
  bd.type = b2_dynamicBody;
  bd.position.Set(0, 4);

  var box = new b2PolygonShape();
  box.SetAsBoxXY(2, 0.1);

  this.body = world.CreateBody(bd);
  this.body.CreateFixtureFromShape(box, 1);

  box = new b2PolygonShape();
  box.SetAsBoxXY(0.25, 0.25);

  bd.position.Set(0.20352793, 10);
  bd.bullet = true;

  this.bullet = world.CreateBody(bd);
  this.bullet.CreateFixtureFromShape(box, 100);
  this.bullet.SetLinearVelocity(new b2Vec2(0, -50));
  this.step = 0;
}

TestBullet.prototype.Step = function() {
  Step();
  if (this.step % 60 === 0) {
    this.body.SetTransform(new b2Vec2(0, 4), 0);
    this.body.SetLinearVelocity(new b2Vec2(0, 0));
    this.body.SetAngularVelocity(0);

    var x = RandomFloat(-1.0, 1.0);
    this.bullet.SetTransform(new b2Vec2(x, 10), 0);
    this.bullet.SetLinearVelocity(new b2Vec2(0, -50));
    this.bullet.SetAngularVelocity(0);
  }
  this.step += 1;
}