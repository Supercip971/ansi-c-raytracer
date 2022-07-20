#include "light.h"
#include <material/material.h>
#include <shape/hittable.h>
#include <stdlib.h>
#include <texture/solid_color.h>

bool light_emit(const HitRecord *record, Vec3 *emit, const Light *self)
{

    if (record->front_face == self->flipped)
    {
        *emit = vec3_create(0, 0, 0);
        return false;
    }

    *emit = texture_get(record->u, record->v, &record->pos, &self->emition);

    return true;
}

Material light_create_texture(Texture emition)
{
    Light *light = malloc(sizeof(Light));

    light->emition = emition;

    return (Material){
        .type = MATERIAL_LIGHT,
        .data = light,
    };
}

Material light_create(Vec3 emition)
{
    return light_create_texture(solid_color_create(emition));
}
