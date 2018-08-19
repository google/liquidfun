var b2_unknownJoint = 0;
var b2_revoluteJoint = 1;
var b2_prismaticJoint = 2;
var b2_distanceJoint = 3;
var b2_pulleyJoint = 4;
var b2_mouseJoint = 5;
var b2_gearJoint = 6;
var b2_wheelJoint = 7;
var b2_weldJoint = 8;
var b2_frictionJoint = 9;
var b2_ropeJoint = 10;
var b2_motorJoint = 11;

var b2_inactiveLimit = 0;
var b2_atLowerLimit = 1;
var b2_atUpperLimit = 2;
var b2_equalLimits = 3;

var b2Joint_GetBodyA = Module.cwrap('b2Joint_GetBodyA', 'number', ['number']);
var b2Joint_GetBodyB = Module.cwrap('b2Joint_GetBodyB', 'number', ['number']);
var b2Joint_GetType = Module.cwrap('b2Joint_GetType', 'number', ['number']);
var b2Joint_GetAnchorA = Module.cwrap('b2Joint_GetAnchorA', 'null', ['number', 'number']);
var b2Joint_GetAnchorB = Module.cwrap('b2Joint_GetAnchorB', 'null', ['number', 'number']);
var b2Joint_GetReactionForce = Module.cwrap('b2Joint_GetReactionForce', 'null', ['number', 'number', 'number']);
var b2Joint_GetReactionTorque = Module.cwrap('b2Joint_GetReactionTorque', 'number', ['number', 'number']);
var b2Joint_IsActive = Module.cwrap('b2Joint_IsActive', 'boolean', ['number']);
var b2Joint_GetCollideConnected = Module.cwrap('b2Joint_GetCollideConnected', 'boolean', ['number']);

/**@constructor*/
function b2Joint(def) {
  this.ptr = null;
  this.next = null;
  this.bodyA = def.bodyA;
  this.bodyB = def.bodyB;
  this.userData = def.userData;
}

b2Joint.prototype.GetBodyA = function() {
  return world.bodiesLookup[b2Joint_GetBodyA(this.ptr)];
};

b2Joint.prototype.GetBodyB = function() {
  return world.bodiesLookup[b2Joint_GetBodyB(this.ptr)];
};

b2Joint.prototype.GetType = function() {
  return b2Joint_GetType(this.ptr);
};

b2Joint.prototype.GetAnchorA = function() {
  b2Joint_GetAnchorA(this.ptr, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};
    
b2Joint.prototype.GetAnchorB = function() {
  b2Joint_GetAnchorB(this.ptr, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
};
b2Joint.prototype.GetReactionForce = function(inv_dt) {
  b2Joint_GetReactionForce(this.ptr, inv_dt, _vec2Buf);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
}
b2Joint.prototype.GetReactionTorque = function(inv_dt) {
  return b2Joint_GetReactionTorque(this.ptr, inv_dt);
}
b2Joint.prototype.IsActive = function() {
  return b2Joint_IsActive(this.ptr);
}
b2Joint.prototype.GetCollideConnected = function() {
  return b2Joint_GetCollideConnected(this.ptr);
}
