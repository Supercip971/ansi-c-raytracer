#include "matrix4x4.h"
#include <stdio.h>
#include <string.h>

void create_matrix_identity(Matrix4x4 *matrix)
{
    Matrix4x4 val = {{{1, 0, 0, 0},
                      {0, 1, 0, 0},
                      {0, 0, 1, 0},
                      {0, 0, 0, 1}}};

    *matrix = val;
}

void create_matrix_scale(Matrix4x4 *matrix, rt_float x, rt_float y, rt_float z)
{
    Matrix4x4 val = {{{x, 0, 0, 0},
                      {0, y, 0, 0},
                      {0, 0, z, 0},
                      {0, 0, 0, 1}}};

    *matrix = val;
}

void create_matrix_translate(Matrix4x4 *matrix, rt_float x, rt_float y, rt_float z)
{
    Matrix4x4 val = {{{1, 0, 0, x},
                      {0, 1, 0, y},
                      {0, 0, 1, z},
                      {0, 0, 0, 1}}};

    *matrix = val;
}

void create_matrix_rotate_x(Matrix4x4 *matrix, rt_float angle)
{
    Matrix4x4 val = {{{1, 0, 0, 0},
                      {0, cos(angle), -sin(angle), 0},
                      {0, sin(angle), cos(angle), 0},
                      {0, 0, 0, 1}}};

    *matrix = val;
}

void create_matrix_rotate_y(Matrix4x4 *matrix, rt_float angle)
{
    Matrix4x4 val = {{{cos(angle), 0, sin(angle), 0},
                      {0, 1, 0, 0},
                      {-sin(angle), 0, cos(angle), 0},
                      {0, 0, 0, 1}}};

    *matrix = val;
}

void create_matrix_rotate_z(Matrix4x4 *matrix, rt_float angle)
{
    // rotate a matrix around the z axis
    Matrix4x4 val = {{{cos(angle), -sin(angle), 0, 0},
                      {sin(angle), cos(angle), 0, 0},
                      {0, 0, 1, 0},
                      {0, 0, 0, 1}}};

    *matrix = val;
}

static void matrix_getcofactor(const Matrix4x4 *matrix, Matrix4x4 *result, int r, int c, int size)
{
    int rx = 0;
    int ry = 0;

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            if (x != r && y != c)
            {
                result->value[ry][rx++] = matrix->value[y][x];
                if (rx == size - 1)
                {
                    rx = 0;
                    ry++;
                }
            }
        }
    }
}

static rt_float matrix_get_determinant(const Matrix4x4 *matrix, int size)
{
    rt_float det = 0;
    Matrix4x4 cofactor = {};

    rt_float sign = 1;

    if (size == 1)
    {
        return matrix->value[0][0];
    }

    for (int i = 0; i < size; i++)
    {
        matrix_getcofactor(matrix, &cofactor, 0, i, size);
        det += sign * matrix->value[i][0] * matrix_get_determinant(&cofactor, size - 1);

        sign = -sign;
    }

    return det;
}

static void matrix_get_adjoint(const Matrix4x4 *matrix, Matrix4x4 *result)
{
    Matrix4x4 temp_matrix = {};

    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            matrix_getcofactor(matrix, &temp_matrix, x, y, 4);

            rt_float sign = ((x + y) % 2 == 0) ? 1.0 : -1.0;

            result->value[y][x] = sign * matrix_get_determinant(&temp_matrix, 4 - 1);
        }
    }
}

void matrix_inverse(const Matrix4x4 *matrix, Matrix4x4 *result)
{
    // calculate the inverse of the 4x4 matrix
    // using Cramer's Rule
    rt_float det = matrix_get_determinant(matrix, 4);

    if (det <= 0.0f)
    {
        create_matrix_identity(result);
        // printf("matrix_inverse: determinant is 0.0f\n");
        return;
    }

    Matrix4x4 adjoint;
    matrix_get_adjoint(matrix, &adjoint);

    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            result->value[y][x] = adjoint.value[y][x] / det;
        }
    }

    Matrix4x4 save = *result;

    matrix_transpose(&save, result);
}

void matrix_transpose(const Matrix4x4 *matrix, Matrix4x4 *result)
{
    // transpose the matrix
    result->value[0][0] = matrix->value[0][0];
    result->value[0][1] = matrix->value[1][0];
    result->value[0][2] = matrix->value[2][0];
    result->value[0][3] = matrix->value[3][0];
    result->value[1][0] = matrix->value[0][1];
    result->value[1][1] = matrix->value[1][1];
    result->value[1][2] = matrix->value[2][1];
    result->value[1][3] = matrix->value[3][1];
    result->value[2][0] = matrix->value[0][2];
    result->value[2][1] = matrix->value[1][2];
    result->value[2][2] = matrix->value[2][2];
    result->value[2][3] = matrix->value[3][2];
    result->value[3][0] = matrix->value[0][3];
    result->value[3][1] = matrix->value[1][3];
    result->value[3][2] = matrix->value[2][3];
    result->value[3][3] = matrix->value[3][3];
}

void matrix_multiply(const Matrix4x4 *a, const Matrix4x4 *b, Matrix4x4 *result)
{
    Matrix4x4 acopy = *a;
    Matrix4x4 bcopy = *b;

    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            result->value[y][x] = acopy.value[y][0] * bcopy.value[0][x] +
                                  acopy.value[y][1] * bcopy.value[1][x] +
                                  acopy.value[y][2] * bcopy.value[2][x] +
                                  acopy.value[y][3] * bcopy.value[3][x];
        }
    }
}

void matrix_apply_vector(const Matrix4x4 *matrix, Vec3 *vector)
{
    Vec3 temp = {
        .x = vector->x * matrix->value[0][0] + vector->y * matrix->value[0][1] + vector->z * matrix->value[0][2],
        .y = vector->x * matrix->value[1][0] + vector->y * matrix->value[1][1] + vector->z * matrix->value[1][2],
        .z = vector->x * matrix->value[2][0] + vector->y * matrix->value[2][1] + vector->z * matrix->value[2][2],
    };
    *vector = temp;
}

// point is just apply vector with the matrix offset applied
void matrix_apply_point(const Matrix4x4 *matrix, Vec3 *point)
{
    Vec3 temp = {
        .x = point->x * matrix->value[0][0] + point->y * matrix->value[0][1] + point->z * matrix->value[0][2] + matrix->value[0][3],
        .y = point->x * matrix->value[1][0] + point->y * matrix->value[1][1] + point->z * matrix->value[1][2] + matrix->value[1][3],
        .z = point->x * matrix->value[2][0] + point->y * matrix->value[2][1] + point->z * matrix->value[2][2] + matrix->value[2][3],
    };

    *point = temp;
}

void matrix_apply_ray(const Matrix4x4 *matrix, Ray *ray)
{
    matrix_apply_point(matrix, &ray->origin);
    matrix_apply_vector(matrix, &ray->direction);
    ray_dir_init(ray);
}
