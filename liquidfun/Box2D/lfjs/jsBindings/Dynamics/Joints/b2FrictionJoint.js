var b2FrictionJointDef_Create = Module.cwrap("b2FrictionJointDef_Create",
  'number',
  ['number',
   // joint Def
   'number', 'number', 'number',
   // friction joint def
   'number', 'number', 'number',
   'number', 'number', 'number']);

var b2FrictionJointDef_InitializeAndCreate = Module.cwrap("b2FrictionJointDef_InitializeAndCreate",
  'number',
  ['number',
    // initialize args
    'number', 'number', 'number',
    'number',
    // friction joint def
    'number', 'number', 'number']);

/**@constructor*/
function b2FrictionJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;

  // friction joint def
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.maxForce = 0;
  this.maxTorque = 0;
  this.userData = null;
}

b2FrictionJointDef.prototype.Create = function(world) {
  var frictionJoint = new b2FrictionJoint(this);
  frictionJoint.ptr = b2FrictionJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //friction joint def
    this.localAnchorA.x, this.localAnchorA.y, this.localAnchorB.x,
    this.localAnchorB.y, this.maxForce, this.maxTorque);
  return frictionJoint;
};

b2FrictionJointDef.prototype.InitializeAndCreate  = function(bodyA, bodyB, anchor) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var frictionJoint = new b2FrictionJoint(this);
  frictionJoint.ptr = b2FrictionJointDef_InitializeAndCreate(
    world.ptr,
    // InitializeArgs
    this.bodyA.ptr, this.bodyB.ptr, anchor.x,
    anchor.y,
    // joint def
    this.collideConnected,
    // friction joint def
    this.maxForce, this.maxTorque);
  b2World._Push(frictionJoint, world.joints);
  return frictionJoint;
};

b2FrictionJoint_SetMaxForce = Module.cwrap("b2FrictionJoint_SetMaxForce", "null", ["number", "number"]);
b2FrictionJoint_GetMaxForce = Module.cwrap("b2FrictionJoint_GetMaxForce", "number", ["number"]);
b2FrictionJoint_SetMaxTorque = Module.cwrap("b2FrictionJoint_SetMaxTorque", "null", ["number", "number"]);
b2FrictionJoint_GetMaxTorque = Module.cwrap("b2FrictionJoint_GetMaxTorque", "number", ["number"]);

/**@constructor*/
function b2FrictionJoint(def) {
  b2Joint.call(this, def);
}
b2FrictionJoint.prototype = Object.create(b2Joint.prototype);
b2FrictionJoint.prototype.constructor = b2FrictionJoint;

b2FrictionJoint.prototype.SetMaxForce = function(force) {
    b2FrictionJoint_SetMaxForce(this.prt, force);
}
b2FrictionJoint.prototype.GetMaxForce = function() {
    return b2FrictionJoint_GetMaxForce(this.prt);
}
b2FrictionJoint.prototype.SetMaxTorque = function(torque) {
    b2FrictionJoint_SetMaxTorque(this.prt, torque);
}
b2FrictionJoint.prototype.GetMaxTorque = function() {
    return b2FrictionJoint_GetMaxTorque(this.prt);
}
