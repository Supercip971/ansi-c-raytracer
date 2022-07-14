#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <hitable.h>
#include <math/matrix4x4.h>

typedef struct
{
    Matrix4x4 matrix;
    Matrix4x4 inv_matrix;
    Object target;
    AABB transformed_aabb;
} Transform;

rt_float transform_pdf_value(Vec3 origin, Vec3 direction, const Transform *self);
Vec3 transform_pdf_random(Vec3 origin, const Transform *self);

Object transform(Object translated, Matrix4x4 matrix);
bool hit_transform_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const Transform *self);
bool transform_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const Transform *self);
bool transform_destroy(Transform *self);

#endif /* C95E0A6A_3BD4_4ED1_89B9_E4D0699C094C */
