function TestDumpShell() {
  var g = new b2Vec2(0.000000000000000e+00, -1.000000000000000e+01);
  world.SetGravity(g);

  // body 1
  var bd = new b2BodyDef;
  bd.type = b2_staticBody;
  bd.position.Set(2.587699890136719e-02, 5.515012264251709e+00);
  bd.angle = 0.000000000000000e+00;
  bd.linearVelocity.Set(0.000000000000000e+00, 0.000000000000000e+00);
  bd.angularVelocity = 0.000000000000000e+00;
  bd.linearDamping = 0.000000000000000e+00;
  bd.angularDamping = 0.000000000000000e+00;
  bd.allowSleep = true;
  bd.awake = true;
  bd.fixedRotation = false;
  bd.bullet = false;
  bd.active = true;
  bd.gravityScale = 1.000000000000000e+00;
  var body = world.CreateBody(bd);

  var fd = new b2FixtureDef;
  fd.friction = 2.000000029802322e-01;
  fd.restitution = 0.000000000000000e+00;
  fd.density = 1.000000000000000e+00;
  fd.isSensor = false;
  fd.filter.categoryBits = 1;
  fd.filter.maskBits = 65535;
  fd.filter.groupIndex = 0;
  var shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(7.733039855957031e-01, -1.497260034084320e-01));
  shape.vertices.push(new b2Vec2(-4.487270116806030e-01, 1.138330027461052e-01));
  shape.vertices.push(new b2Vec2(-1.880589962005615e+00, -1.365900039672852e-01));
  shape.vertices.push(new b2Vec2(3.972740173339844e-01, -3.897832870483398e+00));

  fd.shape = shape;

  body.CreateFixtureFromDef(fd);

  // body 2
  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(-3.122138977050781e-02, 7.535382270812988e+00);
  bd.angle = -1.313644275069237e-02;
  bd.linearVelocity.Set(8.230687379837036e-01, 7.775862514972687e-02);
  bd.angularVelocity = 3.705333173274994e-02;
  bd.linearDamping = 0.000000000000000e+00;
  bd.angularDamping = 0.000000000000000e+00;
  bd.allowSleep = true;
  bd.awake = true;
  bd.fixedRotation = false;
  bd.bullet = false;
  bd.active = true;
  bd.gravityScale = 1.000000000000000e+00;
  body = world.CreateBody(bd);


  fd = new b2FixtureDef;
  fd.friction = 5.000000000000000e-01;
  fd.restitution = 0.000000000000000e+00;
  fd.density = 5.000000000000000e+00;
  fd.isSensor = false;
  fd.filter.categoryBits = 1;
  fd.filter.maskBits = 65535;
  fd.filter.groupIndex = 0;
  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(3.473900079727173e+00, -2.009889930486679e-01));
  shape.vertices.push(new b2Vec2(3.457079887390137e+00, 3.694039955735207e-02));
  shape.vertices.push(new b2Vec2(-3.116359949111938e+00, 2.348500071093440e-03));
  shape.vertices.push(new b2Vec2(-3.109960079193115e+00, -3.581250011920929e-01));
  shape.vertices.push(new b2Vec2(-2.590820074081421e+00, -5.472509860992432e-01));
  shape.vertices.push(new b2Vec2(2.819370031356812e+00, -5.402340292930603e-01));

  fd.shape = shape;

  body.CreateFixtureFromDef(fd);

  // body 3
  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(-7.438077926635742e-01, 6.626811981201172e+00);
  bd.angle = -1.884713363647461e+01;
  bd.linearVelocity.Set(1.785794943571091e-01, 3.799796104431152e-07);
  bd.angularVelocity = -5.908820639888290e-06;
  bd.linearDamping = 0.000000000000000e+00;
  bd.angularDamping = 0.000000000000000e+00;
  bd.allowSleep = true;
  bd.awake = true;
  bd.fixedRotation = false;
  bd.bullet = false;
  bd.active = true;
  bd.gravityScale = 1.000000000000000e+00;
  body = world.CreateBody(bd);


  fd = new b2FixtureDef;
  fd.friction = 9.499999880790710e-01;
  fd.restitution = 0.000000000000000e+00;
  fd.density = 1.000000000000000e+01;
  fd.isSensor = false;
  fd.filter.categoryBits = 1;
  fd.filter.maskBits = 65535;
  fd.filter.groupIndex = -3;
  shape = new b2PolygonShape;
  shape.vertices.push(new b2Vec2(1.639146506786346e-01, 4.428443685173988e-02));
  shape.vertices.push(new b2Vec2(-1.639146655797958e-01, 4.428443685173988e-02));
  shape.vertices.push(new b2Vec2(-1.639146655797958e-01, -4.428443312644958e-02));
  shape.vertices.push(new b2Vec2(1.639146357774734e-01, -4.428444057703018e-02));

  fd.shape = shape;

  body.CreateFixtureFromDef(fd);
}