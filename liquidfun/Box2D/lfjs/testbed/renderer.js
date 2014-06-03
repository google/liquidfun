var inv255 = .003921569;

function Renderer() {
  // init large buffer geometry
  this.maxVertices = 31000;
  var geometry = new THREE.BufferGeometry();
  geometry.dynamic = true;
  geometry.addAttribute('position', Float32Array, this.maxVertices, 3);
  geometry.addAttribute('color', Float32Array, this.maxVertices, 3);
  this.positions = geometry.attributes.position.array;
  this.colors = geometry.attributes.color.array;
  this.currentVertex = 0;
  this.buffer = new THREE.Line(geometry,
    new THREE.LineBasicMaterial({ vertexColors: true }), THREE.LinePieces);

  this.circleVertices = [];
  this.circleResolution = 5;
  this.initCircleVertices(this.circleVertices, this.circleResolution);

  this.particleVertices = [];
  this.particleResolution = 3;
  this.initCircleVertices(this.particleVertices, this.particleResolution);

  scene.add(this.buffer);
}

Renderer.prototype.collapseBuffer = function() {
  var i = this.currentVertex * 3;
  for (; i < this.maxVertices * 3; i++) {
    this.positions[i] = 0;
    this.colors[i] = 0;
  }
};

Renderer.prototype.draw = function() {
  for (var i = 0, max = world.bodies.length; i < max; i++) {
    var body = world.bodies[i];
    var maxFixtures = body.fixtures.length;
    var transform = body.GetTransform();
    for (var j = 0; j < maxFixtures; j++) {
      var fixture = body.fixtures[j];
      fixture.shape.draw(transform);
    }
  }

  // draw particle systems
  for (var i = 0, max = world.particleSystems.length; i < max; i++) {
    drawParticleSystem(world.particleSystems[i]);
  }
  this.collapseBuffer();

  this.buffer.geometry.attributes.position.needsUpdate = true;
  this.buffer.geometry.attributes.color.needsUpdate = true;
};

Renderer.prototype.insertLine = function(x1, y1, x2, y2, r, g, b) {
  var i = this.currentVertex;
  var threeI = i * 3;
  this.positions[threeI] = x1;
  this.positions[threeI + 1] = y1;
  this.positions[threeI + 2] = 0;
  this.colors[threeI] = r;
  this.colors[threeI + 1] = g;
  this.colors[threeI + 2] = b;

  i++;
  threeI = i * 3;
  this.positions[threeI] = x2;
  this.positions[threeI + 1] = y2;
  this.positions[threeI + 2] = 0;
  this.colors[threeI] = r;
  this.colors[threeI + 1] = g;
  this.colors[threeI + 2] = b;
  this.currentVertex += 2;
};

// TODO remove one of the muls
Renderer.prototype.insertCircleVertices = function(transform, radius, x, y, r, g, b) {
  var vertices = this.circleVertices;
  for (var i = 0; i < this.circleResolution; i++) {
    var i4 = i * 4;
    var v1 = new b2Vec2(vertices[i4] * radius + x, vertices[i4 + 1] * radius + y);
    var v2 = new b2Vec2(vertices[i4 + 2] * radius + x, vertices[i4 + 3] * radius + y);

    b2Vec2.Mul(v1, transform, v1);
    b2Vec2.Mul(v2, transform, v2);

    this.insertLine(v1.x, v1.y, v2.x, v2.y, r, g, b);
  }
};

Renderer.prototype.insertParticleVertices = function(radius, x, y, r, g, b) {
  var vertices = this.particleVertices;
  for (var i = 0; i < this.particleResolution; i++) {
    var i4 = i * 4;
    var x1 = vertices[i4] * radius + x;
    var y1 = vertices[i4 + 1] * radius + y;
    var x2 = vertices[i4 + 2] * radius + x;
    var y2 = vertices[i4 + 3] * radius + y;

    this.insertLine(x1, y1, x2, y2, r, g, b);
  }
};

Renderer.prototype.initCircleVertices = function(v, resolution) {
  var size = 360 / resolution;

  for (var i = 0; i < resolution; i++) {
    var s1 = (i * size) * Math.PI / 180;
    var s2 = ((i + 1) * size) * Math.PI / 180;
    v.push(Math.cos(s1));
    v.push(Math.sin(s1));
    v.push(Math.cos(s2));
    v.push(Math.sin(s2));
  }
};


Renderer.prototype.transformAndInsert = function(v1, v2, transform, r, g, b) {
  var transformedV1 = new b2Vec2(),
    transformedV2 = new b2Vec2();

  b2Vec2.Mul(transformedV1, transform, v1);
  b2Vec2.Mul(transformedV2, transform, v2);
  renderer.insertLine(transformedV1.x, transformedV1.y,
    transformedV2.x, transformedV2.y,
    r, g, b);
};

Renderer.prototype.transformVerticesAndInsert = function(vertices, transform, r, g, b) {
  var vertexCount = vertices.length;

  for (var i = 1; i < vertexCount; i++) {
    renderer.transformAndInsert(vertices[i - 1], vertices[i], transform,
      r, g, b);
  }
};

b2CircleShape.prototype.draw = function(transform) {
  var circlePosition = this.position,
    center = new b2Vec2(circlePosition.x, circlePosition.y);
 // b2Vec2.Mul(center, transform, center);
  renderer.insertCircleVertices(transform, this.radius, center.x, center.y, 0, 0, 0, 5);
};

b2ChainShape.prototype.draw = function(transform) {
  renderer.transformVerticesAndInsert(this.vertices, transform, 0, 0, 0);
};


b2EdgeShape.prototype.draw = function(transform) {
  renderer.transformAndInsert(this.vertex1, this.vertex2, transform, 0, 0, 0);
};

b2PolygonShape.prototype.draw = function(transform) {
  var zPosition = renderer.currentVertex * 3;

  renderer.transformVerticesAndInsert(this.vertices, transform, 0, 0, 0);

  // create a loop
  var positions = renderer.positions;
  var last = (renderer.currentVertex - 1) * 3;
  renderer.insertLine(positions[last], positions[last + 1],
                      positions[zPosition], positions[zPosition + 1],
                      0, 0, 0);
};

function drawParticleSystem(system) {
  var particles = system.GetPositionBuffer();
  var color = system.GetColorBuffer();
  var maxParticles = particles.length,
    transform = new b2Transform();
  transform.SetIdentity();

  for (var i = 0, c = 0; i < maxParticles; i += 2, c += 4) {
    renderer.insertParticleVertices(system.radius, particles[i],
      particles[i + 1], color[c] * inv255, color[c + 1] * inv255, color[c + 2] * inv255, 3);
  }
}
