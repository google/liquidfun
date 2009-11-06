#ifndef VMATH_H_
#define VMATH_H_

typedef struct { float x, y, z; } vec3_t;
typedef struct { float x, y, z, w; } vec4_t;

typedef vec4_t quat_t;

typedef float mat4_t[4][4];

/* vector functions */
static inline vec3_t v3_cons(float x, float y, float z);
static inline float v3_dot(vec3_t v1, vec3_t v2);

/* quaternion functions */
static inline quat_t quat_cons(float s, float x, float y, float z);
static inline vec3_t quat_vec(quat_t q);
static inline quat_t quat_mul(quat_t q1, quat_t q2);
static inline void quat_to_mat(mat4_t res, quat_t q);
quat_t quat_rotate(quat_t q, float angle, float x, float y, float z);

/* matrix functions */
static inline void m4_cons(mat4_t m,
		float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);

#include "vmath.inl"

#endif	/* VMATH_H_ */
