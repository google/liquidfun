// Shape constants
var b2Shape_Type_e_circle = 0;
var b2Shape_Type_e_edge = 1;
var b2Shape_Type_e_polygon = 2;
var b2Shape_Type_e_chain = 3;
var b2Shape_Type_e_typeCount = 4;
var b2_linearSlop = 0.005;
var b2_polygonRadius = 2 * b2_linearSlop;
var b2_maxPolygonVertices = 8;

function b2MassData(mass, center, I) {
  this.mass = mass;
  this.center = center;
  this.I = I;
}