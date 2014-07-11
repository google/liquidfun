// TODO this can all be done better, wayyy too manyy calls between asm and js
// a b2contact looks like: (actually this is wrong because of the vtable, I will get a nice one later
/*
uint32 m_flags; // 0
b2Contact* m_prev; // 4
b2Contact* m_next; // 8
b2ContactEdge m_nodeA; // 12 // each of these is 16 bytes, 4 ptrs
b2ContactEdge m_nodeB; // 28
b2Fixture* m_fixtureA; //44
b2Fixture* m_fixtureB; //48
int32 m_indexA;
int32 m_indexB;
b2Manifold m_manifold; a manifold is 20 bytes
int32 m_toiCount;
float32 m_toi;
float32 m_friction;
float32 m_restitution;
float32 m_tangentSpeed;*/

var b2Contact_fixtureA_offset = Offsets.b2Contact.fixtureA;
var b2Contact_fixtureB_offset = Offsets.b2Contact.fixtureB;
var b2Contact_tangentSpeed_offset = Offsets.b2Contact.tangentSpeed;

var b2Contact_GetManifold = Module.cwrap('b2Contact_GetManifold', 'number', ['number']);
var b2Contact_GetWorldManifold = Module.cwrap('b2Contact_GetWorldManifold', 'number', ['number']);
/**@constructor*/
function b2Contact(ptr) {
  this.buffer = new DataView(Module.HEAPU8.buffer, ptr);
  this.ptr = ptr;
}

b2Contact.prototype.GetFixtureA = function() {
  var fixAPtr = this.buffer.getUint32(b2Contact_fixtureA_offset, true);
  return world.fixturesLookup[fixAPtr];
};

b2Contact.prototype.GetFixtureB = function() {
  var fixBPtr = this.buffer.getUint32(b2Contact_fixtureB_offset, true);
  return world.fixturesLookup[fixBPtr];
};

b2Contact.prototype.GetManifold = function() {
  return new b2Manifold(b2Contact_GetManifold(this.ptr));
};

b2Contact.prototype.GetWorldManifold = function() {
  return new b2WorldManifold(b2Contact_GetWorldManifold(this.ptr));
};

b2Contact.prototype.SetTangentSpeed = function(speed) {
  this.buffer.setFloat32(b2Contact_tangentSpeed_offset, speed, true);
};