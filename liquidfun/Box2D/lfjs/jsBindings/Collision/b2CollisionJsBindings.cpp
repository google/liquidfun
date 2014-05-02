#include <Box2D/Box2D.h>
double b2Manifold_GetPointCount(void* manifold) {
  return ((b2Manifold*)manifold)->pointCount;
}
