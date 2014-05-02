var b2EdgeShape_CreateFixture =
  Module.cwrap('b2EdgeShape_CreateFixture', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // edge data
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number']);

/** @constructor */
function b2EdgeShape() {
  this.hasVertex0 = false;
  this.hasVertex3 = false;
  this.vertex0 = new b2Vec2();
  this.vertex1 = new b2Vec2();
  this.vertex2 = new b2Vec2();
  this.vertex3 = new b2Vec2();
  this.type = b2Shape_Type_e_edge;
}

b2EdgeShape.prototype.Set = function(v1, v2) {
  this.vertex1 = v1;
  this.vertex2 = v2;
  this.hasVertex0 = false;
  this.hasVertex3 = false;
};

b2EdgeShape.prototype._CreateFixture = function(body, fixtureDef) {
  return b2EdgeShape_CreateFixture(body.ptr,
    // fixture Def
    fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
    fixtureDef.restitution, fixtureDef.userData,
    // filter def
    fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
    // edge data
    this.hasVertex0, this.hasVertex3,
    this.vertex0.x, this.vertex0.y,
    this.vertex1.x, this.vertex1.y,
    this.vertex2.x, this.vertex2.y,
    this.vertex3.x, this.vertex3.y);
};