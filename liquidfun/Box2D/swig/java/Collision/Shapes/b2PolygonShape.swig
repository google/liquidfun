%module b2PolygonShape

/// Allow a constructor to be generated despite inheriting from an abstract
/// class with no default constructors
%feature("notabstract") b2PolygonShape;

class b2PolygonShape : public b2Shape {
public:
    b2PolygonShape();

    void SetAsBox(float32 hx, float32 hy);
};

/// b2PolygonShape extensions for easier Java access
%extend b2PolygonShape {
    void SetCentroid(float32 x, float32 y) {
        $self->m_centroid.Set(x, y);
    }

    void SetAsBox(
            float32 hx,
            float32 hy,
            float32 centerX,
            float32 centerY,
            float32 angle) {
        $self->SetAsBox(hx, hy, b2Vec2(centerX, centerY), angle);
    }
};
