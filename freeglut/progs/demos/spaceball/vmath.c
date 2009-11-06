#include <math.h>
#include "vmath.h"

quat_t quat_rotate(quat_t q, float angle, float x, float y, float z)
{
	quat_t rq;
	float half_angle = angle * 0.5;
	float sin_half = sin(half_angle);

	rq.w = cos(half_angle);
	rq.x = x * sin_half;
	rq.y = y * sin_half;
	rq.z = z * sin_half;

	return quat_mul(q, rq);
}
