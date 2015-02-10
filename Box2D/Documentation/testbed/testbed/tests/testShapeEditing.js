function TestShapeEditing() {
 // create ground
  var bd = new b2BodyDef;
  var ground = world.CreateBody(bd);

  var shape = new b2EdgeShape;
  shape.Set(new b2Vec2(-40.0, 0.0), new b2Vec2(40.0, 0.0));
  ground.CreateFixtureFromShape(shape, 0.0);

  bd = new b2BodyDef;
  bd.type = b2_dynamicBody;
  bd.position.Set(0.0, 10.0);
  this.body = world.CreateBody(bd);

  shape = new b2PolygonShape;
  shape.SetAsBoxXYCenterAngle(4.0, 4.0, new b2Vec2(0.0, 0.0), 0.0);
  this.fixture1 = this.body.CreateFixtureFromShape(shape, 10.0);

  this.fixture2 = null;

  this.sensor = false;
}

TestShapeEditing.prototype.Keyboard = function(key) {
  switch (key) {
    case 'c':
      if (this.fixture2 === null) {
        var shape = new b2CircleShape;
        shape.radius = 3;
        shape.position.Set(0.5, -4);
        this.fixture2 = this.body.CreateFixtureFromShape(shape, 10);
        this.body.SetAwake(true);
      }
      break;
    case 'd':
      if (this.fixture2 !== null) {
        this.body.DestroyFixture(this.fixture2);
        //testbed specific
        scene.remove(this.fixture2.graphic);
        this.fixture2 = null;
        this.body.SetAwake(true);
      }
      break;
    case 's':
      if (this.fixture2 !== null) {
        this.sensor = !this.sensor;
        this.fixture2.SetSensor(this.sensor);
      }
      break;
  }
};