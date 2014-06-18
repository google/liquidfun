var e_depth = 4;
function TestMobile() {
  // Create ground body.
  var bodyDef = new b2BodyDef;
  bodyDef.position.Set(0.0, 20.0);
  var ground = world.CreateBody(bodyDef);

  var a = 0.5;
  var h = new b2Vec2(0.0, a);

  var root = this.AddNode(ground, new b2Vec2(0, 0), 0, 3.0, a);

  var jointDef = new b2RevoluteJointDef;
  jointDef.bodyA = ground;
  jointDef.bodyB = root;
  jointDef.localAnchorA = new b2Vec2(0, 0);
  jointDef.localAnchorB = h;
  world.CreateJoint(jointDef);
}

TestMobile.prototype.AddNode = function(parent, localAnchor, depth, offset, a) {
  var density = 20.0;
  var h = new b2Vec2(0.0, a);

  var p = parent.GetPosition();
  b2Vec2.Add(p, p, localAnchor);
  b2Vec2.Sub(p, p, h);

  var bodyDef = new b2BodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = p;
  var body = world.CreateBody(bodyDef);

  var shape = new b2PolygonShape;
  shape.SetAsBoxXY(0.25 * a, a);
  body.CreateFixtureFromShape(shape, density);

  if (depth === e_depth) {
    return body;
  }

  var a1 = new b2Vec2(offset, -a);
  var a2 = new b2Vec2(-offset, -a);
  var body1 = this.AddNode(body, a1, depth + 1, 0.5 * offset, a);
  var body2 = this.AddNode(body, a2, depth + 1, 0.5 * offset, a);

  var jointDef = new b2RevoluteJointDef;
  jointDef.bodyA = body;
  jointDef.localAnchorB = h;

  jointDef.localAnchorA = a1;
  jointDef.bodyB = body1;
  world.CreateJoint(jointDef);

  jointDef.localAnchorA = a2;
  jointDef.bodyB = body2;
  world.CreateJoint(jointDef);

  return body;
};