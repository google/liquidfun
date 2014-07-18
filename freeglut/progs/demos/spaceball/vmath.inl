/* vector functions */
static INLINE vec3_t v3_cons(float x, float y, float z)
{
  vec3_t res;
  res.x = x;
  res.y = y;
  res.z = z;
  return res;
}

static INLINE vec3_t quat_vec(quat_t q)
{
  vec3_t v;
  v.x = q.x;
  v.y = q.y;
  v.z = q.z;
  return v;
}

static INLINE float v3_dot(vec3_t v1, vec3_t v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/* quaternion functions */
static INLINE quat_t quat_cons(float s, float x, float y, float z)
{
  quat_t q;
  q.x = x;
  q.y = y;
  q.z = z;
  q.w = s;
  return q;
}

static INLINE quat_t quat_mul(quat_t q1, quat_t q2)
{
  quat_t res;
  vec3_t v1 = quat_vec(q1);
  vec3_t v2 = quat_vec(q2);

  res.w = q1.w * q2.w - v3_dot(v1, v2);
  res.x = v2.x * q1.w + v1.x * q2.w + (v1.y * v2.z - v1.z * v2.y);
  res.y = v2.y * q1.w + v1.y * q2.w + (v1.z * v2.x - v1.x * v2.z);
  res.z = v2.z * q1.w + v1.z * q2.w + (v1.x * v2.y - v1.y * v2.x);
  return res;
}

static INLINE void quat_to_mat(mat4_t res, quat_t q)
{
  m4_cons(res,  1.0f - 2.0f * q.y*q.y - 2.0f * q.z*q.z,  2.0f * q.x * q.y + 2.0f * q.w * q.z,    2.0f * q.z * q.x - 2.0f * q.w * q.y, 0,
          2.0f * q.x * q.y - 2.0f * q.w * q.z,    1.0f - 2.0f * q.x*q.x - 2.0f * q.z*q.z,  2.0f * q.y * q.z + 2.0f * q.w * q.x, 0,
          2.0f * q.z * q.x + 2.0f * q.w * q.y,    2.0f * q.y * q.z - 2.0f * q.w * q.x,    1.0f - 2.0f * q.x*q.x - 2.0f * q.y*q.y, 0,
          0, 0, 0, 1);
}

/* matrix functions */
static INLINE void m4_cons(mat4_t m,
    float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44)
{
  m[0][0] = m11; m[1][0] = m12; m[2][0] = m13; m[3][0] = m14;
  m[0][1] = m21; m[1][1] = m22; m[2][1] = m23; m[3][1] = m24;
  m[0][2] = m31; m[1][2] = m32; m[2][2] = m33; m[3][2] = m34;
  m[0][3] = m41; m[1][3] = m42; m[2][3] = m43; m[3][3] = m44;
}
