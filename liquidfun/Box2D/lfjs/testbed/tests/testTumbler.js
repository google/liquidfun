function TestTumbler() {
  camera.position.y = 10;
  camera.position.z = 25;

  this.maxCount = 800;

  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);


  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.allowSleep = false;
  bd.position.Set(0.0, 10.0);
  var body = world.CreateBody(bd);

  var shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(0.5, 10.0, new b2Vec2( 10.0, 0.0), 0.0);
  body.CreateFixtureFromShape(shape, 5.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(0.5, 10.0, new b2Vec2(-10.0, 0.0), 0.0);
  body.CreateFixtureFromShape(shape, 5.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(10.0, 0.5, new b2Vec2(0.0, 10.0), 0.0);
  body.CreateFixtureFromShape(shape, 5.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(10.0, 0.5, new b2Vec2(0.0, -10.0), 0.0);
  body.CreateFixtureFromShape(shape, 5.0);

  var jd = new b2RevoluteJointDef;
  jd.bodyA = ground;
  jd.bodyB = body;
  jd.localAnchorA.Set(0.0, 10.0);
  jd.localAnchorB.Set(0.0, 0.0);
  jd.referenceAngle = 0.0;
  jd.motorSpeed = 0.05 * Math.PI;
  jd.maxMotorTorque = 1e8;
  jd.enableMotor = true;
  this.joint = world.CreateJoint(jd);

  this.count = 0;
}

TestTumbler.prototype.Step = function() {
  Step();

  if (this.count < this.maxCount) {
    var bd = new b2BodyDef;
    bd.type = b2_dynamicBody;
    bd.position.Set(0, 10);
    var body = world.CreateBody(bd);

    var shape = new b2PolygonShape;
    shape.SetAsBoxXY(0.125, 0.125);
    body.CreateFixtureFromShape(shape, 1);
    ++this.count;
  }
};