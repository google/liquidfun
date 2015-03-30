// TODO If you remove the bodies and then try to remove the joints, bad things happen
function TestWeb() {
  this.bodies = [];
  this.joints = [];
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 0.5);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.shape = shape;
  bd.position.Set(-5.0, 5.0);
  this.bodies[0] = world.CreateBody(bd);
  this.bodies[0].CreateFixtureFromShape(shape, 5.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 0.5);
  bd.shape = shape;
  bd.position.Set(5.0, 5.0);
  this.bodies[1] = world.CreateBody(bd);
  this.bodies[1].CreateFixtureFromShape(shape, 5.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 0.5);
  bd.position.Set(5.0, 15.0);
  this.bodies[2] = world.CreateBody(bd);
  this.bodies[2].CreateFixtureFromShape(shape, 5.0);

  shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.5, 0.5);
  bd.position.Set(-5.0, 15.0);
  this.bodies[3] = world.CreateBody(bd);
  this.bodies[3].CreateFixtureFromShape(shape, 5.0);


  var jd = new b2DistanceJointDef;
  var p1 = new b2Vec2(), p2 = new b2Vec2(), d = new b2Vec2();

  jd.frequencyHz = 2.0;
  jd.dampingRatio = 0.0;

  jd.bodyA = ground;
  jd.bodyB = this.bodies[0];
  jd.localAnchorA.Set(-10.0, 0.0);
  jd.localAnchorB.Set(-0.5, -0.5);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[0] = world.CreateJoint(jd);

  jd.bodyA = ground;
  jd.bodyB = this.bodies[1];
  jd.localAnchorA.Set(10.0, 0.0);
  jd.localAnchorB.Set(0.5, -0.5);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[1] = world.CreateJoint(jd);

  jd.bodyA = ground;
  jd.bodyB = this.bodies[2];
  jd.localAnchorA.Set(10.0, 20.0);
  jd.localAnchorB.Set(0.5, 0.5);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[2] = world.CreateJoint(jd);

  jd.bodyA = ground;
  jd.bodyB = this.bodies[3];
  jd.localAnchorA.Set(-10.0, 20.0);
  jd.localAnchorB.Set(-0.5, 0.5);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[3] = world.CreateJoint(jd);

  jd.bodyA = this.bodies[0];
  jd.bodyB = this.bodies[1];
  jd.localAnchorA.Set(0.5, 0.0);
  jd.localAnchorB.Set(-0.5, 0.0);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[4] = world.CreateJoint(jd);

  jd.bodyA = this.bodies[1];
  jd.bodyB = this.bodies[2];
  jd.localAnchorA.Set(0.0, 0.5);
  jd.localAnchorB.Set(0.0, -0.5);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[5] = world.CreateJoint(jd);

  jd.bodyA = this.bodies[2];
  jd.bodyB = this.bodies[3];
  jd.localAnchorA.Set(-0.5, 0.0);
  jd.localAnchorB.Set(0.5, 0.0);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[6] = world.CreateJoint(jd);

  jd.bodyA = this.bodies[3];
  jd.bodyB = this.bodies[0];
  jd.localAnchorA.Set(0.0, -0.5);
  jd.localAnchorB.Set(0.0, 0.5);
  p1 = jd.bodyA.GetWorldPoint(jd.localAnchorA);
  p2 = jd.bodyB.GetWorldPoint(jd.localAnchorB);
  b2Vec2.Sub(d, p2, p1);
  jd.length = d.Length();
  this.joints[7] = world.CreateJoint(jd);
}

TestWeb.prototype.Keyboard = function(key) {
  if (key === 'b') {
    for (var i = 0; i < 4; i++) {
      if (this.bodies[i] !== null) {
        for (var f = 0, max = this.bodies[i].fixtures.length;
             f < max; f++) {
          // This line is JUST for Three.js
          scene.remove(this.bodies[i].fixtures[f].graphic);
        }
        world.DestroyBody(this.bodies[i]);
        this.bodies[i] = null;
        break;
      }
    }
  }
  if (key === 'j') {
    for (var i = 0; i < 8; i++) {
      if (this.joints[i] !== null) {
        world.DestroyJoint(this.joints[i]);
        this.joints[i] = null;
        break;
      }
    }
  }
};