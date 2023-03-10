#include "simd.hpp"

#define _USE_MATH_DEFINES
#include <assert.h>
#include <math.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif

#ifdef __clang__
#define unroll_loop_3 _Pragma("clang loop unroll_count(3)")
#define unroll_loop_4 _Pragma("clang loop unroll_count(4)")
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define unroll_loop_3 _Pragma("GCC unroll 3")
#define unroll_loop_4 _Pragma("GCC unroll 4")
#endif

float3 f4tof3(float4 f) { return (float3){f[0], f[1], f[2]}; }
float4 f3tof4(float3 f, float w) { return (float4){f[0], f[1], f[2], w}; }
float2 f3tof2(float3 f) { return (float2){f[0], f[1]}; }

float3x4 m44tom34(float4x4 m) {
  return (float3x4){
      .row0 = m.row0,
      .row1 = m.row1,
      .row2 = m.row2,
  };
}
float3x3 m44tom33(float4x4 m) {
  return (float3x3){
      .row0 = f4tof3(m.row0),
      .row1 = f4tof3(m.row1),
      .row2 = f4tof3(m.row2),
  };
}

float4x4 m33tom44(float3x3 m) {
  return (float4x4){
      .row0 = f3tof4(m.row0, 0.0f),
      .row1 = f3tof4(m.row1, 0.0f),
      .row2 = f3tof4(m.row2, 0.0f),
      .row3 = {0.0f, 0.0f, 0.0f, 1.0f},
  };
}

float dotf2(float2 x, float2 y) { return (x[0] * y[0]) + (x[1] * y[1]); }

float dotf3(float3 x, float3 y) {
  return (x[0] * y[0]) + (x[1] * y[1]) + (x[2] * y[2]);
}

float dotf4(float4 x, float4 y) {
  return (x[0] * y[0]) + (x[1] * y[1]) + (x[2] * y[2]) + (x[3] * y[3]);
}

float3 crossf3(float3 x, float3 y) {
  return (float3){
      (x[1] * y[2]) - (x[2] * y[1]),
      (x[2] * y[0]) - (x[0] * y[2]),
      (x[0] * y[1]) - (x[1] * y[0]),
  };
}

float magf2(float2 v) { return sqrtf((v[0] * v[0]) + (v[1] * v[1])); }

float magf3(float3 v) {
  return sqrtf((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
}

float magf4(float4 v) {
  return sqrtf((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]) + (v[3] * v[3]));
}

float magsqf3(float3 v) {
  return (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
}

float magsqf4(float4 v) {
  return (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]) + (v[3] * v[3]);
}

float2 normf2(float2 v) {
  float inv_sum = 1 / magf2(v);
  return (float2){
      v[0] * inv_sum,
      v[1] * inv_sum,
  };
}

float3 normf3(float3 v) {
  float inv_sum = 1 / magf3(v);
  return (float3){
      v[0] * inv_sum,
      v[1] * inv_sum,
      v[2] * inv_sum,
  };
}

float4 normf4(float3 v) {
  float inv_sum = 1 / magf4(v);
  return (float3){
      v[0] * inv_sum,
      v[1] * inv_sum,
      v[2] * inv_sum,
      v[3] * inv_sum,
  };
}

Quaternion normq(Quaternion q) { return (Quaternion)normf4((float4)q); }

float lenf3(float3 v) { return sqrtf(dotf3(v, v)); }

float3x3 mf33_identity(void) {
  return (float3x3){
      (float3){1, 0, 0},
      (float3){0, 1, 0},
      (float3){0, 0, 1},
  };
}

float3x4 mf34_identity(void) {
  return (float3x4){
      (float4){1, 0, 0, 0},
      (float4){0, 1, 0, 0},
      (float4){0, 0, 1, 0},
  };
}

float4x4 mf44_identity(void) {
  return (float4x4){
      (float4){1, 0, 0, 0},
      (float4){0, 1, 0, 0},
      (float4){0, 0, 1, 0},
      (float4){0, 0, 0, 1},
  };
}

float4 mulf44(float4x4 m, float4 v) {
  float4 out = {0};

  unroll_loop_4 for (uint32_t i = 0; i < 4; ++i) {
    float sum = 0.0f;
    unroll_loop_4 for (uint32_t ii = 0; ii < 4; ++ii) {
      sum += m.rows[ii][i] * v[ii];
    }
    out[i] = sum;
  }

  return out;
}

float4 mul4f44f(float4 v, float4x4 m) {
  float4 out = {0};

  unroll_loop_4 for (uint32_t i = 0; i < 4; ++i) {
    float sum = 0.0f;
    unroll_loop_4 for (uint32_t ii = 0; ii < 4; ++ii) {
      sum += m.rows[i][ii] * v[ii];
    }
    out[i] = sum;
  }

  return out;
}

float4x4 mulmf44(float4x4 x, float4x4 y) {
  float4x4 o = {.row0 = {0}};
  unroll_loop_4 for (uint32_t i = 0; i < 4; ++i) {
    unroll_loop_4 for (uint32_t ii = 0; ii < 4; ++ii) {
      float s = 0.0f;
      unroll_loop_4 for (uint32_t iii = 0; iii < 4; ++iii) {
        s += x.rows[i][iii] * y.rows[iii][ii];
      }
      o.rows[i][ii] = s;
    }
  }
  return o;
}

float4x4 inv_mf44(float4x4 m) {
  float coef00 = m.row2[2] * m.row3[3] - m.row3[2] * m.row2[3];
  float coef02 = m.row1[2] * m.row3[3] - m.row3[2] * m.row1[3];
  float coef03 = m.row1[2] * m.row2[3] - m.row2[2] * m.row1[3];
  float coef04 = m.row2[1] * m.row3[3] - m.row3[1] * m.row2[3];
  float coef06 = m.row1[1] * m.row3[3] - m.row3[1] * m.row1[3];
  float coef07 = m.row1[1] * m.row2[3] - m.row2[1] * m.row1[3];
  float coef08 = m.row2[1] * m.row3[2] - m.row3[1] * m.row2[2];
  float coef10 = m.row1[1] * m.row3[2] - m.row3[1] * m.row1[2];
  float coef11 = m.row1[1] * m.row2[2] - m.row2[1] * m.row1[2];
  float coef12 = m.row2[0] * m.row3[3] - m.row3[0] * m.row2[3];
  float coef14 = m.row1[0] * m.row3[3] - m.row3[0] * m.row1[3];
  float coef15 = m.row1[0] * m.row2[3] - m.row2[0] * m.row1[3];
  float coef16 = m.row2[0] * m.row3[2] - m.row3[0] * m.row2[2];
  float coef18 = m.row1[0] * m.row3[2] - m.row3[0] * m.row1[2];
  float coef19 = m.row1[0] * m.row2[2] - m.row2[0] * m.row1[2];
  float coef20 = m.row2[0] * m.row3[1] - m.row3[0] * m.row2[1];
  float coef22 = m.row1[0] * m.row3[1] - m.row3[0] * m.row1[1];
  float coef23 = m.row1[0] * m.row2[1] - m.row2[0] * m.row1[1];

  float4 fac0 = {coef00, coef00, coef02, coef03};
  float4 fac1 = {coef04, coef04, coef06, coef07};
  float4 fac2 = {coef08, coef08, coef10, coef11};
  float4 fac3 = {coef12, coef12, coef14, coef15};
  float4 fac4 = {coef16, coef16, coef18, coef19};
  float4 fac5 = {coef20, coef20, coef22, coef23};

  float4 vec0 = {m.row1[0], m.row0[0], m.row0[0], m.row0[0]};
  float4 vec1 = {m.row1[1], m.row0[1], m.row0[1], m.row0[1]};
  float4 vec2 = {m.row1[2], m.row0[2], m.row0[2], m.row0[2]};
  float4 vec3 = {m.row1[3], m.row0[3], m.row0[3], m.row0[3]};

  float4 inv0 = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
  float4 inv1 = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
  float4 inv2 = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
  float4 inv3 = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

  float4 sign_a = {+1, -1, +1, -1};
  float4 sign_b = {-1, +1, -1, +1};
  float4x4 inv = {inv0 * sign_a, inv1 * sign_b, inv2 * sign_a, inv3 * sign_b};

  float4 Row0 = {inv.row0[0], inv.row1[0], inv.row2[0], inv.row3[0]};

  float4 dot0 = m.row0 * Row0;
  float dot1 = (dot0[0] + dot0[1]) + (dot0[2] + dot0[3]);

  float OneOverDeterminant = 1.0f / dot1;

  float4x4 out = {
      inv.row0 * OneOverDeterminant,
      inv.row1 * OneOverDeterminant,
      inv.row2 * OneOverDeterminant,
      inv.row3 * OneOverDeterminant,
  };
  return out;
}

float3x3 mf33_from_axes(float3 forward, float3 right, float3 up) {
  return (float3x3){
      .row0 = {forward[0], up[0], right[0]},
      .row1 = {forward[1], up[1], right[1]},
      .row2 = {forward[2], up[2], right[2]},
  };
}

Quaternion mf33_to_quat(float3x3 mat) {
  float four_x_squared_minus_1 =
      mat.rows[0][0] - mat.rows[1][1] - mat.rows[2][2];
  float four_y_squared_minus_1 =
      mat.rows[1][1] - mat.rows[0][0] - mat.rows[2][2];
  float four_z_squared_minus_1 =
      mat.rows[2][2] - mat.rows[0][0] - mat.rows[1][1];
  float four_w_squared_minus_1 =
      mat.rows[0][0] + mat.rows[1][1] + mat.rows[2][2];

  int32_t biggest_index = 0;
  float four_biggest_squared_minus_1 = four_w_squared_minus_1;
  if (four_x_squared_minus_1 > four_biggest_squared_minus_1) {
    four_biggest_squared_minus_1 = four_x_squared_minus_1;
    biggest_index = 1;
  }
  if (four_y_squared_minus_1 > four_biggest_squared_minus_1) {
    four_biggest_squared_minus_1 = four_y_squared_minus_1;
    biggest_index = 2;
  }
  if (four_z_squared_minus_1 > four_biggest_squared_minus_1) {
    four_biggest_squared_minus_1 = four_z_squared_minus_1;
    biggest_index = 3;
  }

  float biggest_val = sqrtf(four_biggest_squared_minus_1 + 1.0f) * 0.5f;
  float mult = 0.25f / biggest_val;

  switch (biggest_index) {
  case 0:
    return (Quaternion){
        (mat.rows[1][2] - mat.rows[2][1]) * mult,
        (mat.rows[2][0] - mat.rows[0][2]) * mult,
        (mat.rows[0][1] - mat.rows[1][0]) * mult,
        biggest_val,
    };
  case 1:
    return (Quaternion){
        biggest_val,
        (mat.rows[0][1] + mat.rows[1][0]) * mult,
        (mat.rows[2][0] + mat.rows[0][2]) * mult,
        (mat.rows[1][2] - mat.rows[2][1]) * mult,
    };
  case 2:
    return (Quaternion){
        (mat.rows[0][1] + mat.rows[1][0]) * mult,
        biggest_val,
        (mat.rows[1][2] + mat.rows[2][1]) * mult,
        (mat.rows[2][0] - mat.rows[0][2]) * mult,
    };
  case 3:
    return (Quaternion){
        (mat.rows[2][0] + mat.rows[0][2]) * mult,
        (mat.rows[1][2] + mat.rows[2][1]) * mult,
        biggest_val,
        (mat.rows[0][1] - mat.rows[1][0]) * mult,
    };
  default:
    assert(false);
    return (Quaternion){0, 0, 0, 1};
  }
}

Quaternion quat_from_axes(float3 forward, float3 right, float3 up) {
  return mf33_to_quat(mf33_from_axes(forward, right, up));
}

Quaternion angle_axis_to_quat(float4 angle_axis) {
  float s = sinf(angle_axis[3] * 0.5f);
  return (Quaternion){
      angle_axis[0] * s,
      angle_axis[1] * s,
      angle_axis[2] * s,
      cosf(angle_axis[3] * 0.5f),
  };
}

float3x3 quat_to_mf33(Quaternion q) {
  float qxx = q[0] * q[0];
  float qyy = q[1] * q[1];
  float qzz = q[2] * q[2];
  float qxz = q[0] * q[2];
  float qxy = q[0] * q[1];
  float qyz = q[1] * q[2];
  float qwx = q[3] * q[0];
  float qwy = q[3] * q[1];
  float qwz = q[3] * q[2];

  float m00 = 1.0f - 2.0f * (qyy + qzz);
  float m01 = 2.0f * (qxy + qwz);
  float m02 = 2.0f * (qxz - qwy);

  float m10 = 2.0f * (qxy - qwz);
  float m11 = 1.0f - 2.0f * (qxx + qzz);
  float m12 = 2.0f * (qyz + qwx);

  float m20 = 2.0f * (qxz + qwy);
  float m21 = 2.0f * (qyz - qwx);
  float m22 = 1.0f - 2.0f * (qxx + qyy);

  return (float3x3){
      .row0 = {m00, m01, m02},
      .row1 = {m10, m11, m12},
      .row2 = {m20, m21, m22},
  };
}

float4x4 quat_to_trans(Quaternion q) { return m33tom44(quat_to_mf33(q)); }

Quaternion trans_to_quat(float4x4 mat) { return mf33_to_quat(m44tom33(mat)); }

Quaternion mulq(Quaternion p, Quaternion q) {
  return (Quaternion){
      (p[3] * q[0]) + (p[0] * q[3]) + (p[1] * q[2]) - (p[2] * q[1]),
      (p[3] * q[1]) + (p[1] * q[3]) + (p[2] * q[0]) - (p[0] * q[2]),
      (p[3] * q[2]) + (p[2] * q[3]) + (p[0] * q[1]) - (p[1] * q[0]),
      (p[3] * q[3]) - (p[0] * q[0]) - (p[1] * q[1]) - (p[2] * q[2]),
  };
}

// https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
float3 qrotf3(Quaternion q, float3 v) {
  // Extract the vector and scalar parts of the quat
  float3 u = {q[0], q[1], q[2]};
  float s = q[3];

  return (2.0f * dotf3(u, v) * u) + (((s * s) - dotf3(u, v)) * v) +
         (2.0f * s * crossf3(u, v));
}

AABB aabb_init(void) {
  return (AABB){
      .min = {FLT_MAX, FLT_MAX, FLT_MAX},
      .max = {-FLT_MAX, -FLT_MAX, -FLT_MAX},
  };
}

void aabb_add_point(AABB *aabb, float3 point) {
  aabb->min[0] = SDL_min(aabb->min[0], point[0]);
  aabb->min[1] = SDL_min(aabb->min[1], point[1]);
  aabb->min[2] = SDL_min(aabb->min[2], point[2]);
  aabb->max[0] = SDL_max(aabb->max[0], point[0]);
  aabb->max[1] = SDL_max(aabb->max[1], point[1]);
  aabb->max[2] = SDL_max(aabb->max[2], point[2]);
}

void translate(Transform *t, float3 p) {
  assert(t);
  t->position += p;
}
void scale(Transform *t, float3 s) {
  assert(t);
  t->scale *= s;
}
void rotate(Transform *t, Quaternion r) {
  assert(t);
  t->rotation = mulq(r, t->rotation);
}

void transform_to_matrix(float4x4 *m, const Transform *t) {
  // Position matrix
  float4x4 p = {
      (float4){1, 0, 0, t->position[0]},
      (float4){0, 1, 0, t->position[1]},
      (float4){0, 0, 1, t->position[2]},
      (float4){0, 0, 0, 1},
  };

  // Rotation matrix from quaternion
  float4x4 r = quat_to_trans(t->rotation);

  // Scale matrix
  float4x4 s = {
      (float4){t->scale[0], 0, 0, 0},
      (float4){0, t->scale[1], 0, 0},
      (float4){0, 0, t->scale[2], 0},
      (float4){0, 0, 0, 1},
  };

  // Transformation matrix = p * r * s
  *m = mulmf44(mulmf44(p, r), s);
}

void look_forward(float4x4 *m, float3 pos, float3 forward, float3 up) {
  forward = normf3(forward);
  float3 right = normf3(crossf3(normf3(up), forward));
  up = normf3(crossf3(forward, right));

  *m = (float4x4){
      (float4){right[0], right[1], right[2], -dotf3(right, pos)},
      (float4){up[0], up[1], up[2], -dotf3(up, pos)},
      (float4){forward[0], forward[1], forward[2], -dotf3(forward, pos)},
      (float4){0, 0, 0, 1},
  };
}

// Left-Handed
void look_at(float4x4 *m, float3 pos, float3 target, float3 up) {
  assert(m);

  float3 forward = normf3(target - pos);
  look_forward(m, pos, forward, up);
}

Quaternion look_forward_quat(float3 forward, float3 up) {
  float4x4 m = {.row0 = {0}};
  look_forward(&m, (float3){0.0f, 0.0f, 0.0f}, forward, up);
  return trans_to_quat(m);
}

Quaternion look_at_quat(float3 pos, float3 target, float3 up) {
  float3 forward = normf3(target - pos);
  return look_forward_quat(forward, up);
}

// Left Handed
void perspective(float4x4 *m, float fovy, float aspect, float zn, float zf) {
  assert(m);
  float focal_length = 1.0f / tanf(fovy * 0.5f);

  float m00 = focal_length / aspect;
  float m11 = focal_length;
  float m22 = zf / (zf - zn);
  float m23 = -(zn * zf) / (zf - zn);

  *m = (float4x4){
      (float4){m00, 0, 0, 0},
      (float4){0, m11, 0, 0},
      (float4){0, 0, m22, m23},
      (float4){0, 0, 1, 0},
  };
}

// Left Handed
void reverse_perspective(float4x4 *m, float fovy, float aspect, float zn,
                         float zf) {
  assert(m);
  float focal_length = 1.0f / tanf(fovy * 0.5f);

  float m00 = focal_length / aspect;
  float m11 = focal_length;
  float m22 = zn / (zf - zn);
  float m23 = zf * zn / (zf - zn);

  *m = (float4x4){
      (float4){m00, 0, 0, 0},
      (float4){0, m11, 0, 0},
      (float4){0, 0, m22, m23},
      (float4){0, 0, -1, 0},
  };
}

// Left Handed
float4x4 orthographic(float r, float l, float t, float b, float zn, float zf) {
  return (float4x4){
      (float4){2.0f / (r - l), 0, 0, 0},
      (float4){0, 2.0f / (t - b), 0, 0},
      (float4){0, 0, 1 / (zf - zn), 0},
      (float4){(l + r) / (l - r), (t + b) / (b - t), zn / (zn - zf), 1},
  };
}

// See //
// https://www.braynzarsoft.net/viewtutorial/q16390-34-aabb-cpu-side-frustum-culling
Frustum frustum_from_view_proj(const float4x4 *vp) {
  Frustum f = {
      .planes[LeftPlane] =
          (float4){
              vp->row3[0] + vp->row0[0],
              vp->row3[1] + vp->row0[1],
              vp->row3[2] + vp->row0[2],
              vp->row3[3] + vp->row0[3],
          },
      .planes[RightPlane] =
          (float4){
              vp->row3[0] - vp->row0[0],
              vp->row3[1] - vp->row0[1],
              vp->row3[2] - vp->row0[2],
              vp->row3[3] - vp->row0[3],
          },
      .planes[TopPlane] =
          (float4){
              vp->row3[0] - vp->row1[0],
              vp->row3[1] - vp->row1[1],
              vp->row3[2] - vp->row1[2],
              vp->row3[3] - vp->row1[3],
          },
      .planes[BottomPlane] =
          (float4){
              vp->row3[0] + vp->row1[0],
              vp->row3[1] + vp->row1[1],
              vp->row3[2] + vp->row1[2],
              vp->row3[3] + vp->row1[3],
          },
      .planes[NearPlane] =
          (float4){
              vp->row2[0],
              vp->row2[1],
              vp->row2[2],
              vp->row2[3],
          },
      .planes[FarPlane] =
          (float4){
              vp->row3[0] - vp->row2[0],
              vp->row3[1] - vp->row2[1],
              vp->row3[2] - vp->row2[2],
              vp->row3[3] - vp->row2[3],
          },
  };
  // Must normalize planes
  for (uint32_t i = 0; i < FrustumPlaneCount; ++i) {
    Plane *p = &f.planes[i];
    const float norm_length =
        lenf3((float3){p->xyzw[0], p->xyzw[1], p->xyzw[2]});
    p->xyzw[0] /= norm_length;
    p->xyzw[1] /= norm_length;
    p->xyzw[2] /= norm_length;
    p->xyzw[3] /= norm_length;
  }
  return f;
}

bool frustum_test_aabb(const Frustum *frust, const AABB *aabb) {
  // See
  // https://www.braynzarsoft.net/viewtutorial/q16390-34-aabb-cpu-side-frustum-culling
  for (uint32_t i = 0; i < FrustumPlaneCount; ++i) {
    const Plane *plane = &frust->planes[i];
    const float3 normal =
        (float3){plane->xyzw[0], plane->xyzw[1], plane->xyzw[2]};
    const float plane_const = plane->xyzw[3];

    float3 axis = {0};
    axis[0] = normal[0] < 0.0f ? aabb->min[0] : aabb->max[0];
    axis[1] = normal[1] < 0.0f ? aabb->min[1] : aabb->max[1];
    axis[2] = normal[2] < 0.0f ? aabb->min[2] : aabb->max[2];

    if (dotf3(normal, axis) + plane_const < 0.0f) {
      // The AABB was outside one of the planes and failed this test
      return false;
    }
  }
  // The AABB was inside each plane
  return true;
}

float deg_to_rad(float d) { return d * (M_PI / 180.0f); }
float rad_to_deg(float r) { return r * (180 / M_PI); }

// https://en.wikipedia.org/wiki/Linear_interpolation
float lerpf(float v0, float v1, float a) { return (1 - a) * v0 + a * v1; }
float3 lerpf3(float3 v0, float3 v1, float a) {
  return ((1 - a) * v0) + (a * v1);
}

// https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
Quaternion slerp(Quaternion q1, Quaternion q2, float a) {
  // Calc angle between quaternions
  float cos_half_theta =
      q1[3] * q2[3] + q1[0] * q2[0] + q1[1] * q2[1] + q1[2] * q2[2];
  // If q1=q2 or q1=-q2 then alpha = 0 and we can return q1
  if (fabsf(cos_half_theta) >= 1.0f) {
    return q1;
  }

  float half_theta = acosf(cos_half_theta);
  float sin_half_theta = sqrtf(1.0f - cos_half_theta * cos_half_theta);
  // if alpha = 180 degrees then result is not fully defined
  // we could rotate around any axis normal to qa or qb
  if (fabsf(sin_half_theta) < 0.001f) { // fabs is floating point absolute
    return (Quaternion){
        q1[0] * 0.5f + q2[0] * 0.5f,
        q1[1] * 0.5f + q2[1] * 0.5f,
        q1[2] * 0.5f + q2[2] * 0.5f,
        q1[3] * 0.5f + q2[3] * 0.5f,
    };
  }

  float ratio_a = sinf((1.0f - a) * half_theta) / sin_half_theta;
  float ratio_b = sinf(a * half_theta) / sin_half_theta;

  return (Quaternion){
      q1[0] * ratio_a + q2[0] * ratio_b,
      q1[1] * ratio_a + q2[1] * ratio_b,
      q1[2] * ratio_a + q2[2] * ratio_b,
      q1[3] * ratio_a + q2[3] * ratio_b,
  };
}

float clampf(float v, float min, float max) {
  if (v < min) {
    return min;
  }
  if (v > max) {
    return max;
  }
  return v;
}

float3 clampf3(float3 v, float3 min, float3 max) {
  return (float3){
      clampf(v[0], min[0], max[0]),
      clampf(v[1], min[1], max[1]),
      clampf(v[2], min[2], max[2]),
  };
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
