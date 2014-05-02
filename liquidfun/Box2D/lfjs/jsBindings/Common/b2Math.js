var FLT_EPSILON = 1.19209290E-07;

function b2Max(a ,b) {
  return new b2Vec2(Math.max(a.x, b.x), Math.max(a.y, b.y));
}

function b2Min(a, b) {
  return new b2Vec2(Math.min(a.x, b.x), Math.min(a.y, b.y));
}

function b2Clamp(a, low, high) {
  return b2Max(low, b2Min(a, high));
}


/** @constructor */
function b2Vec2(x, y) {
  if (x === undefined) {
    x = 0;
  }
  if (y === undefined) {
    y = 0;
  }
  this.x = x;
  this.y = y;
}

// static functions on b2Vec2
b2Vec2.Add = function(out, a, b) {
  out.x = a.x + b.x;
  out.y = a.y + b.y;
};

b2Vec2.CrossScalar = function(output, input, scalar) {
  output.x = -scalar * input.y;
  output.y =  scalar * input.x;
};

b2Vec2.Cross = function(a, b) {
  return a.x * b.y - a.y * b.x;
};

b2Vec2.MulScalar = function(out, input, scalar) {
  out.x = input.x * scalar;
  out.y = input.y * scalar;
};

b2Vec2.Mul = function(out, T, v) {
  var Tp = T.p;
  var Tqc = T.q.c;
  var Tqs = T.q.s;

  var x = v.x;
  var y = v.y;

  out.x = (Tqc * x - Tqs * y) + Tp.x;
  out.y = (Tqs * x + Tqc * y) + Tp.y;
};

b2Vec2.Normalize = function(out, input) {
  var length = input.Length();
  if (length < FLT_EPSILON) {
    out.x = 0;
    out.y = 0;
    return;
  }
  var invLength = 1.0 / length;
  out.x = input.x * invLength;
  out.y = input.y * invLength;
};

b2Vec2.Sub = function(out, input, subtract) {
  out.x = input.x - subtract.x;
  out.y = input.y - subtract.y;
};

b2Vec2.prototype.Clone = function() {
  return new b2Vec2(this.x, this.y);
};

b2Vec2.prototype.Set = function(x, y) {
  this.x = x;
  this.y = y;
};

b2Vec2.prototype.Length = function() {
  var x = this.x;
  var y = this.y;
  return Math.sqrt(x * x + y * y);
};

b2Vec2.prototype.LengthSquared = function() {
  var x = this.x;
  var y = this.y;
  return x * x + y * y;
};

/** @constructor */
function b2Rot(radians) {
  if (radians === undefined) {
    radians = 0;
  }
  this.s = Math.sin(radians);
  this.c = Math.cos(radians);
}

b2Rot.prototype.Set = function(radians) {
  this.s = Math.sin(radians);
  this.c = Math.cos(radians);
};

b2Rot.prototype.SetIdentity = function() {
  this.s = 0;
  this.c = 1;
};

b2Rot.prototype.GetXAxis = function() {
  return new b2Vec2(this.c, this.s);
};

/** @constructor */
function b2Transform(position, rotation) {
  if (position === undefined) {
    position = new b2Vec2();
  }
  if (rotation === undefined) {
    rotation = new b2Rot();
  }
  this.p = position;
  this.q = rotation;
}

b2Transform.prototype.FromFloat64Array = function(arr) {
  var p = this.p;
  var q = this.q;
  p.x = arr[0];
  p.y = arr[1];
  q.s = arr[2];
  q.c = arr[3];
};

b2Transform.prototype.SetIdentity = function() {
  this.p.Set(0, 0);
  this.q.SetIdentity();
};