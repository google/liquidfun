var b2RopeJoint_SetMaxLength = Module.cwrap('', 'null', ['number', 'number']);
var b2RopeJoint_GetMaxLength = Module.cwrap('', 'number', ['number']);
var b2RopeJoint_GetLimitState = Module.cwrap('', 'number', ['number']);

/**@constructor*/
function b2RopeJoint(def) {
  b2Joint.call(this, def);
}
b2RopeJoint.prototype = Object.create(b2Joint.prototype);
b2RopeJoint.prototype.constructor = b2RopeJoint;

b2RopeJoint.prototype.b2RopeJoint_SetMaxLength = function(length) {
    b2RopeJoint_SetMaxLength(this.ptr, length);
}
b2RopeJoint.prototype.b2RopeJoint_GetMaxLength = function() {
    return b2RopeJoint_GetMaxLength(this.ptr);
}
b2RopeJoint.prototype.b2RopeJoint_GetLimitState = function() {
    return b2RopeJoint_GetLimitState(this.ptr);
}

var b2RopeJointDef_Create = Module.cwrap("b2RopeJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // rope joint def
    'number', 'number', 'number',
    'number', 'number']);

/**@constructor*/
function b2RopeJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;

  // mouse joint def
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.maxLength = 0;
}

b2RopeJointDef.prototype.Create = function(world) {
  var ropeJoint = new b2RopeJoint(this);
  ropeJoint.ptr = b2RopeJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //rope joint def
    this.localAnchorA.x, this.localAnchorA.y, this.localAnchorB.x,
    this.localAnchorB.y, this.maxLength);
  return ropeJoint;
};
