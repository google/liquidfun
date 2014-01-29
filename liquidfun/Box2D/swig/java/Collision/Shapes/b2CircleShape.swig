%module b2CircleShape

/// Allow a constructor to be generated despite inheriting from an abstract
/// class with no default constructors
%feature("notabstract") b2CircleShape;

class b2CircleShape : public b2Shape {
public:
    b2CircleShape();
};

// b2CircleShape extensions for easier Java access
%extend b2CircleShape {
    void SetPosition(float32 x, float32 y) {
        $self->m_p.Set(x, y);
    }

    float32 GetPosX() {
        return $self->m_p.x;
    }

    float32 GetPosY() {
        return $self->m_p.y;
    }
};
