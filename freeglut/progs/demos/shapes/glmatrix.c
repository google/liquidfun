#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "glmatrix.h"

#define MMODE_IDX(x)	((x) - GL_MODELVIEW)
#define MAT_STACK_SIZE	32
#define MAT_IDENT	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}

static int mm_idx = 0;
static float mat_stack[3][MAT_STACK_SIZE][16] = {{MAT_IDENT}, {MAT_IDENT}, {MAT_IDENT}};
static int stack_top[3];

void gl_matrix_mode(int mm)
{
    mm_idx = MMODE_IDX(mm);
}

void gl_push_matrix(void)
{
    int top = stack_top[mm_idx];

    memcpy(mat_stack[mm_idx][top + 1], mat_stack[mm_idx][top], 16 * sizeof(float));
    stack_top[mm_idx]++;
}

void gl_pop_matrix(void)
{
    stack_top[mm_idx]--;
}

void gl_load_identity(void)
{
    static const float idmat[] = MAT_IDENT;
    int top = stack_top[mm_idx];
    float *mat = mat_stack[mm_idx][top];

    memcpy(mat, idmat, sizeof idmat);
}

void gl_load_matrixf(const float *m)
{
    int top = stack_top[mm_idx];
    float *mat = mat_stack[mm_idx][top];

    memcpy(mat, m, 16 * sizeof *mat);
}

#define M4(i, j)	((i << 2) + j)

void gl_mult_matrixf(const float *m2)
{
    int i, j;
    int top = stack_top[mm_idx];
    float *m1 = mat_stack[mm_idx][top];
    float res[16];

    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            res[M4(i,j)] = m1[M4(i,0)] * m2[M4(0,j)] +
                        m1[M4(i,1)] * m2[M4(1,j)] +
                        m1[M4(i,2)] * m2[M4(2,j)] +
                        m1[M4(i,3)] * m2[M4(3,j)];
        }
    }

    memcpy(m1, res, sizeof res);
}

void gl_translatef(float x, float y, float z)
{
    float mat[] = MAT_IDENT;

    mat[12] = x;
    mat[13] = y;
    mat[14] = z;

    gl_mult_matrixf(mat);
}

void gl_rotatef(float angle, float x, float y, float z)
{
    float mat[] = MAT_IDENT;

    float angle_rad = (float)M_PI * angle / 180.f;
    float sina = (float)sin(angle_rad);
    float cosa = (float)cos(angle_rad);
    float one_minus_cosa = 1.f - cosa;
    float nxsq = x * x;
    float nysq = y * y;
    float nzsq = z * z;

    mat[0] = nxsq + (1.f - nxsq) * cosa;
    mat[4] = x * y * one_minus_cosa - z * sina;
    mat[8] = x * z * one_minus_cosa + y * sina;
    mat[1] = x * y * one_minus_cosa + z * sina;
    mat[5] = nysq + (1.f - nysq) * cosa;
    mat[9] = y * z * one_minus_cosa - x * sina;
    mat[2] = x * z * one_minus_cosa - y * sina;
    mat[6] = y * z * one_minus_cosa + x * sina;
    mat[10] = nzsq + (1.f - nzsq) * cosa;

    gl_mult_matrixf(mat);
}

void gl_scalef(float x, float y, float z)
{
    float mat[] = MAT_IDENT;

    mat[0] = x;
    mat[5] = y;
    mat[10] = z;

    gl_mult_matrixf(mat);
}

void gl_ortho(float left, float right, float bottom, float top, float near, float far)
{
    float mat[] = MAT_IDENT;

    float dx = right - left;
    float dy = top - bottom;
    float dz = far - near;

    float tx = -(right + left) / dx;
    float ty = -(top + bottom) / dy;
    float tz = -(far + near) / dz;

    float sx = 2.f / dx;
    float sy = 2.f / dy;
    float sz = -2.f / dz;

    mat[0] = sx;
    mat[5] = sy;
    mat[10] = sz;
    mat[12] = tx;
    mat[13] = ty;
    mat[14] = tz;

    gl_mult_matrixf(mat);
}

void gl_frustum(float left, float right, float bottom, float top, float near, float far)
{
    float mat[] = MAT_IDENT;

    float dx = right - left;
    float dy = top - bottom;
    float dz = far - near;

    float a = (right + left) / dx;
    float b = (top + bottom) / dy;
    float c = -(far + near) / dz;
    float d = -2.f * far * near / dz;

    mat[0] = 2.f * near / dx;
    mat[5] = 2.f * near / dy;
    mat[8] = a;
    mat[9] = b;
    mat[10] = c;
    mat[11] = -1.f;
    mat[14] = d;
    mat[15] = 0;

    gl_mult_matrixf(mat);
}

void glu_perspective(float vfov, float aspect, float near, float far)
{
    float vfov_rad = (float)M_PI * vfov / 180.f;
    float x = near * (float)tan(vfov_rad / 2.f);
    gl_frustum(-aspect * x, aspect * x, -x, x, near, far);
}

/* return the matrix (16 elements, 4x4 matrix, row-major order */
float* get_matrix(int mm)
{
    int idx = MMODE_IDX(mm);
    int top = stack_top[idx];
    return mat_stack[idx][top];
}


#define M3(i, j)	((i * 3) + j)
static float inv_transpose_result[9];

/* return the inverse transpose of the left-upper 3x3 of a matrix
   The returned pointer is only valid until the next time this function is
   called, so make a deep copy when you want to keep it around.
 */
float* get_inv_transpose_3x3(int mm)
{
    int idx = MMODE_IDX(mm);
    int top = stack_top[idx];
    float *m1 = mat_stack[idx][top];
    

    float determinant = +m1[M4(0,0)]*(m1[M4(1,1)]*m1[M4(2,2)]-m1[M4(2,1)]*m1[M4(1,2)])
                        -m1[M4(0,1)]*(m1[M4(1,0)]*m1[M4(2,2)]-m1[M4(1,2)]*m1[M4(2,0)])
                        +m1[M4(0,2)]*(m1[M4(1,0)]*m1[M4(2,1)]-m1[M4(1,1)]*m1[M4(2,0)]);

    float invdet = 1/determinant;

    inv_transpose_result[M3(0,0)] =  (m1[M4(1,1)]*m1[M4(2,2)]-m1[M4(2,1)]*m1[M4(1,2)])*invdet;
    inv_transpose_result[M3(1,0)] = -(m1[M4(0,1)]*m1[M4(2,2)]-m1[M4(0,2)]*m1[M4(2,1)])*invdet;
    inv_transpose_result[M3(2,0)] =  (m1[M4(0,1)]*m1[M4(1,2)]-m1[M4(0,2)]*m1[M4(1,1)])*invdet;
    inv_transpose_result[M3(0,1)] = -(m1[M4(1,0)]*m1[M4(2,2)]-m1[M4(1,2)]*m1[M4(2,0)])*invdet;
    inv_transpose_result[M3(1,1)] =  (m1[M4(0,0)]*m1[M4(2,2)]-m1[M4(0,2)]*m1[M4(2,0)])*invdet;
    inv_transpose_result[M3(2,1)] = -(m1[M4(0,0)]*m1[M4(1,2)]-m1[M4(1,0)]*m1[M4(0,2)])*invdet;
    inv_transpose_result[M3(0,2)] =  (m1[M4(1,0)]*m1[M4(2,1)]-m1[M4(2,0)]*m1[M4(1,1)])*invdet;
    inv_transpose_result[M3(1,2)] = -(m1[M4(0,0)]*m1[M4(2,1)]-m1[M4(2,0)]*m1[M4(0,1)])*invdet;
    inv_transpose_result[M3(2,2)] =  (m1[M4(0,0)]*m1[M4(1,1)]-m1[M4(1,0)]*m1[M4(0,1)])*invdet;

    return inv_transpose_result;
}
