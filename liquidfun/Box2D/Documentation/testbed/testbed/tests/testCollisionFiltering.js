var k_smallGroup = 1;
var k_largeGroup = -1;

var k_defaultCategory = 0x0001;
var k_triangleCategory = 0x0002;
var k_boxCategory = 0x0004;
var k_circleCategory = 0x0008;

var k_triangleMask = 0xFFFF;
var k_boxMask = 0xFFFF ^ k_triangleCategory;
var k_circleMask = 0xFFFF;

function TestCollisionFiltering() {
  // Ground body
  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));

  var sd = new b2FixtureDef;
  sd.shape = shape;
  sd.friction = 0.3;

  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);
  ground.CreateFixtureFromDef(sd);

  // Small triangle
  var polygon = new b2PolygonShape;
  polygon.vertices.push(new b2Vec2(-1.0, 0.0));
  polygon.vertices.push(new b2Vec2(1.0, 0.0));
  polygon.vertices.push(new b2Vec2(0.0, 2.0));

  var triangleShapeDef = new b2FixtureDef;
  triangleShapeDef.shape = polygon;
  triangleShapeDef.density = 1.0;

  triangleShapeDef.filter.groupIndex = k_smallGroup;
  triangleShapeDef.filter.categoryBits = k_triangleCategory;
  triangleShapeDef.filter.maskBits = k_triangleMask;

  var triangleBodyDef = new b2BodyDef;
  triangleBodyDef.type = b2_dynamicBody;
  triangleBodyDef.position.Set(-5.0, 2.0);

  var body1 = world.CreateBody(triangleBodyDef);
  body1.CreateFixtureFromDef(triangleShapeDef);

  // Large triangle
  polygon = new b2PolygonShape;
  polygon.vertices.push(new b2Vec2(-2.0, 0.0));
  polygon.vertices.push(new b2Vec2(2.0, 0.0));
  polygon.vertices.push(new b2Vec2(0.0, 4.0));
  triangleShapeDef.shape = polygon;
  triangleShapeDef.filter.groupIndex = k_largeGroup;
  triangleBodyDef.position.Set(-5.0, 6.0);
  triangleBodyDef.fixedRotation = true; // look at me!

  var body2 = world.CreateBody(triangleBodyDef);
  body2.CreateFixtureFromDef(triangleShapeDef);

   bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(-5.0, 10.0);
  var body = world.CreateBody(bd);

  var p = new b2PolygonShape;
  p.SetAsBoxXY(0.5, 1.0);
  body.CreateFixtureFromShape(p, 1.0);

  var jd = new b2PrismaticJointDef;
  jd.bodyA = body2;
  jd.bodyB = body;
  jd.enableLimit = true;
  jd.localAnchorA.Set(0.0, 4.0);
  jd.localAnchorB.Set(0, 0);
  jd.localAxisA.Set(0.0, 1.0);
  jd.lowerTranslation = -1.0;
  jd.upperTranslation = 1.0;

  world.CreateJoint(jd);

  // Small box
  polygon = new b2PolygonShape;
  polygon.SetAsBoxXY(1.0, 0.5);
  var boxShapeDef = new b2FixtureDef;
  boxShapeDef.shape = polygon;
  boxShapeDef.density = 1.0;
  boxShapeDef.restitution = 0.1;

  boxShapeDef.filter.groupIndex = k_smallGroup;
  boxShapeDef.filter.categoryBits = k_boxCategory;
  boxShapeDef.filter.maskBits = k_boxMask;

  var boxBodyDef = new b2BodyDef;
  boxBodyDef.type = b2_dynamicBody;
  boxBodyDef.position.Set(0.0, 2.0);

  var body3 = world.CreateBody(boxBodyDef);
  body3.CreateFixtureFromDef(boxShapeDef);

  // Large box
  polygon = new b2PolygonShape;
  polygon.SetAsBoxXY(2.0, 1.0);
  boxShapeDef.shape = polygon;
  boxShapeDef.filter.groupIndex = k_largeGroup;
  boxBodyDef.position.Set(0.0, 6.0);

  var body4 = world.CreateBody(boxBodyDef);
  body4.CreateFixtureFromDef(boxShapeDef);

  // Small circle
  var circle = new b2CircleShape;
  circle.radius = 1.0;

  var circleShapeDef = new b2FixtureDef;
  circleShapeDef.shape = circle;
  circleShapeDef.density = 1.0;

  circleShapeDef.filter.groupIndex = k_smallGroup;
  circleShapeDef.filter.categoryBits = k_circleCategory;
  circleShapeDef.filter.maskBits = k_circleMask;

  var circleBodyDef = new b2BodyDef;
  circleBodyDef.type = b2_dynamicBody;
  circleBodyDef.position.Set(5.0, 2.0);

  var body5 = world.CreateBody(circleBodyDef);
  body5.CreateFixtureFromDef(circleShapeDef);

  // Large circle
  var circle = new b2CircleShape;
  circle.radius = 2.0;
  circleShapeDef.shape = circle;
  circleShapeDef.filter.groupIndex = k_largeGroup;
  circleBodyDef.position.Set(5.0, 6.0);

  var body6 = world.CreateBody(circleBodyDef);
  body6.CreateFixtureFromDef(circleShapeDef);
}
