// todo see if switching to function ptrs is faster
mergeInto(LibraryManager.library, {
  b2WorldBeginContactBody: function(contactPtr) {
    b2World.BeginContactBody(contactPtr);
  },
  b2WorldEndContactBody: function(contactPtr) {
    b2World.EndContactBody(contactPtr);
  },
  b2WorldPreSolve: function(contactPtr, oldManifoldPtr) {
    b2World.PreSolve(contactPtr, oldManifoldPtr);
  },
  b2WorldPostSolve: function(contactPtr, impulsePtr) {
    b2World.PostSolve(contactPtr, impulsePtr);
  },
  b2WorldQueryAABB: function(fixturePtr) {
    return b2World.QueryAABB(fixturePtr);
  },
  b2WorldRayCastCallback: function(fixturePtr, pointX, pointY,
                                   normalX, normalY, fraction) {
    return b2World.RayCast(fixturePtr, pointX, pointY, normalX, normalY, fraction);
  }
});