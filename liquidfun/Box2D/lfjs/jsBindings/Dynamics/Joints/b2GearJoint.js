var b2GearJoint_SetRatio = Module.cwrap("b2GearJoint_SetRatio", 'number', ['number', 'number']);

/**@constructor*/
function b2GearJoint(def) {
  b2Joint.call(this, def);
  this.joint1 = def.joint1;
  this.joint2 = def.joint2;
  this.ratio = def.ratio;
}
b2GearJoint.prototype = Object.create(b2Joint.prototype);
b2GearJoint.prototype.constructor = b2GearJoint;

b2GearJoint.prototype.SetRatio = function (ratio) {
  b2GearJoint_SetRatio(this.ptr, ratio);
  this.ratio = ratio;
}

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

b2GearJointDef.prototype.Create = function (world) {
  var gearJoint = new b2GearJoint(this);
  gearJoint.ptr = b2GearJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //gear joint def
    this.joint1.ptr, this.joint2.ptr, this.ratio);
  return gearJoint;
};
