var b2RevoluteJoint_EnableLimit =
  Module.cwrap('b2RevoluteJoint_EnableLimit', 'number',
    ['number', 'number']);

var b2RevoluteJoint_EnableMotor =
  Module.cwrap('b2RevoluteJoint_EnableMotor', 'number',
    ['number', 'number']);

var b2RevoluteJoint_GetJointAngle =
  Module.cwrap('b2RevoluteJoint_GetJointAngle', 'number',
    ['number']);

var b2RevoluteJoint_IsLimitEnabled =
  Module.cwrap('b2RevoluteJoint_IsLimitEnabled', 'number',
    ['number']);

var b2RevoluteJoint_IsMotorEnabled =
  Module.cwrap('b2RevoluteJoint_IsMotorEnabled', 'number',
    ['number']);

var b2RevoluteJoint_SetMotorSpeed =
  Module.cwrap('b2RevoluteJoint_SetMotorSpeed', 'number',
    ['number', 'number']);

var b2RevoluteJoint_GetReferenceAngle = Module.cwrap('b2RevoluteJoint_GetReferenceAngle', 'number', ['number']);
var b2RevoluteJoint_GetJointSpeed = Module.cwrap('b2RevoluteJoint_GetJointSpeed', 'number', ['number']);
var b2RevoluteJoint_GetLowerLimit = Module.cwrap('b2RevoluteJoint_GetLowerLimit', 'number', ['number']);
var b2RevoluteJoint_GetUpperLimit = Module.cwrap('b2RevoluteJoint_GetUpperLimit', 'number', ['number']);
var b2RevoluteJoint_SetLimits = Module.cwrap('b2RevoluteJoint_SetLimits', 'null', ['number', 'number', 'number']);
var b2RevoluteJoint_GetMotorSpeed = Module.cwrap('b2RevoluteJoint_GetMotorSpeed', 'number', ['number']);
var b2RevoluteJoint_SetMaxMotorTorque = Module.cwrap('b2RevoluteJoint_SetMaxMotorTorque', 'null', ['number', 'number']);
var b2RevoluteJoint_GetMaxMotorTorque = Module.cwrap('b2RevoluteJoint_GetMaxMotorTorque', 'number', ['number']);
var b2RevoluteJoint_GetMotorTorque = Module.cwrap('b2RevoluteJoint_GetMotorTorque', 'number', ['number']);

/** @constructor */
function b2RevoluteJoint(revoluteJointDef) {
  b2Joint.call(this, revoluteJointDef);
  this.collideConnected = revoluteJointDef.collideConnected;
  this.enableLimit = revoluteJointDef.enableLimit;
  this.enableMotor = revoluteJointDef.enableMotor;
  this.lowerAngle = revoluteJointDef.lowerAngle;
  this.maxMotorTorque = revoluteJointDef.maxMotorTorque;
  this.motorSpeed = revoluteJointDef.motorSpeed;
  this.upperAngle = revoluteJointDef.upperAngle;
}
b2RevoluteJoint.prototype = Object.create(b2Joint.prototype);
b2RevoluteJoint.prototype.constructor = b2RevoluteJoint;

b2RevoluteJoint.prototype.EnableLimit = function(flag) {
  b2RevoluteJoint_EnableLimit(this.ptr, flag);
};

b2RevoluteJoint.prototype.EnableMotor = function(flag) {
  b2RevoluteJoint_EnableMotor(this.ptr, flag);
};

b2RevoluteJoint.prototype.GetJointAngle = function(flag) {
  return b2RevoluteJoint_GetJointAngle(this.ptr);
};

b2RevoluteJoint.prototype.IsLimitEnabled = function() {
  return b2RevoluteJoint_IsLimitEnabled(this.ptr);
};

b2RevoluteJoint.prototype.IsMotorEnabled = function() {
  return b2RevoluteJoint_IsMotorEnabled(this.ptr);
};

b2RevoluteJoint.prototype.SetMotorSpeed = function(speed) {
  b2RevoluteJoint_SetMotorSpeed(this.ptr, speed);
  this.motorSpeed = speed;
};

b2RevoluteJoint.prototype.GetReferenceAngle = function() {
    return b2RevoluteJoint_GetReferenceAngle(this.ptr);
}
b2RevoluteJoint.prototype.GetJointSpeed = function() {
    return b2RevoluteJoint_GetJointSpeed(this.ptr);
}
b2RevoluteJoint.prototype.GetLowerLimit = function() {
    return b2RevoluteJoint_GetLowerLimit(this.ptr);
}
b2RevoluteJoint.prototype.GetUpperLimit = function() {
    return b2RevoluteJoint_GetUpperLimit(this.ptr);
}
b2RevoluteJoint.prototype.SetLimits = function(lower, upper) {
    b2RevoluteJoint_SetLimits(this.ptr, lower, upper);
}
b2RevoluteJoint.prototype.GetMotorSpeed = function() {
    return b2RevoluteJoint_GetMotorSpeed(this.ptr);
}
b2RevoluteJoint.prototype.SetMaxMotorTorque = function(torque) {
    b2RevoluteJoint_SetMaxMotorTorque(this.ptr, torque);
}
b2RevoluteJoint.prototype.GetMaxMotorTorque = function() {
    return b2RevoluteJoint_GetMaxMotorTorque(this.ptr);
}
b2RevoluteJoint.prototype.GetMotorTorque = function() {
    return b2RevoluteJoint_GetMotorTorque(this.ptr);
}


var b2RevoluteJointDef_Create =
  Module.cwrap('b2RevoluteJointDef_Create', 'number',
    ['number',
    //joint def
    'number', 'number', 'number',
    // revolute joint def
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number']);

var b2RevoluteJointDef_InitializeAndCreate =
  Module.cwrap('b2RevoluteJointDef_InitializeAndCreate', 'number',
    ['number',
      //initialize args
     'number', 'number', 'number',
     'number',
      //revoluteJointDef
     'number', 'number', 'number',
     'number', 'number', 'number',
     'number']);

/** @constructor */
function b2RevoluteJointDef() {
  this.collideConnected = false;
  this.enableLimit = false;
  this.enableMotor = false;
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.lowerAngle = 0;
  this.maxMotorTorque = 0;
  this.motorSpeed = 0;
  this.referenceAngle = 0;
  this.upperAngle = 0;
  this.userData = null;
}

b2RevoluteJointDef.prototype.Create = function(world) {
  var revoluteJoint = new b2RevoluteJoint(this);
  revoluteJoint.ptr = b2RevoluteJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //revoluteJointDef
    this.enableLimit, this.enableMotor, this.lowerAngle,
    this.localAnchorA.x, this.localAnchorA.y, this.localAnchorB.x,
    this.localAnchorB.y, this.maxMotorTorque, this.motorSpeed,
    this.referenceAngle, this.upperAngle);
  return revoluteJoint;
};

// todo Initialize and create probably shouldnt use the global world ptr
b2RevoluteJointDef.prototype.InitializeAndCreate = function(bodyA, bodyB, anchor) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var revoluteJoint = new b2RevoluteJoint(this);
  revoluteJoint.ptr =
    b2RevoluteJointDef_InitializeAndCreate(world.ptr,
      // initialize args
      bodyA.ptr, bodyB.ptr, anchor.x,
      anchor.y,
      // joint def
      this.collideConnected,
      // revloute joint def
      this.enableLimit, this.enableMotor, this.lowerAngle,
      this.maxMotorTorque, this.motorSpeed, this.upperAngle);
  b2World._Push(revoluteJoint, world.joints);
  return revoluteJoint;
};