#include<Box2D/Box2D.h>

void* b2Joint_GetBodyA(void* joint) {
  return ((b2Joint*)joint)->GetBodyA();
}
void* b2Joint_GetBodyB(void* joint) {
  return ((b2Joint*)joint)->GetBodyB();
}
