// wheel joint globals
var b2WheelJoint_SetMotorSpeed =
  Module.cwrap('b2WheelJoint_SetMotorSpeed', 'null', ['number', 'number']);
var b2WheelJoint_SetSpringFrequencyHz =
  Module.cwrap('b2WheelJoint_SetSpringFrequencyHz', 'null', ['number', 'number']);

/**@constructor*/
function b2WheelJoint(def) {
  this.next = null;
  this.ptr = null;
}

b2WheelJoint.prototype.SetMotorSpeed = function(speed) {
  b2WheelJoint_SetMotorSpeed(this.ptr, speed);
};

b2WheelJoint.prototype.SetSpringFrequencyHz = function(hz) {
  b2WheelJoint_SetSpringFrequencyHz(this.ptr, hz);
};

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

b2WheelJointDef.prototype.Create = function(world) {
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

b2WheelJointDef.prototype.InitializeAndCreate  = function(bodyA, bodyB, anchor, axis) {
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
