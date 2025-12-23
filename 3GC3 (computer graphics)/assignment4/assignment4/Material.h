#ifndef MATERIAL_H
#define MATERIAL_H

class HitResult;

class ReflectResult
{
public:
    Ray m_ray;
    Vector3D m_color;
};

class Material
{
public:
    Vector3D m_color;
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) = 0;
};


class Diffuse : public Material
{
public:
    Diffuse(const Vector3D& color)
    {
        m_color = color;
    };
    
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override
    {
        ReflectResult res;
        //TODO: 4. generate one scattered ray and assign it to res.m_ray
        // Create a scattered ray
        Ray reflectRay;
        reflectRay.m_origin = hit.m_hitPos;
        
        // The scattered ray shoots toward any random direction.
        Vector3D dir = normalize(Vector3D::random(-1,1));
        reflectRay.m_direction = dir;
        // If the random direction is not on the hemishpere, flip the direction vector
        if (dot(hit.m_hitNormal, dir) < 0) {
            reflectRay.m_direction = -dir;
        }
        // Assign it to res.m_ray
        res.m_ray = reflectRay;
        res.m_color = m_color;
        return res;
    }
};


class Specular : public Material
{
public:
    Specular(const Vector3D& color)
    {
        m_color = color;
    }
    
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override
    {
        ReflectResult res;
        //TODO 5
        // Generate one mirrored ray
        Ray resRay;
        Vector3D l = -(ray.direction());
        Vector3D n = hit.m_hitNormal;
        resRay.m_direction = 2*dot(l, n)*n - l;
        resRay.m_origin = hit.m_hitPos;
        // Assign it to res.m_ray
        res.m_ray = resRay;
        res.m_color = m_color;
        return res;
    }
};
#endif
