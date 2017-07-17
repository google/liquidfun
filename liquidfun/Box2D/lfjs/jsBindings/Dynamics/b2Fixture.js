/**@constructor*/
function b2Filter() {
  this.categoryBits = 0x0001;
  this.maskBits = 0xFFFF;
  this.groupIndex = 0;
}

// fixture globals
var b2Fixture_isSensor_offset = Offsets.b2Fixture.isSensor;
var b2Fixture_userData_offset = Offsets.b2Fixture.userData;
var b2Fixture_filter_categoryBits_offset = Offsets.b2Fixture.filterCategoryBits;
var b2Fixture_filter_maskBits_offset = Offsets.b2Fixture.filterMaskBits;
var b2Fixture_filter_groupIndex_offset = Offsets.b2Fixture.filterGroupIndex;
/**@constructor*/

function b2Fixture() {
  this.body = null;
  this.buffer = null;
  this.ptr = null;
  this.shape = null;
}

var b2Fixture_TestPoint =
  Module.cwrap('b2Fixture_TestPoint', 'number', ['number', 'number', 'number']);
var b2Fixture_Refilter =
  Module.cwrap('b2Fixture_Refilter', 'null', ['number']);

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

b2Fixture.prototype.SetFilterData = function(filter) {
  this.buffer.setUint16(b2Fixture_filter_categoryBits_offset, filter.categoryBits, true);
  this.buffer.setUint16(b2Fixture_filter_maskBits_offset, filter.maskBits, true);
  this.buffer.setUint16(b2Fixture_filter_groupIndex_offset, filter.groupIndex, true);
  this.Refilter();
};

b2Fixture.prototype.SetSensor = function(flag) {
  this.buffer.setUint32(b2Fixture_isSensor_offset, flag, true);
};

b2Fixture.prototype.Refilter = function() {
  b2Fixture_Refilter(this.ptr);
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
