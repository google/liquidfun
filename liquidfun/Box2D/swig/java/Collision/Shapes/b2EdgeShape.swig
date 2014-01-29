%module b2EdgeShape

/// Allow a constructor to be generated despite inheriting from an abstract
/// class with no default constructors
%feature("notabstract") b2EdgeShape;

/// b2EdgeShape class
class b2EdgeShape : public b2Shape {
public:
    b2EdgeShape();
};

/// b2EdgeShape extensions for easier Java access
%extend b2EdgeShape {
    void Set(float32 vx1, float32 vy1, float32 vx2, float32 vy2) {
        $self->Set(b2Vec2(vx1, vy1), b2Vec2(vx2, vy2));
    }
};