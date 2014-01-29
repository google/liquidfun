%module b2Math

/// b2World struct
struct b2Vec2 {
    b2Vec2();
    b2Vec2(float32 x, float32 y);

    void Set(float32 x_, float32 y_);

    float32 x, y;
};

/// b2Transform struct
struct b2Transform {
    b2Transform();

    void SetIdentity();
};

/// b2Transform extensions for easier Java access
%extend b2Transform {
    float32 getPosX() {
        return $self->p.x;
    }
    float32 getPosY() {
        return $self->p.y;
    }
    float32 getRotSin() {
        return $self->q.s;
    }
    float32 getRotCos() {
        return $self->q.c;
    }
}
