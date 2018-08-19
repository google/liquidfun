var b2PrismaticJoint_EnableLimit =
  Module.cwrap('b2PrismaticJoint_EnableLimit', 'number', ['number', 'number']);
var b2PrismaticJoint_EnableMotor =
  Module.cwrap('b2PrismaticJoint_EnableMotor', 'number', ['number', 'number']);
var b2PrismaticJoint_GetJointTranslation =
  Module.cwrap('b2PrismaticJoint_GetJointTranslation', 'number', ['number']);
var b2PrismaticJoint_GetMotorSpeed =
  Module.cwrap('b2PrismaticJoint_GetMotorSpeed', 'number', ['number']);
var b2PrismaticJoint_GetMotorForce =
  Module.cwrap('b2PrismaticJoint_GetMotorForce', 'number', ['number', 'number']);
var b2PrismaticJoint_IsLimitEnabled =
  Module.cwrap('b2PrismaticJoint_IsLimitEnabled', 'number', ['number']);
var b2PrismaticJoint_IsMotorEnabled =
  Module.cwrap('b2PrismaticJoint_IsMotorEnabled', 'number', ['number']);
var b2PrismaticJoint_SetMotorSpeed =
  Module.cwrap('b2PrismaticJoint_SetMotorSpeed', 'number', ['number', 'number']);

var b2PrismaticJoint_GetLocalAxisA = Module.cwrap('b2PrismaticJoint_GetLocalAxisA', 'null', ['number', 'number']);
var b2PrismaticJoint_GetReferenceAngle = Module.cwrap('b2PrismaticJoint_GetReferenceAngle', 'number', ['number']);
var b2PrismaticJoint_GetJointSpeed = Module.cwrap('b2PrismaticJoint_GetJointSpeed', 'number', ['number']);
var b2PrismaticJoint_GetLowerLimit = Module.cwrap('b2PrismaticJoint_GetLowerLimit', 'number', ['number']);
var b2PrismaticJoint_GetUpperLimit = Module.cwrap('b2PrismaticJoint_GetUpperLimit', 'number', ['number']);
var b2PrismaticJoint_SetLimits = Module.cwrap('b2PrismaticJoint_SetLimits', 'null', ['number', 'number', 'number']);
var b2PrismaticJoint_SetMaxMotorForce = Module.cwrap('b2PrismaticJoint_SetMaxMotorForce', 'null', ['number', 'null']);
var b2PrismaticJoint_GetMaxMotorForce = Module.cwrap('b2PrismaticJoint_GetMaxMotorForce', 'number', ['number']);

/**@constructor*/
function b2PrismaticJoint(def) {
  b2Joint.call(this, def);
}
b2PrismaticJoint.prototype = Object.create(b2Joint.prototype);
b2PrismaticJoint.prototype.constructor = b2PrismaticJoint;

b2PrismaticJoint.prototype.EnableLimit = function(flag) {
  return b2PrismaticJoint_EnableLimit(this.ptr, flag);
};

b2PrismaticJoint.prototype.EnableMotor = function(flag) {
  return b2PrismaticJoint_EnableMotor(this.ptr, flag);
};

b2PrismaticJoint.prototype.GetJointTranslation = function() {
  return b2PrismaticJoint_GetJointTranslation(this.ptr);
};

b2PrismaticJoint.prototype.GetMotorSpeed = function() {
  return b2PrismaticJoint_GetMotorSpeed(this.ptr);
};

b2PrismaticJoint.prototype.GetMotorForce = function(inv_dt) {
  return b2PrismaticJoint_GetMotorForce(this.ptr, inv_dt);
};

b2PrismaticJoint.prototype.IsLimitEnabled = function() {
  return b2PrismaticJoint_IsLimitEnabled(this.ptr);
};

b2PrismaticJoint.prototype.IsMotorEnabled = function() {
  return b2PrismaticJoint_IsMotorEnabled(this.ptr);
};

b2PrismaticJoint.prototype.GetMotorEnabled = function() {
  return b2PrismaticJoint_IsMotorEnabled(this.ptr);
};

b2PrismaticJoint.prototype.SetMotorSpeed = function(speed) {
  return b2PrismaticJoint_SetMotorSpeed(this.ptr, speed);
};

b2PrismaticJoint.prototype.GetLocalAxisA = function() {
    b2PrismaticJoint_GetLocalAxisA(this.ptr, _vec2Buf.byteOffset);
    var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
    return new b2Vec2(result[0], result[1]);
}
b2PrismaticJoint.prototype.GetReferenceAngle = function() {
    return b2PrismaticJoint_GetReferenceAngle(this.ptr);
}
b2PrismaticJoint.prototype.GetJointSpeed = function() {
    return b2PrismaticJoint_GetJointSpeed(this.ptr);
}
b2PrismaticJoint.prototype.GetLowerLimit = function() {
    return b2PrismaticJoint_GetLowerLimit(this.ptr);
}
b2PrismaticJoint.prototype.GetUpperLimit = function() {
    return b2PrismaticJoint_GetUpperLimit(this.ptr);
}
b2PrismaticJoint.prototype.SetLimits = function(lower, upper) {
    b2PrismaticJoint_SetLimits(this.ptr, lower, upper);
}
b2PrismaticJoint.prototype.SetMaxMotorForce = function(force) {
    b2PrismaticJoint_SetMaxMotorForce(this.ptr, force);
}
b2PrismaticJoint.prototype.GetMaxMotorForce = function() {
    return b2PrismaticJoint_GetMaxMotorForce(this.ptr);
}

var b2PrismaticJointDef_Create = Module.cwrap("b2PrismaticJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // prismatic joint def
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number']);

var b2PrismaticJointDef_InitializeAndCreate = Module.cwrap("b2PrismaticJointDef_InitializeAndCreate",
  'number',
  ['number',
    // initialize args
    'number', 'number', 'number',
    'number', 'number', 'number',
    // joint def
    'number',
    // prismatic joint def
    'number', 'number', 'number',
    'number', 'number', 'number']);

/**@constructor*/
function b2PrismaticJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;

  // prismatic joint def
  this.enableLimit = false;
  this.enableMotor = false;
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.localAxisA = new b2Vec2(1, 0);
  this.lowerTranslation = 0;
  this.maxMotorForce = 0;
  this.motorSpeed = 0;
  this.referenceAngle = 0;
  this.upperTranslation = 0;
}

b2PrismaticJointDef.prototype.Create = function(world) {
  var prismaticJoint = new b2PrismaticJoint(this);
  prismaticJoint.ptr = b2PrismaticJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //prismatic joint def
    this.enableLimit, this.enableMotor, this.localAnchorA.x,
    this.localAnchorA.y, this.localAnchorB.x, this.localAnchorB.y,
    this.localAxisA.x, this.localAxisA.y, this.lowerTranslation,
    this.maxMotorForce, this.motorSpeed, this.referenceAngle,
    this.upperTranslation);
  return prismaticJoint;
};

b2PrismaticJointDef.prototype.InitializeAndCreate  = function(bodyA, bodyB, anchor, axis) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var prismaticJoint = new b2PrismaticJoint(this);
  prismaticJoint.ptr = b2PrismaticJointDef_InitializeAndCreate(
    world.ptr,
    // InitializeArgs
    this.bodyA.ptr, this.bodyB.ptr, anchor.x,
    anchor.y, axis.x, axis.y,
    // joint def
    this.collideConnected,
    // prismatic joint def
    this.enableLimit, this.enableMotor, this.lowerTranslation,
    this.maxMotorForce, this.motorSpeed, this.upperTranslation);
  b2World._Push(prismaticJoint, world.joints);
  return prismaticJoint;
};