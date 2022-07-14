#include "constant_medium.h"
#include <stdlib.h>
#include "../material/isotropic.h"
#include "../texture/solid_color.h"
#include "utils.h"

Object make_constant_medium(Object obj, rt_float d, Vec3 albedo)
{
    return make_constant_medium_mat(obj, d, isotropic_create(solid_color_create(albedo)));
}

Object make_constant_medium_mat(Object obj, rt_float d, Material mat)
{
    ConstantMedium *data = malloc(sizeof(ConstantMedium));
    *data = (ConstantMedium){
        .d = -1 / d,
        .phase_material = mat,
        .target = obj,
    };

    return (Object){
        .type = SHAPE_CONSTANT_MEDIUM,
        .is_leaf = true,
        .data = data,
    };
}

bool constant_get_aabb(rt_float time_start, rt_float time_end, AABB *output, const ConstantMedium *self)
{
    return object_get_aabb(time_start, time_end, output, (Object *)&self->target);
}

bool constant_destroy(ConstantMedium *self)
{
    return object_destroy(&self->target);
}

bool hit_constant_object_callback(Ray ray, rt_float t_min, rt_float t_max, HitRecord *record, const ConstantMedium *self)
{
    HitRecord rec_0 = {};
    HitRecord rec_1 = {};
    const rt_float ray_length = vec3_length(ray.direction);

    if (!object_collide(ray, -INFINITY, +INFINITY, &rec_0, (Object *)&self->target))
    {
        return false;
    }

    if (!object_collide(ray, rec_0.t + 0.0001, +INFINITY, &rec_1, (Object *)&self->target))
    {
        return false;
    }

    rec_0.t = rt_max(rec_0.t, t_min);
    rec_1.t = rt_min(rec_1.t, t_max);

    if (rec_0.t >= rec_1.t)
    {
        return false;
    }

    rec_1.t = rt_max(rec_1.t, 0);

    rt_float distance_inside_box = (rec_1.t - rec_0.t) * ray_length;
    rt_float hit_distance = self->d * fast_log(random_rt_float());

    if (hit_distance > distance_inside_box)
    {
        return false;
    }

    record->t = rec_0.t + hit_distance / ray_length;
    record->pos = ray_point_param(ray, record->t);
    record->normal = vec3_create(1, 0, 0);
    record->material = self->phase_material;
    record->front_face = true;

    return true;
}
