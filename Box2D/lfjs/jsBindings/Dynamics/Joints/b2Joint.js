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

/**@constructor*/
function b2Joint() {}

b2Joint.prototype.GetBodyA = function() {
  return world.bodiesLookup[b2Joint_GetBodyA(this.ptr)];
};

b2Joint.prototype.GetBodyB = function() {
  return world.bodiesLookup[b2Joint_GetBodyB(this.ptr)];
};