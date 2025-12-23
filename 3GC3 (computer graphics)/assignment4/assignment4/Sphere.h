#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

using namespace std;
class Material;

class HitResult {
public:
    HitResult() { m_isHit = false; };
    bool m_isHit;
    Vector3D m_hitPos;
    Vector3D m_hitNormal;
    shared_ptr<Material> m_hitMaterial;
    float m_t;
};


class Sphere {
    
public:
    Sphere() {}
    Sphere(Vector3D center, float r, shared_ptr<Material> m)
    {
        m_center = center;
        m_radius = r;
        m_pMaterial = m;
    }
    HitResult hit(Ray& r, float min_t, float max_t);

    public:
    Vector3D m_center;
    float m_radius;
    shared_ptr<Material> m_pMaterial;
};

//test if ray hits this sphere within range min_t and max_t
HitResult Sphere::hit(Ray& ray, float min_t, float max_t)
{
    HitResult hit_result;
    // TODO 2
    // Compute the discriminant (value in the square root)
    Vector3D originMinusCentre = ray.origin() - m_center;
    auto half_B = dot(ray.direction(), originMinusCentre);
    auto A = ray.direction().length_squared();
    auto C = originMinusCentre.length_squared() - m_radius*m_radius;
    auto discriminant = half_B*half_B - A*C;
    if (discriminant < 0) {
        hit_result.m_isHit = false;
    } else {
        // Use the formula to compute two versions of t
        auto t_minus = (-half_B - sqrt(discriminant))/A;
        auto t_plus = (-half_B + sqrt(discriminant))/A;
        // if t_minus is in range of min_t and max_t
        if (t_minus >= min_t && t_minus <= max_t) {
            hit_result.m_isHit = true;
            hit_result.m_t = t_minus;
            hit_result.m_hitPos = ray.at(t_minus);
            hit_result.m_hitNormal = (hit_result.m_hitPos - m_center)/m_radius;
            hit_result.m_hitMaterial = m_pMaterial;
        } // if t_plus is in range of min_t and max_t
        else if (t_plus >= min_t && t_plus <= max_t) {
            hit_result.m_isHit = true;
            hit_result.m_t = t_plus;
            hit_result.m_hitPos = ray.at(t_plus);
            hit_result.m_hitNormal = (hit_result.m_hitPos - m_center)/m_radius;
            hit_result.m_hitMaterial = m_pMaterial;
        }
        else {
            hit_result.m_isHit = false;
        }
    }

    return hit_result;
}

#endif
