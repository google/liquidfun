#include<Box2D/Box2D.h>

void* b2Joint_GetBodyA(void* joint) {
  return ((b2Joint*)joint)->GetBodyA();
}
void* b2Joint_GetBodyB(void* joint) {
  return ((b2Joint*)joint)->GetBodyB();
}
double b2Joint_GetType(void* joint) {
  return ((b2Joint*)joint)->GetType();
}
void b2Joint_GetAnchorA(void* joint, float* arr) {
  b2Vec2 anchor = ((b2Joint*)joint)->GetAnchorA();
  arr[0] = anchor.x;
  arr[1] = anchor.y;
}
void b2Joint_GetAnchorB(void* joint, float* arr) {
  b2Vec2 anchor = ((b2Joint*)joint)->GetAnchorB();
  arr[0] = anchor.x;
  arr[1] = anchor.y;
}
