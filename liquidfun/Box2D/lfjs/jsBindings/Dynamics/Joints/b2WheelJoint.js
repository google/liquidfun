// wheel joint globals
var b2WheelJoint_SetMotorSpeed =
  Module.cwrap('b2WheelJoint_SetMotorSpeed', 'null', ['number', 'number']);
var b2WheelJoint_SetSpringFrequencyHz =
  Module.cwrap('b2WheelJoint_SetSpringFrequencyHz', 'null', ['number', 'number']);

var b2WheelJoint_GetLocalAxisA = Module.cwrap('b2WheelJoint_GetLocalAxisA', 'null', ['number', 'number']);
var b2WheelJoint_GetJointTranslation = Module.cwrap('b2WheelJoint_GetJointTranslation', 'number', ['number']);
var b2WheelJoint_GetJointSpeed = Module.cwrap('b2WheelJoint_GetJointSpeed', 'number', ['number']);
var b2WheelJoint_IsMotorEnabled = Module.cwrap('b2WheelJoint_IsMotorEnabled', 'number', ['number']);
var b2WheelJoint_EnableMotor = Module.cwrap('b2WheelJoint_EnableMotor', 'null', ['number', 'number']);
var b2WheelJoint_GetMotorSpeed = Module.cwrap('b2WheelJoint_GetMotorSpeed', 'number', ['number']);
var b2WheelJoint_SetMaxMotorTorque = Module.cwrap('b2WheelJoint_SetMaxMotorTorque', 'null', ['number', 'number']);
var b2WheelJoint_GetMotorTorque = Module.cwrap('b2WheelJoint_GetMotorTorque', 'number', ['number', 'number']);
var b2WheelJoint_SetSpringDampingRatio = Module.cwrap('b2WheelJoint_SetSpringDampingRatio', 'null', ['number', 'number']);

/**@constructor*/
function b2WheelJoint(def) {
  b2Joint.call(this, def);
  this.maxMotorTorque = def.maxMotorTorque;
  this.frequencyHz = def.frequencyHz;
  this.dampingRatio = def.dampingRatio;
}
b2WheelJoint.prototype = Object.create(b2Joint.prototype);
b2WheelJoint.prototype.constructor = b2WheelJoint;

b2WheelJoint.prototype.SetMotorSpeed = function (speed) {
  b2WheelJoint_SetMotorSpeed(this.ptr, speed);
};

b2WheelJoint.prototype.SetSpringFrequencyHz = function (hz) {
  b2WheelJoint_SetSpringFrequencyHz(this.ptr, hz);
  this.frequencyHz = hz;
};

b2WheelJoint.prototype.GetLocalAxisA = function () {
  b2WheelJoint_GetLocalAxisA(this.ptr, _vec2Buf.byteOffset);
  var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
  return new b2Vec2(result[0], result[1]);
}
b2WheelJoint.prototype.GetJointTranslation = function () {
  return b2WheelJoint_GetJointTranslation(this.ptr);
}
b2WheelJoint.prototype.GetJointSpeed = function () {
  return b2WheelJoint_GetJointSpeed(this.ptr);
}
b2WheelJoint.prototype.IsMotorEnabled = function () {
  return b2WheelJoint_IsMotorEnabled(this.ptr);
}
b2WheelJoint.prototype.EnableMotor = function (flag) {
  b2WheelJoint_EnableMotor(this.ptr, flag);
}
b2WheelJoint.prototype.GetMotorSpeed = function () {
  return b2WheelJoint_GetMotorSpeed(this.ptr);
}
b2WheelJoint.prototype.SetMaxMotorTorque = function (torque) {
  b2WheelJoint_SetMaxMotorTorque(this.ptr, torque);
  this.maxMotorTorque = torque;
}
b2WheelJoint.prototype.GetMotorTorque = function (inv_dt) {
  return b2WheelJoint_GetMotorTorque(this.ptr, inv_dt);
}
b2WheelJoint.prototype.SetSpringDampingRatio = function (ratio) {
  b2WheelJoint_SetSpringDampingRatio(this.ptr, ratio);
  this.dampingRatio = ratio;
}

// wheeljoint def
var b2WheelJointDef_Create = Module.cwrap("b2WheelJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // wheel joint def
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number']);

var b2WheelJointDef_InitializeAndCreate = Module.cwrap("b2WheelJointDef_InitializeAndCreate",
  'number',
  ['number',
    // initialize args
    'number', 'number', 'number',
    'number', 'number', 'number',
    // joint def
    'number',
    // wheel joint def
    'number', 'number', 'number',
    'number', 'number']);

/** @constructor*/
function b2WheelJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;

  // wheel joint def
  this.dampingRatio = 0.7;
  this.enableMotor = false;
  this.frequencyHz = 2;
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.localAxisA = new b2Vec2(1, 0);
  this.maxMotorTorque = 0;
  this.motorSpeed = 0;
}

b2WheelJointDef.prototype.Create = function (world) {
  var wheelJoint = new b2WheelJoint(this);
  wheelJoint.ptr = b2WheelJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //wheel joint def
    this.dampingRatio, this.enableMotor, this.frequencyHz,
    this.localAnchorA.x, this.localAnchorA.y, this.localAnchorB.x,
    this.localAnchorB.y, this.localAxisA.x, this.localAxisA.y,
    this.maxMotorTorque, this.motorSpeed);
  return wheelJoint;
};

b2WheelJointDef.prototype.InitializeAndCreate = function (bodyA, bodyB, anchor, axis) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var wheelJoint = new b2WheelJoint(this);
  wheelJoint.ptr = b2WheelJointDef_InitializeAndCreate(
    world.ptr,
    // InitializeArgs
    this.bodyA.ptr, this.bodyB.ptr, anchor.x,
    anchor.y, axis.x, axis.y,
    // joint def
    this.collideConnected,
    // wheel joint def
    this.dampingRatio, this.enableMotor, this.frequencyHz,
    this.maxMotorTorque, this.motorSpeed);
  b2World._Push(wheelJoint, world.joints);
  return wheelJoint;
};
