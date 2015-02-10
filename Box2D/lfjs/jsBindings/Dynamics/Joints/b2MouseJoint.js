var b2MouseJoint_SetTarget =
  Module.cwrap('b2MouseJoint_SetTarget', 'null',
    ['number', 'number', 'number']);

/**@constructor*/
function b2MouseJoint(def) {
  this.ptr = null;
  this.next = null;
}

b2MouseJoint.prototype.SetTarget = function(p) {
  b2MouseJoint_SetTarget(this.ptr, p.x, p.y);
};

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