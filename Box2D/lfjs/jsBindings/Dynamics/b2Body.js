// b2Body Globals
var b2Body_ApplyAngularImpulse = Module.cwrap('b2Body_ApplyAngularImpulse', 'null',
  ['number', 'number', 'number']);

var b2Body_ApplyForce = Module.cwrap('b2Body_ApplyForce', 'number',
  ['number', 'number', 'number', 'number', 'number', 'number']);

var b2Body_ApplyForceToCenter = Module.cwrap('b2Body_ApplyForceToCenter', 'number',
  ['number', 'number', 'number', 'number']);

var b2Body_ApplyTorque = Module.cwrap('b2Body_ApplyTorque', 'number',
  ['number', 'number', 'number']);

var b2Body_DestroyFixture = Module.cwrap('b2Body_DestroyFixture', 'null',
  ['number', 'number']);

var b2Body_GetAngle = Module.cwrap('b2Body_GetAngle', 'number', ['number']);

var b2Body_GetAngularVelocity =
  Module.cwrap('b2Body_GetAngularVelocity', 'number', ['number']);

var b2Body_GetInertia = Module.cwrap('b2Body_GetInertia', 'number', ['number']);

var b2Body_GetLinearVelocity =
  Module.cwrap('b2Body_GetLinearVelocity', 'null', ['number', 'number']);

var b2Body_GetLocalPoint = Module.cwrap('b2Body_GetLocalPoint', 'null',
  ['number', 'number', 'number', 'number']);

var b2Body_GetLocalVector = Module.cwrap('b2Body_GetLocalVector', 'null',
  ['number', 'number', 'number', 'number']);

var b2Body_GetMass = Module.cwrap('b2Body_GetMass', 'number', ['number']);

var b2Body_GetPosition = Module.cwrap('b2Body_GetPosition', 'null', ['number', 'number']);

var b2Body_GetTransform = Module.cwrap('b2Body_GetTransform', 'null',
  ['number', 'number']);

var b2Body_GetType = Module.cwrap('b2Body_GetType', 'number', ['number']);

var b2Body_GetWorldCenter = Module.cwrap('b2Body_GetWorldCenter', 'null',
  ['number', 'number']);

var b2Body_GetWorldPoint = Module.cwrap('b2Body_GetWorldPoint', 'null',
  ['number', 'number', 'number', 'number']);

var b2Body_GetWorldVector = Module.cwrap('b2Body_GetWorldVector', 'null',
  ['number', 'number', 'number', 'number']);

var b2Body_SetAngularVelocity = Module.cwrap('b2Body_SetAngularVelocity', 'null',
  ['number', 'number']);

var b2Body_SetAwake =
  Module.cwrap('b2Body_SetAwake', 'number',['number', 'number']);

var b2Body_SetLinearVelocity = Module.cwrap('b2Body_SetLinearVelocity', 'null',
  ['number', 'number', 'number']);

var b2Body_SetMassData = Module.cwrap('b2Body_SetMassData', 'null',
  ['number', 'number', 'number',
   'number', 'number']);

var b2Body_SetTransform =
  Module.cwrap('b2Body_SetTransform', 'null', ['number', 'number', 'number']);

var b2Body_SetType =
  Module.cwrap('b2Body_SetType', 'null', ['number', 'number']);


// memory offsets
var b2Body_xf_offset = Offsets.b2Body.xf;
var b2Body_userData_offset = Offsets.b2Body.userData;
/**@constructor*/
function b2Body(ptr) {
  this.buffer = new DataView(Module.HEAPU8.buffer, ptr);
  this.ptr = ptr;
  this.fixtures = [];
}

b2Body.prototype.ApplyAngularImpulse = function(force, wake) {
  b2Body_ApplyAngularImpulse(this.ptr, force, wake);
};

b2Body.prototype.ApplyForce = function(force, point, wake) {
  b2Body_ApplyForce(this.ptr, force.x, force.y, point.x, point.y, wake);
};

b2Body.prototype.ApplyForceToCenter = function(force, wake) {
  b2Body_ApplyForceToCenter(this.ptr, force.x, force.y, wake);
};

b2Body.prototype.ApplyTorque = function(force, wake) {
  b2Body_ApplyTorque(this.ptr, force, wake);
};

b2Body.prototype.CreateFixtureFromDef = function(fixtureDef) {
  var fixture = new b2Fixture();
  fixture.FromFixtureDef(fixtureDef);
  fixture._SetPtr(fixtureDef.shape._CreateFixture(this, fixtureDef));
  fixture.body = this;
  b2World._Push(fixture, this.fixtures);
  world.fixturesLookup[fixture.ptr] = fixture;
  return fixture;
};

b2Body.prototype.CreateFixtureFromShape = function(shape, density) {
  var fixtureDef = new b2FixtureDef();
  fixtureDef.shape = shape;
  fixtureDef.density = density;
  return this.CreateFixtureFromDef(fixtureDef);
};

b2Body.prototype.DestroyFixture = function(fixture) {
  b2Body_DestroyFixture(this.ptr, fixture.ptr);
  b2World._RemoveItem(fixture, this.fixtures);
};

b2Body.prototype.GetAngle = function() {
  return b2Body_GetAngle(this.ptr);
};

b2Body.prototype.GetAngularVelocity = function() {
  return b2Body_GetAngularVelocity(this.ptr);
};

b2Body.prototype.GetInertia = function() {
  return b2Body_GetInertia(this.ptr);
};

b2Body.prototype.GetMass = function() {
  return b2Body_GetMass(this.ptr);
};

b2Body.prototype.GetLinearVelocity = function() {
  b2Body_GetLinearVelocity(this.ptr, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};

b2Body.prototype.GetLocalPoint = function(vec) {
  b2Body_GetLocalPoint(this.ptr, vec.x, vec.y, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};

b2Body.prototype.GetLocalVector = function(vec) {
  b2Body_GetLocalVector(this.ptr, vec.x, vec.y, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};


b2Body.prototype.GetPosition = function() {
  b2Body_GetPosition(this.ptr, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return  new b2Vec2(result[0], result[1]);
};

b2Body.prototype.GetTransform = function() {
  var transform = new b2Transform();
  transform.p.x = this.buffer.getFloat32(b2Body_xf_offset, true);
  transform.p.y = this.buffer.getFloat32(b2Body_xf_offset+4, true);
  transform.q.s = this.buffer.getFloat32(b2Body_xf_offset+8, true);
  transform.q.c = this.buffer.getFloat32(b2Body_xf_offset+12, true);
  return transform;
};

b2Body.prototype.GetType = function() {
  return b2Body_GetType(this.ptr);
};

b2Body.prototype.GetUserData = function() {
  return this.buffer.getUint32(b2Body_userData_offset, true);
};

b2Body.prototype.GetWorldCenter = function() {
  b2Body_GetWorldCenter(this.ptr, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};

b2Body.prototype.GetWorldPoint = function(vec) {
  b2Body_GetWorldPoint(this.ptr, vec.x, vec.y, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};

b2Body.prototype.GetWorldVector = function(vec) {
  b2Body_GetWorldVector(this.ptr, vec.x, vec.y, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};

b2Body.prototype.SetAngularVelocity = function(angle) {
  b2Body_SetAngularVelocity(this.ptr, angle);
};

b2Body.prototype.SetAwake = function(flag) {
  b2Body_SetAwake(this.ptr, flag);
};

b2Body.prototype.SetLinearVelocity = function(v) {
  b2Body_SetLinearVelocity(this.ptr, v.x, v.y);
};

b2Body.prototype.SetMassData = function(massData) {
  b2Body_SetMassData(this.ptr, massData.mass, massData.center.x, massData.center.y, massData.I);
};

b2Body.prototype.SetTransform = function(v, angle) {
  b2Body_SetTransform(this.ptr, v.x, v.y, angle);
};

b2Body.prototype.SetType = function(type) {
  b2Body_SetType(this.ptr, type);
};

// General body globals
var b2_staticBody = 0;
var b2_kinematicBody = 1;
var b2_dynamicBody = 2;

/** @constructor */
function b2BodyDef() {
  this.active = true;
  this.allowSleep = true;
  this.angle = 0;
  this.angularVelocity = 0;
  this.angularDamping = 0;
  this.awake = true;
  this.bullet = false;
  this.fixedRotation = false;
  this.gravityScale = 1.0;
  this.linearDamping = 0;
  this.linearVelocity = new b2Vec2();
  this.position = new b2Vec2();
  this.type = b2_staticBody;
  this.userData = null;
}

