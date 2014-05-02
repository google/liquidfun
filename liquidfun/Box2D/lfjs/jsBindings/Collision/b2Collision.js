/**@constructor*/
function b2AABB() {
  this.lowerBound = new b2Vec2();
  this.upperBound = new b2Vec2();
}

b2AABB.prototype.GetCenter = function() {
  var sum = new b2Vec2();
  b2Vec2.Add(sum, this.lowerBound, this.upperBound);
  b2Vec2.MulScalar(sum, sum, 0.5);
};

// todo use just the pointer and offsets to get this data directly from the heap
var b2Manifold_GetPointCount =
  Module.cwrap('b2Manifold_GetPointCount', 'number', ['number']);

/**@constructor*/
function b2Manifold(ptr) {
  this.ptr = ptr;
}

b2Manifold.prototype.GetPointCount = function() {
  return b2Manifold_GetPointCount(this.ptr);
};

var b2WorldManifold_points_offset = Offsets.b2WorldManifold.points;
/**@constructor*/
function b2WorldManifold(ptr) {
  this.buffer = new DataView(Module.HEAPU8.buffer, ptr);
  this.ptr = ptr;
}

b2WorldManifold.prototype.GetPoint = function(i) {
  var point = new b2Vec2();
  point.x = this.buffer.getFloat32((i * 2) + b2WorldManifold_points_offset, true);
  point.y = this.buffer.getFloat32((i * 2) + 4 + b2WorldManifold_points_offset, true);
  return point;
};