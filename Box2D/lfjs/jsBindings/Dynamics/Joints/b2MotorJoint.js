var b2MotorJoint_SetAngularOffset =
  Module.cwrap('b2MotorJoint_SetAngularOffset', 'null', ['number', 'number']);

var b2MotorJoint_SetLinearOffset =
  Module.cwrap('b2MotorJoint_SetLinearOffset', 'null',
    ['number', 'number', 'number']);

/**@constructor*/
function b2MotorJoint(def) {
  this.bodyA = def.bodyA;
  this.bodyB = def.bodyB;
  this.ptr = null;
  this.next = null;
}

b2MotorJoint.prototype.SetAngularOffset = function(angle) {
  b2MotorJoint_SetAngularOffset(this.ptr, angle);
};

b2MotorJoint.prototype.SetLinearOffset = function(v) {
  b2MotorJoint_SetLinearOffset(this.ptr, v.x, v.y);
};

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
