%module b2Body

/// b2FixtureDef class
struct b2FixtureDef {
    const b2Shape* shape;
};

%nodefaultctor;      // don't create default constructors
%nodefaultdtor;      // don't create default destructors
/// b2Fixture class
class b2Fixture {
};
%clearnodefaultdtor; // Re-enable default constructors
%clearnodefaultctor; // Re-enable default destructors
