var b2PulleyJoint_GetGroundAnchorA = Module.cwrap('b2PulleyJoint_GetGroundAnchorA', 'null', ['number', 'number']);
var b2PulleyJoint_GetGroundAnchorB = Module.cwrap('b2PulleyJoint_GetGroundAnchorB', 'null', ['number', 'number']);
var b2PulleyJoint_GetCurrentLengthA = Module.cwrap('b2PulleyJoint_GetCurrentLengthA', 'number', ['number']);
var b2PulleyJoint_GetCurrentLengthB = Module.cwrap('b2PulleyJoint_GetCurrentLengthB', 'number', ['number']);

/**@constructor*/
function b2PulleyJoint(def) {
  b2Joint.call(this, def);
  this.lengthA = def.lengthA;
  this.lengthB = def.lengthB;
  this.ratio = def.ratio;
}
b2PulleyJoint.prototype = Object.create(b2Joint.prototype);
b2PulleyJoint.prototype.constructor = b2PulleyJoint;

b2PulleyJoint.prototype.GetGroundAnchorA = function() {
    b2PulleyJoint_GetGroundAnchorA(this.ptr, _vec2Buf.byteOffset);
    var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
    return new b2Vec2(result[0], result[1]);
}
b2PulleyJoint.prototype.GetGroundAnchorB = function() {
    b2PulleyJoint_GetGroundAnchorB(this.ptr, _vec2Buf.byteOffset);
    var result = new Float32Array(_vec2Buf.buffer, _vec2Buf.byteOffset, _vec2Buf.length);
    return new b2Vec2(result[0], result[1]);
}
b2PulleyJoint.prototype.GetCurrentLengthA = function() {
    return b2PulleyJoint_GetCurrentLengthA(this.ptr);
}
b2PulleyJoint.prototype.GetCurrentLengthB = function() {
    return b2PulleyJoint_GetCurrentLengthB(this.ptr);
}

var b2PulleyJointDef_Create = Module.cwrap("b2PulleyJointDef_Create",
  'number',
  ['number',
    // joint Def
    'number', 'number', 'number',
    // pulley joint def
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number', 'number',
    'number', 'number']);

var b2PulleyJointDef_InitializeAndCreate = Module.cwrap("b2PulleyJointDef_InitializeAndCreate",
  'number',
  ['number',
    // initialize args
    'number', 'number', 'number',
    'number', 'number', 'number',
    // joint def
    'number',
    // pulley joint def
    'number', 'number', 'number',
    'number', 'number', 'number']);

/**@constructor*/
function b2PulleyJointDef() {
  // joint def
  this.bodyA = null;
  this.bodyB = null;
  this.collideConnected = true;

  // pulley joint def
  this.groundAnchorA = new b2Vec2();
  this.groundAnchorB = new b2Vec2();
  this.localAnchorA = new b2Vec2();
  this.localAnchorB = new b2Vec2();
  this.lengthA = 0;
  this.lengthB = 0;
  this.ratio = 1;
}

b2PulleyJointDef.prototype.Create = function(world) {
  var pulleyJoint = new b2PulleyJoint(this);
  pulleyJoint.ptr = b2PulleyJointDef_Create(
    world.ptr,
    // joint def
    this.bodyA.ptr, this.bodyB.ptr, this.collideConnected,
    // pulley joint def
    this.groundAnchorA.x, this.groundAnchorA.y, this.groundAnchorB.x,
    this.groundAnchorB.y, this.lengthA, this.lengthB,
    this.localAnchorA.x, this.localAnchorA.y, this.localAnchorB.x,
    this.localAnchorB.y, this.ratio);
  return pulleyJoint;
};

b2PulleyJointDef.prototype.InitializeAndCreate  = function(bodyA, bodyB, groundAnchorA,
                                                           groundAnchorB, anchorA, anchorB,
                                                           ratio) {
  this.bodyA = bodyA;
  this.bodyB = bodyB;
  var pulleyJoint = new b2PulleyJoint(this);
  pulleyJoint.ptr = b2PulleyJointDef_InitializeAndCreate(
    world.ptr,
    // InitializeArgs
    this.bodyA.ptr, this.bodyB.ptr, anchorA.x,
    anchorA.y, anchorB.x, anchorB.y,
    groundAnchorA.x, groundAnchorA.y, groundAnchorB.x,
    groundAnchorB.y, ratio,
    // joint def
    this.collideConnected);
  b2World._Push(pulleyJoint, world.joints);
  return pulleyJoint;
};
