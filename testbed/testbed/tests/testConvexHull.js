function TestConvexHull() {
  this.auto = false;
  this.Generate();
}

TestConvexHull.prototype.Generate = function() {
  var lowerBound = new b2Vec2(-8.0, -8.0);
  var upperBound = new b2Vec2(8.0, 8.0);

  this.shape = new b2PolygonShape;
 /* TODO: this code was part of the original test
    and has not been tested.  It was commented out in the
    original as well.

    for (var i = 0; i < b2_maxPolygonVertices; ++i) {
    var x = 10.0 * RandomFloat(-1, 1);
    var y = 10.0 * RandomFloat(-1, 1);

    // Clamp onto a square to help create collinearities.
    // This will stress the convex hull algorithm.
    var v = new b2Vec2(x, y);
    v = b2Clamp(v, lowerBound, upperBound);
    this.shape.vertices.push(v);
  }*/

  this.count = b2_maxPolygonVertices;

  this.shape.vertices.push(new b2Vec2(-1.70082211,  -1.43221712 ));
  this.shape.vertices.push(new b2Vec2( 0.5,         -0.5 ));
  this.shape.vertices.push(new b2Vec2(  0.104992867,  0.97400856 ));
  this.shape.vertices.push(new b2Vec2( -0.658002853,  0.926608086 ));
  this.shape.vertices.push(new b2Vec2( -0.994554818,  0.165337861 ));
  this.count = 5;
};

TestConvexHull.prototype.Keyboard = function(key) {
  switch (key) {
    case 'a':
      this.auto = !this.auto;
      break;

    case 'g':
      this.Generate();
      break;
  }
};

TestConvexHull.prototype.Step = function() {
  Step();

  var t = new b2Transform();
  t.SetIdentity();

  this.shape.draw(t);

  if (!this.shape.Validate()) {
    console.log("IsValid = " + this.shape.Validate());
  }
};
