var b2MotorJoint_SetAngularOffset =
  Module.cwrap('b2MotorJoint_SetAngularOffset', 'null', ['number', 'number']);

var b2MotorJoint_SetLinearOffset =
  Module.cwrap('b2MotorJoint_SetLinearOffset', 'null',
    ['number', 'number', 'number']);

var b2MotorJoint_SetMaxForce = Module.cwrap("b2MotorJoint_SetMaxForce", "null", ["number", "number"]);
var b2MotorJoint_SetMaxTorque = Module.cwrap("b2MotorJoint_SetMaxTorque", "null", ["number", "number"]);
var b2MotorJoint_SetCorrectionFactor = Module.cwrap("b2MotorJoint_SetCorrectionFactor", "null", ["number", "number"]);


/**@constructor*/
function b2MotorJoint(def) {
  b2Joint.call(this, def);
  this.angularOffset = def.angularOffset;
  this.linearOffset = def.linearOffset.Clone();
  this.maxForce = def.maxForce;
  this.maxTorque = def.maxTorque;
  this.correctionFactor = def.correctionFactor;
}
b2MotorJoint.prototype = Object.create(b2Joint.prototype);
b2MotorJoint.prototype.constructor = b2MotorJoint;

b2MotorJoint.prototype.SetAngularOffset = function(angle) {
  b2MotorJoint_SetAngularOffset(this.ptr, angle);
  this.angularOffset = angle;
};
b2MotorJoint.prototype.SetLinearOffset = function(v) {
  b2MotorJoint_SetLinearOffset(this.ptr, v.x, v.y);
  this.linearOffset.Set(v.x, v.y);
};
b2MotorJoint.prototype.SetMaxForce = function(force) {
    b2MotorJoint_SetMaxForce(this.ptr, force);
    this.maxForce = force;
}
b2MotorJoint.prototype.SetMaxTorque = function(torque) {
    b2MotorJoint_SetMaxTorque(this.ptr, torque);
    this.maxTorque = torque;
}
b2MotorJoint.prototype.SetCorrectionFactor = function(factor) {
    b2MotorJoint_SetCorrectionFactor(this.ptr, factor);
    this.correctionFactor = factor;
}

var b2MotorJointDef_Create = Module.cwrap("b2MotorJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // motor joint def
    'number', 'number', 'number',
    'number', 'number', 'number']);

var b2MotorJointDef_InitializeAndCreate = Module.cwrap("b2MotorJointDef_InitializeAndCreate",
  'number',
  ['number',
    // initialize args
    'number', 'number', 'number',
    'number',
    // motor joint def
    'number', 'number', 'number']);

/**@constructor*/
function b2MotorJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;

  // motor joint def
  this.angularOffset = 0;
  this.correctionFactor = 0.3;
  this.linearOffset = new b2Vec2();
  this.maxForce = 0;
  this.maxTorque = 0;
}

b2MotorJointDef.prototype.Create = function(world) {
  var motorJoint = new b2MotorJoint(this);
  motorJoint.ptr = b2MotorJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //motor joint def
    this.angularOffset, this.correctionFactor, this.linearOffset.x,
    this.linearOffset.y, this.maxForce, this.maxTorque);
  return motorJoint;
};

b2MotorJointDef.prototype.InitializeAndCreate  = function(bodyA, bodyB) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var motorJoint = new b2MotorJoint(this);
  motorJoint.ptr = b2MotorJointDef_InitializeAndCreate(
    world.ptr,
    // InitializeArgs
    this.bodyA.ptr, this.bodyB.ptr,
    // joint def
    this.collideConnected,
    //motor joint def
    this.correctionFactor, this.maxForce, this.maxTorque);
  b2World._Push(motorJoint, world.joints);
  return motorJoint;
};
