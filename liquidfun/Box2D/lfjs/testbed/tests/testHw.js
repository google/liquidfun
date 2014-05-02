function TestHW() {
  //world.SetContactListener();

  var groundBodyDef = new b2BodyDef();
  groundBodyDef.position.Set(0.0, -10.0);
  var groundBody = world.CreateBody(groundBodyDef);

  var groundBox = new b2PolygonShape();
  groundBox.SetAsBoxXY(50, 10);
  groundBody.CreateFixtureFromShape(groundBox, 0.0);
  groundBody.detail = "I'm the ground";

  var bodyDef = new b2BodyDef();
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(20, 75);
  var body = world.CreateBody(bodyDef);
  body.detail = "I'm the box";

  var dynamicBox = new b2PolygonShape();
  dynamicBox.SetAsBoxXY(1.0, 1.0);

  var fixtureDef = new b2FixtureDef();
  fixtureDef.shape = dynamicBox;
  fixtureDef.friction = 0.3;
  fixtureDef.density = 1.0;

  body.CreateFixtureFromDef(fixtureDef);
}