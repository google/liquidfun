var b2WeldJointDef_Create = Module.cwrap("b2WeldJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // weld joint def
    'number', 'number', 'number',
    'number', 'number', 'number']);

var b2WeldJointDef_InitializeAndCreate = Module.cwrap("b2WeldJointDef_InitializeAndCreate",
  'number',
  ['number',
    // initialize args
    'number', 'number', 'number',
    'number',
    // joint def
    'number',
    // weld joint def
    'number', 'number']);

/** @constructor */
function b2WeldJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;

  // Weld joint def
  this.dampingRatio = 0;
  this.frequencyHz = 0;
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.referenceAngle = 0;
}

b2WeldJointDef.prototype.Create = function(world) {
  var weldJoint = new b2WeldJoint(this);
  weldJoint.ptr = b2WeldJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //Weld joint def
    this.dampingRatio, this.frequencyHz, this.localAnchorA.x,
    this.localAnchorA.y, this.localAnchorB.x, this.localAnchorB.y,
    this.referenceAngle);
  return weldJoint;
};

b2WeldJointDef.prototype.InitializeAndCreate  = function(bodyA, bodyB, anchor) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var weldJoint = new b2WeldJoint(this);
  weldJoint.ptr = b2WeldJointDef_InitializeAndCreate(
    world.ptr,
    // InitializeArgs
    this.bodyA.ptr, this.bodyB.ptr, anchor.x,
    anchor.y,
    // joint def
    this.collideConnected,
    //Weld joint def
    this.dampingRatio, this.frequencyHz);
  b2World._Push(weldJoint, world.joints);
  return weldJoint;
};

var b2WeldJoint_GetReferenceAngle = Module.cwrap('b2WeldJoint_GetReferenceAngle', 'number', ['number']);
var b2WeldJoint_SetFrequency = Module.cwrap('b2WeldJoint_SetFrequency', 'null', ['number', 'number']);
var b2WeldJoint_GetFrequency = Module.cwrap('b2WeldJoint_GetFrequency', 'number', ['number']);
var b2WeldJoint_SetDampingRatio = Module.cwrap('b2WeldJoint_SetDampingRatio', 'null', ['number', 'number']);
var b2WeldJoint_GetDampingRatio = Module.cwrap('b2WeldJoint_GetDampingRatio', 'number', ['number']);

/** @constructor */
function b2WeldJoint(def) {
  b2Joint.call(this, def);
}
b2WeldJoint.prototype = Object.create(b2Joint.prototype);
b2WeldJoint.prototype.constructor = b2WeldJoint;

b2WeldJoint.prototype.GetReferenceAngle = function() {
    return b2WeldJoint_GetReferenceAngle(this.ptr);
}
b2WeldJoint.prototype.SetFrequency = function(hz) {
    b2WeldJoint_SetFrequency(this.ptr, hz);
}
b2WeldJoint.prototype.GetFrequency = function() {
    return b2WeldJoint_GetFrequency(this.ptr);
}
b2WeldJoint.prototype.SetDampingRatio = function(ratio) {
    b2WeldJoint_SetDampingRatio(this.ptr, ratio);
}
b2WeldJoint.prototype.GetDampingRatio = function() {
    return b2WeldJoint_GetDampingRatio(this.ptr);
}
