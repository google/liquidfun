/**@constructor*/
function b2PulleyJoint(def) {
  this.ptr = null;
  this.next = null;
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
