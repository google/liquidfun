var b2MouseJoint_SetTarget =
  Module.cwrap('b2MouseJoint_SetTarget', 'null',
    ['number', 'number', 'number']);
var b2MouseJoint_SetMaxForce = Module.cwrap("b2MouseJoint_SetMaxForce", "null", ["number", "number"]);
var b2MouseJoint_SetFrequency = Module.cwrap("b2MouseJoint_SetFrequency", "null", ["number", "number"]);
var b2MouseJoint_SetDampingRatio = Module.cwrap("b2MouseJoint_SetDampingRatio", "null", ["number", "number"]);
/**@constructor*/
function b2MouseJoint(def) {
  b2Joint.call(this, def);
  this.target = def.target.Clone();
  this.maxForce = def.maxForce;
  this.frequencyHz = def.frequencyHz;
  this.dampingRatio = def.dampingRatio;
}
b2MouseJoint.prototype = Object.create(b2Joint.prototype);
b2MouseJoint.prototype.constructor = b2MouseJoint;

b2MouseJoint.prototype.SetTarget = function(p) {
  b2MouseJoint_SetTarget(this.ptr, p.x, p.y);
  this.target.Set(p.x, p.y);
};
b2MouseJoint.prototype.SetMaxForce = function(force) {
    b2MouseJoint_SetMaxForce(this.ptr, force);
    this.maxForce = force;
}
b2MouseJoint.prototype.SetFrequency = function(hz) {
    b2MouseJoint_SetFrequency(this.ptr, hz);
    this.frequencyHz = hz;
}
b2MouseJoint.prototype.SetDampingRatio = function(ratio) {
    b2MouseJoint_SetDampingRatio(this.ptr, ratio);
    this.dampingRatio = ratio;
}

var b2MouseJointDef_Create = Module.cwrap("b2MouseJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // mouse joint def
    'number', 'number', 'number',
    'number', 'number']);

/**@constructor*/
function b2MouseJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;

  // mouse joint def
  this.dampingRatio = 0.7;
  this.frequencyHz = 5;
  this.maxForce = 0;
  this.target = new b2Vec2();
}

b2MouseJointDef.prototype.Create = function(world) {
  var mouseJoint = new b2MouseJoint(this);
  mouseJoint.ptr = b2MouseJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //mouse joint def
    this.dampingRatio, this.frequencyHz, this.maxForce,
    this.target.x, this.target.y);
  return mouseJoint;
};