
#include "solid_color.h"
#include <stdlib.h>
#include <texture/texture.h>

Vec3 solid_color_get(rt_float u, rt_float v, const Vec3 *point, const SolidColor *self)
{
    (void)u;
    (void)v;
    (void)point;
    return self->color;
}

Texture solid_color_create(Vec3 color)
{
    SolidColor *col = malloc(sizeof(SolidColor));

    col->color = color;

    return (Texture){
        .data = col,
        .type = TEXTURE_SOLID_COLOR};
}
