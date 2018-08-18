var e_unknownJoint = 0;
var e_revoluteJoint = 1;
var e_prismaticJoint = 2;
var e_distanceJoint = 3;
var e_pulleyJoint = 4;
var e_mouseJoint = 5;
var e_gearJoint = 6;
var e_wheelJoint = 7;
var e_weldJoint = 8;
var e_frictionJoint = 9;
var e_ropeJoint = 10;
var e_motorJoint = 11;

var b2Joint_GetBodyA = Module.cwrap('b2Joint_GetBodyA', 'number', ['number']);
var b2Joint_GetBodyB = Module.cwrap('b2Joint_GetBodyB', 'number', ['number']);
var b2Joint_GetType = Module.cwrap('b2Joint_GetType', 'number', ['number']);
var b2Joint_GetAnchorA =
  Module.cwrap('b2Joint_GetAnchorA', 'null', ['number', 'number']);
var b2Joint_GetAnchorB =
  Module.cwrap('b2Joint_GetAnchorB', 'null', ['number', 'number']);


/**@constructor*/
function b2Joint() {
  this.ptr = null;
  this.next = null;
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
