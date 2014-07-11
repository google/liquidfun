// TODO Rope should probably be removed
function TestRope() {
  var N = 40;
  var def = new b2RopeDef;

  for (var i = 0; i < N; ++i) {
    def.vertices.push(new b2Vec2(0.0, 20.0 - 0.25 * i));
    def.masses.push(1.0);
  }
  def.masses[0] = 0.0;
  def.masses[1] = 0.0;

  def.count = N;
  def.gravity.Set(0.0, -10.0);
  def.damping = 0.1;
  def.k2 = 1.0;
  def.k3 = 0.5;

  this.rope.Initialize(def);

  this.angle = 0.0;
  this.rope.SetAngle(this.angle);
}

TestRope.prototype.Keyboard = function(key) {
  switch (key) {
    case 'q':
      this.angle = Math.max(-Math.PI, this.angle - 0.5 * Math.PI);
      this.rope.SetAngle(this.angle);
      break;
    case 'e':
      this.angle = Math.min(Math.PI, this.angle + 0.5 * Math.PI);
      this.rope.SetAngle(this.angle);
      break;
  }
};

TestRope.prototype.Step = function() {
  this.rope.Step(1/60, 1);
  Step();
};