#include <math.h>

#include "camera_utils3d.h"

void reset_camera(Camera3D* camera) {
    camera->position = (Vector3){500.0f, 350.0f, 700.0f};
    camera->target = (Vector3){0.0f, 0.0f, 0.0f};
    camera->up = (Vector3){0.0f, 1.0f, 0.0f};
    camera->fovy = 45.0f;
    camera->projection = CAMERA_PERSPECTIVE;
}

float dot_product_3d(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int select_body_3d(Vector2 mouse,
                   Camera3D camera,
                   body_t* tab,
                   int nbe,
                   ScaleType scaledist,
                   ScaleType scalerad,
                   float radius_scale,
                   float dist_scale,
                   float AU) {
    Ray ray = GetMouseRay(mouse, camera);

    int selected = -1;
    float bestT = 1000000000.0f;

    for (int i = 0; i < nbe; i++) {
        Vector3 pos;
        float radius;

        virtual3d(&radius, &pos,
                  scaledist, scalerad,
                  radius_scale, dist_scale,
                  tab[i].radius, tab[i].position, AU);

        if (radius < 5.0f) {
            radius = 5.0f;
        }

        Vector3 oc;
        oc.x = ray.position.x - pos.x;
        oc.y = ray.position.y - pos.y;
        oc.z = ray.position.z - pos.z;

        float b = 2.0f * dot_product_3d(oc, ray.direction);
        float c = dot_product_3d(oc, oc) - radius * radius;

        float discriminant = b * b - 4.0f * c;

        if (discriminant >= 0.0f) {
            float t = (-b - sqrtf(discriminant)) / 2.0f;

            if (t < 0.0f) {
                t = (-b + sqrtf(discriminant)) / 2.0f;
            }

            if (t > 0.0f && t < bestT) {
                bestT = t;
                selected = i;
            }
        }
    }

    return selected;
}

void follow_body_3d(Camera3D* camera,
                    body_t body,
                    ScaleType scaledist,
                    ScaleType scalerad,
                    float radius_scale,
                    float dist_scale,
                    float AU) {
    Vector3 pos;
    float radius;

    virtual3d(&radius, &pos,
              scaledist, scalerad,
              radius_scale, dist_scale,
              body.radius, body.position, AU);

    Vector3 offset;
    offset.x = camera->position.x - camera->target.x;
    offset.y = camera->position.y - camera->target.y;
    offset.z = camera->position.z - camera->target.z;

    camera->target = pos;

    camera->position.x = camera->target.x + offset.x;
    camera->position.y = camera->target.y + offset.y;
    camera->position.z = camera->target.z + offset.z;
}