#include <Box2D/Box2D.h>

// global for fetching the worldManifold
b2WorldManifold worldManifold;

void* b2Contact_GetManifold(void* contact) {
  return ((b2Contact*)contact)->GetManifold();
}

void* b2Contact_GetWorldManifold(void* contact) {
  ((b2Contact*)contact)->GetWorldManifold(&worldManifold);
  return &worldManifold;
}
