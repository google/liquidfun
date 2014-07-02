// fixture creation wrappers
var b2PolygonShape_CreateFixture_3 =
  Module.cwrap('b2PolygonShape_CreateFixture_3', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // vertices
      'number', 'number',
      'number', 'number',
      'number', 'number']);

var b2PolygonShape_CreateFixture_4 =
  Module.cwrap('b2PolygonShape_CreateFixture_4', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // b2Vec2
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number']);

var b2PolygonShape_CreateFixture_5 =
  Module.cwrap('b2PolygonShape_CreateFixture_5', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // b2Vec2
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number']);

var b2PolygonShape_CreateFixture_6 =
  Module.cwrap('b2PolygonShape_CreateFixture_6', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // b2Vec2
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number']);

var b2PolygonShape_CreateFixture_7 =
  Module.cwrap('b2PolygonShape_CreateFixture_7', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // b2Vec2
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number']);

var b2PolygonShape_CreateFixture_8 =
  Module.cwrap('b2PolygonShape_CreateFixture_8', 'number',
    ['number',
      // Fixture defs
      'number', 'number', 'number',
      'number', 'number',
      // b2Vec2
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number']);

// particle group creation wrappers
var b2PolygonShape_CreateParticleGroup_4 =
  Module.cwrap('b2PolygonShape_CreateParticleGroup_4', 'number',
    ['number',
      // particleGroupDef
      'number', 'number', 'number',
      'number', 'number', 'number',
      'number', 'number', 'number',
      'number', 'number', 'number',
      'number', 'number', 'number',
      'number', 'number', 'number',
      'number',
      // polygon
      'number', 'number',
      'number', 'number',
      'number', 'number',
      'number', 'number'
    ]);

// particle group destruction wrappers
var b2PolygonShape_DestroyParticlesInShape_4 =
  Module.cwrap('b2PolygonShape_DestroyParticlesInShape_4', 'number',
    ['number',
     //polygon shape
     'number', 'number',
     'number', 'number',
     'number', 'number',
     'number', 'number',
     // xf
     'number', 'number', 'number',
     'number']);

/** @constructor */
function b2PolygonShape() {
  this.position = new b2Vec2();
  this.vertices = [];
  this.type = b2Shape_Type_e_polygon;
}

b2PolygonShape.prototype.SetAsBoxXY = function(hx, hy) {
  this.vertices[0] = new b2Vec2(-hx, -hy);
  this.vertices[1] = new b2Vec2( hx, -hy);
  this.vertices[2] = new b2Vec2( hx,  hy);
  this.vertices[3] = new b2Vec2(-hx,  hy);
};

b2PolygonShape.prototype.SetAsBoxXYCenterAngle = function(hx, hy, center, angle) {
  this.vertices[0] = new b2Vec2(-hx, -hy);
  this.vertices[1] = new b2Vec2( hx, -hy);
  this.vertices[2] = new b2Vec2( hx,  hy);
  this.vertices[3] = new b2Vec2(-hx,  hy);

  var xf = new b2Transform();
  xf.p = center;
  xf.q.Set(angle);

  for (var i = 0; i < 4; i++) {
    b2Vec2.Mul(this.vertices[i], xf, this.vertices[i]);
  }
};

b2PolygonShape.prototype._CreateFixture = function(body, fixtureDef) {
  var vertices = this.vertices;
  switch (vertices.length) {
    case 3:
      var v0 = vertices[0];
      var v1 = vertices[1];
      var v2 = vertices[2];
      return b2PolygonShape_CreateFixture_3(body.ptr,
        // fixture Def
        fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
        fixtureDef.restitution, fixtureDef.userData,
        // filter def
        fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
        // points
        v0.x, v0.y,
        v1.x, v1.y,
        v2.x, v2.y);
      break;
    case 4:
      var v0 = vertices[0];
      var v1 = vertices[1];
      var v2 = vertices[2];
      var v3 = vertices[3];
      return b2PolygonShape_CreateFixture_4(body.ptr,
        // fixture Def
        fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
        fixtureDef.restitution, fixtureDef.userData,
        // filter def
        fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
        // points
        v0.x, v0.y,
        v1.x, v1.y,
        v2.x, v2.y,
        v3.x, v3.y);
      break;
    case 5:
      var v0 = vertices[0];
      var v1 = vertices[1];
      var v2 = vertices[2];
      var v3 = vertices[3];
      var v4 = vertices[4];
      return b2PolygonShape_CreateFixture_5(body.ptr,
        // fixture Def
        fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
        fixtureDef.restitution, fixtureDef.userData,
        // filter def
        fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
        // points
        v0.x, v0.y,
        v1.x, v1.y,
        v2.x, v2.y,
        v3.x, v3.y,
        v4.x, v4.y);
      break;
    case 6:
      var v0 = vertices[0];
      var v1 = vertices[1];
      var v2 = vertices[2];
      var v3 = vertices[3];
      var v4 = vertices[4];
      var v5 = vertices[5];
      return b2PolygonShape_CreateFixture_6(body.ptr,
        // fixture Def
        fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
        fixtureDef.restitution, fixtureDef.userData,
        // filter def
        fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
        // points
        v0.x, v0.y,
        v1.x, v1.y,
        v2.x, v2.y,
        v3.x, v3.y,
        v4.x, v4.y,
        v5.x, v5.y);
      break;
    case 7:
      var v0 = vertices[0];
      var v1 = vertices[1];
      var v2 = vertices[2];
      var v3 = vertices[3];
      var v4 = vertices[4];
      var v5 = vertices[5];
      var v6 = vertices[6];
      return b2PolygonShape_CreateFixture_7(body.ptr,
        // fixture Def
        fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
        fixtureDef.restitution, fixtureDef.userData,
        // filter def
        fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
        // points
        v0.x, v0.y,
        v1.x, v1.y,
        v2.x, v2.y,
        v3.x, v3.y,
        v4.x, v4.y,
        v5.x, v5.y,
        v6.x, v6.y);
      break;
    case 8:
      var v0 = vertices[0];
      var v1 = vertices[1];
      var v2 = vertices[2];
      var v3 = vertices[3];
      var v4 = vertices[4];
      var v5 = vertices[5];
      var v6 = vertices[6];
      var v7 = vertices[7];
      return b2PolygonShape_CreateFixture_8(body.ptr,
        // fixture Def
        fixtureDef.density, fixtureDef.friction, fixtureDef.isSensor,
        fixtureDef.restitution, fixtureDef.userData,
        // filter def
        fixtureDef.filter.categoryBits, fixtureDef.filter.groupIndex, fixtureDef.filter.maskBits,
        // points
        v0.x, v0.y,
        v1.x, v1.y,
        v2.x, v2.y,
        v3.x, v3.y,
        v4.x, v4.y,
        v5.x, v5.y,
        v6.x, v6.y,
        v6.x, v7.y);
      break;

  }
};

b2PolygonShape.prototype._CreateParticleGroup = function(particleSystem, pgd) {
  var v = this.vertices;
  switch (v.length) {
    case 3:
      break;
    case 4:
      return b2PolygonShape_CreateParticleGroup_4(
        particleSystem.ptr,
        // particle group def
        pgd.angle,  pgd.angularVelocity, pgd.color.r,
        pgd.color.g, pgd.color.b, pgd.color.a,
        pgd.flags, pgd.group.ptr, pgd.groupFlags,
        pgd.lifetime, pgd.linearVelocity.x, pgd.linearVelocity.y,
        pgd.position.x, pgd.position.y, pgd.positionData,
        pgd.particleCount,  pgd.strength, pgd.stride,
        pgd.userData,
        // polygon
        v[0].x, v[0].y,
        v[1].x, v[1].y,
        v[2].x, v[2].y,
        v[3].x, v[3].y);
      break;
  }
};

b2PolygonShape.prototype._DestroyParticlesInShape = function(ps, xf) {
  var v = this.vertices;
  switch (v.length) {
    case 3:
      break;
    case 4:
      return b2PolygonShape_DestroyParticlesInShape_4(
        ps.ptr,
        // polygon
        v[0].x, v[0].y,
        v[1].x, v[1].y,
        v[2].x, v[2].y,
        v[3].x, v[3].y,
        // xf
        xf.p.x, xf.p.y,
        xf.q.s, xf.q.c);
      break;
  }
};

/**@return bool*/
b2PolygonShape.prototype.Validate = function() {
  for (var i = 0, max = this.vertices.length; i < max; ++i) {
    var i1 = i;
    var i2 = i < max - 1 ? i1 + 1 : 0;
    var p = this.vertices[i1];
    var e = this.vertices[i2];
    var eSubP = new b2Vec2();
    b2Vec2.Sub(eSubP, e, p);

    for (var j = 0; j < max; ++j) {
      if (j == i1 || j == i2) {
        continue;
      }

      var v = new b2Vec2();
      b2Vec2.Sub(v, this.vertices[j], p);
      var c = b2Vec2.Cross(eSubP, v);
      if (c < 0.0) {
        return false;
      }
    }
  }

  return true;
}