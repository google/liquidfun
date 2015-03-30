// in memory a contact impulse looks like
// float normalImpulse[0]
// float normalImpulse[1]
// float tangentImpulse[0]
// float tangentImpulse[1]
// int count
// TODO update with offsets table
/** @constructor */
function b2ContactImpulse(ptr) {
  this.ptr = ptr;
  this.buffer = new DataView(Module.HEAPU8.buffer, ptr);
}

b2ContactImpulse.prototype.GetNormalImpulse = function(i) {
  return this.buffer.getFloat32(i * 4, true);
};

b2ContactImpulse.prototype.GetTangentImpulse = function(i) {
  return this.buffer.getFloat32(i * 4 + 8, true);
};

b2ContactImpulse.prototype.GetCount = function(i) {
  console.log(this.buffer.getInt32(16, true));
};