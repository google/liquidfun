#ifndef GLMATRIX_H_
#define GLMATRIX_H_

#ifndef GL_MODELVIEW
#define GL_MODELVIEW		0x1700
#endif
#ifndef GL_PROJECTION
#define GL_PROJECTION		0x1701
#endif
#ifndef GL_TEXTURE
#define GL_TEXTURE			0x1702
#endif

void gl_matrix_mode(int mmode);
void gl_push_matrix(void);
void gl_pop_matrix(void);
void gl_load_identity(void);
void gl_load_matrixf(const float *mat);
void gl_mult_matrixf(const float *mat);
void gl_translatef(float x, float y, float z);
void gl_rotatef(float angle, float x, float y, float z);
void gl_scalef(float x, float y, float z);
void gl_ortho(float left, float right, float bottom, float top, float near, float far);
void gl_frustum(float left, float right, float bottom, float top, float near, float far);
void glu_perspective(float vfov, float aspect, float near, float far);

/* getters */
float* get_matrix(int mm);
float* get_inv_transpose_3x3(int mm);

#endif	/* GLMATRIX_H_ */
