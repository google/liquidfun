var b2GearJoint_GetRatio = Module.cwrap("b2GearJoint_GetRatio", 'number',
  ['number']);

function b2GearJoint(def) {
  this.ptr = null;
  this.next = null;
}

b2GearJoint.prototype.GetRatio = function() {
  return b2GearJoint_GetRatio(this.ptr);
};

var b2GearJointDef_Create = Module.cwrap("b2GearJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // gear joint def
    'number', 'number', 'number']);

/**@constructor*/
function b2GearJointDef() {
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;
  this.joint1 = null;
  this.joint2 = null;
  this.ratio = 0;
}

b2GearJointDef.prototype.Create = function(world) {
  var gearJoint = new b2GearJoint(this);
  gearJoint.ptr = b2GearJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //gear joint def
    this.joint1.ptr, this.joint2.ptr, this.ratio);
  return gearJoint;
};
