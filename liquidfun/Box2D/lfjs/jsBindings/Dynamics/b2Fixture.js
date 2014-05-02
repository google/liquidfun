/**@constructor*/
function b2Filter() {
  this.categoryBits = 0x0001;
  this.groupIndex = 0;
  this.maskBits = 0xFFFF;
}

// fixture globals
var b2Fixture_isSensor_offset = Offsets.b2Fixture.isSensor;
var b2Fixture_userData_offset = Offsets.b2Fixture.userData;
/**@constructor*/

function b2Fixture() {
  this.body = null;
  this.buffer = null;
  this.ptr = null;
  this.shape = null;
}

var b2Fixture_TestPoint =
  Module.cwrap('b2Fixture_TestPoint', 'number', ['number', 'number', 'number']);

b2Fixture.prototype._SetPtr = function(ptr) {
  this.ptr = ptr;
  this.buffer = new DataView(Module.HEAPU8.buffer, ptr);
};

b2Fixture.prototype.FromFixtureDef = function(fixtureDef) {
  this.density = fixtureDef.density;
  this.friction = fixtureDef.friction;
  this.isSensor = fixtureDef.isSensor;
  this.restitution = fixtureDef.restitution;
  this.shape = fixtureDef.shape;
  this.userData = fixtureDef.userData;
  this.vertices = [];
};

b2Fixture.prototype.GetUserData = function() {
  return this.buffer.getUint32(b2Fixture_userData_offset, true);
};

b2Fixture.prototype.SetSensor = function(flag) {
  this.buffer.setUint32(b2Fixture_isSensor_offset, flag, true);
};

b2Fixture.prototype.TestPoint = function(p) {
  return b2Fixture_TestPoint(this.ptr, p.x, p.y);
};

/**@constructor*/
function b2FixtureDef() {
  this.density = 0.0;
  this.friction = 0.2;
  this.isSensor = false;
  this.restitution = 0.0;
  this.shape = null;
  this.userData = null;
  this.filter = new b2Filter();
}
