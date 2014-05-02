var b2ChainShape_CreateFixture =
  Module.cwrap('b2ChainShape_CreateFixture', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // Chain vertices and count
      'number', 'number']);

/**@constructor*/
function b2ChainShape() {
  this.radius = b2_polygonRadius;
  this.vertices = [];
  this.type = b2Shape_Type_e_chain;
}

b2ChainShape.prototype.CreateLoop = function() {
   this.vertices.push(this.vertices[0]);
};

// TODO Optimize this
b2ChainShape.prototype._CreateFixture = function(body, fixtureDef) {
  var vertices = this.vertices;
  var chainLength = vertices.length;
  var dataLength = chainLength * 2;
  var data = new Float32Array(dataLength);

  for (var i = 0, j = 0; i < dataLength; i += 2, j++) {
    data[i] = vertices[j].x;
    data[i+1] = vertices[j].y;
  }

  // Get data byte size, allocate memory on Emscripten heap, and get pointer
  var nDataBytes = data.length * data.BYTES_PER_ELEMENT;
  var dataPtr = Module._malloc(nDataBytes);

  // Copy data to Emscripten heap (directly accessed from Module.HEAPU8)
  var dataHeap = new Uint8Array(Module.HEAPU8.buffer, dataPtr, nDataBytes);
  dataHeap.set(new Uint8Array(data.buffer));

  // Call function and get result
  var fixture = b2ChainShape_CreateFixture(body.ptr,
    // fixture def
    fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
    fixtureDef.restitution, fixtureDef.userData,
    // filter def
    fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
    // vertices and length
    dataHeap.byteOffset, data.length);

  // Free memory
  Module._free(dataHeap.byteOffset);
  return fixture;
};