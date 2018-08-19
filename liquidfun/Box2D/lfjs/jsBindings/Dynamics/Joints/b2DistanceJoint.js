var b2DistanceJointDef_SetLength = Module.cwrap("b2DistanceJointDef_SetLength", "null", ["number", "number"]);
var b2DistanceJointDef_GetLength = Module.cwrap("b2DistanceJointDef_GetLength", "number", ["number"]);
var b2DistanceJointDef_SetFrequency = Module.cwrap("b2DistanceJointDef_SetFrequency", "null", ["number", "number"]);
var b2DistanceJointDef_GetFrequency = Module.cwrap("b2DistanceJointDef_GetFrequency", "number", ["number"]);
var b2DistanceJointDef_SetDampingRatio = Module.cwrap("b2DistanceJointDef_SetDampingRatio", "null", ["number", "number"]);
var b2DistanceJointDef_GetDampingRatio = Module.cwrap("b2DistanceJointDef_GetDampingRatio", "number", ["number"]);

/**@constructor*/
function b2DistanceJoint(def) {
  b2Joint.call(this, def);
}
b2DistanceJoint.prototype = Object.create(b2Joint.prototype);
b2DistanceJoint.prototype.constructor = b2DistanceJoint;

b2DistanceJoint.prototype.SetLength = function(length) {
    b2DistanceJointDef_SetLength(this.prt, length);
}
b2DistanceJoint.prototype.GetLength = function() {
    return b2DistanceJointDef_GetLength(this.prt);
}

b2DistanceJoint.prototype.SetFrequency = function(hz) {
    b2DistanceJointDef_SetFrequency(this.prt, hz);
}
b2DistanceJoint.prototype.GetLength = function() {
    return b2DistanceJointDef_GetLength(this.prt);
}

b2DistanceJoint.prototype.SetDampingRatio = function(ratio) {
    b2DistanceJointDef_SetDampingRatio(this.prt, ratio);
}
b2DistanceJoint.prototype.GetDampingRatio = function() {
    return b2DistanceJointDef_GetDampingRatio(this.prt);
}

var b2DistanceJointDef_Create = Module.cwrap("b2DistanceJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // distance joint def
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number']);

var b2DistanceJointDef_InitializeAndCreate = Module.cwrap("b2DistanceJointDef_InitializeAndCreate",
  'number',
  ['number',
    // initialize args
    'number', 'number',
    'number', 'number',
    'number', 'number',
    // distance joint def
    'number', 'number', 'number']);

/**@constructor*/
function b2DistanceJointDef() {
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = false;
  this.dampingRatio = 0;
  this.length = 1;
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.frequencyHz = 0;
}

b2DistanceJointDef.prototype.Create = function(world) {
  var distanceJoint = new b2DistanceJoint(this);
  distanceJoint.ptr = b2DistanceJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    //distance joint def
    this.dampingRatio, this.frequencyHz, this.length,
    this.localAnchorA.x, this.localAnchorA.y,
    this.localAnchorB.x, this.localAnchorB.y);
  return distanceJoint;
};

b2DistanceJointDef.prototype.InitializeAndCreate  = function(bodyA, bodyB, anchorA, anchorB) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var distanceJoint = new b2DistanceJoint(this);
  distanceJoint.ptr = b2DistanceJointDef_InitializeAndCreate(
    world.ptr,
    // InitializeArgs
    this.bodyA.ptr, this.bodyB.ptr,
    anchorA.x, anchorA.y,
    anchorB.x, anchorB.y,
    // joint def
    this.collideConnected,
    //distance joint def
    this.dampingRatio, this.frequencyHz);
  b2World._Push(distanceJoint, world.joints);
  return distanceJoint;
};