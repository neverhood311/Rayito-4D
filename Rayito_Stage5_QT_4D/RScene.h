////////////////////////////////////////////////////////////////////////////////
//
// Very simple ray tracing example
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RSCENE_H__
#define __RSCENE_H__

#include <list>
#include <algorithm>

#include "RMath.h"
#include "RMaterial.h"
#include "RRay.h"
#include "RSampling.h"


namespace Rayito
{


//
// Shapes (scene hierarchy)
//

class Shape
{
public:
    virtual ~Shape() { }
    
    // Subclasses must implement this; this is the meat of ray tracing.
    // The first version finds the nearest intersection, the second just tells
    // us if the ray hits anything at all (generally used for shadow rays).
    virtual bool intersect(Intersection& intersection) = 0;
    virtual bool doesIntersect(const Ray& ray) = 0;
    
    // Usually for lights: given two random numbers between 0.0 and 1.0, find a
    // location + surface normal on the surface, and return the PDF for how
    // likely the sample was (with respect to solid angle).  Return false if not
    // a surface.
    virtual bool sampleSurface(const Point& refPosition,
                               const Vector& refNormal,
                               float u1,
                               float u2,
                               Point& outPosition,
                               Vector& outNormal,
                               float& outPdf)
    {
        outPdf = 0.0f;
        return false;
    }
    
    // Given a reference position and normal, and a position and normal on the
    // surface of this shape, return the PDF (with respect to solid angle) of
    // having chosen that surface position.
    virtual float pdfSA(const Point& refPosition,
                        const Vector& refNormal,
                        const Point& surfPosition,
                        const Vector& surfNormal) const
    {
        Vector incoming = surfPosition - refPosition;
        float dist = incoming.normalize();
        return dist * dist * surfaceAreaPdf() / std::fabs(dot(surfNormal, incoming));
    }
    
    // Return the likelihood of sampling a point on a surface
    virtual float surfaceAreaPdf() const
    {
        return 0.0f;
    }
    
    // Find all lights in the scene starting with this shape
    virtual void findLights(std::list<Shape*>& outLightList) { }
    
    // Is this shape itself actually a light?
    virtual bool isLight() const { return false; }
};


// List of shapes, so you can aggregate a pile of them
class ShapeSet : public Shape
{
public:
    virtual ~ShapeSet() { }
    
    virtual bool intersect(Intersection& intersection)
    {
        bool intersectedAny = false;
        for (std::list<Shape*>::iterator iter = m_shapes.begin();
             iter != m_shapes.end();
             ++iter)
        {
            Shape *pShape = *iter;
            bool intersected = pShape->intersect(intersection);
            if (intersected)
            {
                intersectedAny = true;
            }
        }
        return intersectedAny;
    }
    
    virtual bool doesIntersect(const Ray& ray)
    {
        for (std::list<Shape*>::iterator iter = m_shapes.begin();
             iter != m_shapes.end();
             ++iter)
        {
            Shape *pShape = *iter;
            bool intersected = pShape->doesIntersect(ray);
            if (intersected)
            {
                return true;
            }
        }
        return false;
    }
    
    virtual void findLights(std::list<Shape*>& outLightList)
    {
        for (std::list<Shape*>::iterator iter = m_shapes.begin();
             iter != m_shapes.end();
             ++iter)
        {
            Shape *pShape = *iter;
            pShape->findLights(outLightList);
        }
    }
    
    void addShape(Shape *pShape) { m_shapes.push_back(pShape); }
    
    void clearShapes() { m_shapes.clear(); }
    
protected:
    std::list<Shape*> m_shapes;
};


// Infinite-extent plane, with option bullseye texturing to make it interesting.
class Plane : public Shape
{
public:
    Plane(const Point& position, const Vector& normal, Material *pMaterial, bool bullseye = false)
        : m_position(position),
          m_normal(normal.normalized()),
          m_pMaterial(pMaterial),
          m_bullseye(bullseye)
    {
        
    }
    
    virtual ~Plane() { }
    
    virtual bool intersect(Intersection& intersection)
    {
        // Plane eqn: ax+by+cz+d=0; another way of writing it is: dot(n, p-p0)=0
        // where n=normal=(a,b,c), and p=(x,y,z), and p0 is position.  Now, p is
        // the ray equation (the intersection point is along the ray): p=origin+t*direction
        // So the plane-ray intersection eqn is dot(n, origin+t*direction-p0)=0.
        // Distribute, and you get:
        //     dot(n, origin) + t*dot(n, direction) - dot(n, p0) = 0
        // Solve for t, and you get:
        //    t = (dot(n, p0) - dot(n, origin)) / dot(n, direction)
        
        // Check if it's even possible to intersect
        float nDotD = dot(m_normal, intersection.m_ray.m_direction);
        if (nDotD >= 0.0f)
        {
            return false;
        }
        
        float t = (dot(m_position, m_normal) - dot(intersection.m_ray.m_origin, m_normal)) / nDotD;
        
        // Make sure t is not behind the ray, and is closer than the current
        // closest intersection.
        if (t >= intersection.m_t || t < kRayTMin)
        {
            return false;
        }
        
        // This intersection is closer, so record it.
        intersection.m_t = t;
        intersection.m_pShape = this;
        intersection.m_pMaterial = m_pMaterial;
        intersection.m_normal = m_normal;
        intersection.m_colorModifier = Color(1.0f, 1.0f, 1.0f);
        
        // Hack bullseye pattern to get some variation
        if (m_bullseye && std::fmod((intersection.position() - m_position).length() * 0.25f, 1.0f) > 0.5f)
        {
            intersection.m_colorModifier *= 0.2f;
        }
        
        return true;
    }

    virtual bool doesIntersect(const Ray& ray)
    {
        // Plane eqn: ax+by+cz+d=0; another way of writing it is: dot(n, p-p0)=0
        // where n=normal=(a,b,c), and p=(x,y,z), and p0 is position.  Now, p is
        // the ray equation (the intersection point is along the ray): p=origin+t*direction
        // So the plane-ray intersection eqn is dot(n, origin+t*direction-p0)=0.
        // Distribute, and you get:
        //     dot(n, origin) + t*dot(n, direction) - dot(n, p0) = 0
        // Solve for t, and you get:
        //    t = (dot(n, p0) - dot(n, origin)) / dot(n, direction)
        
        // Check if it's even possible to intersect
        float nDotD = dot(m_normal, ray.m_direction);
        if (nDotD >= 0.0f)
        {
            return false;
        }
        
        float t = (dot(m_position, m_normal) - dot(ray.m_origin, m_normal)) / nDotD;
        
        // Make sure t is not behind the ray, and is closer than the current
        // closest intersection.
        if (t >= ray.m_tMax || t < kRayTMin)
        {
            return false;
        }
        
        return true;
    }

protected:
    Point m_position;
    Vector m_normal;
    Material *m_pMaterial;
    bool m_bullseye;
};


// Sphere (heh, what else?)
class Sphere : public Shape
{
public:
    Sphere(const Point& position, float radius, Material* pMaterial)
        : m_position(position),
          m_radius(radius),
          m_pMaterial(pMaterial)
    {
        
    }
    
    virtual ~Sphere() { }
    
    virtual bool intersect(Intersection& intersection)
    {
        // Transform ray to local space.  In this case it's just moving the
        // sphere center to the origin (and the ray along with it).   This makes
        // the intersection logic easier to follow.
        Ray localRay = intersection.m_ray;
        localRay.m_origin -= m_position;
        
        // Ray-sphere intersection can result in either zero, one or two points
        // of intersection.  It turns into a quadratic equation, so we just find
        // the solution using the quadratic formula.  Note that there is a
        // slightly more stable form of it when computing it on a computer, and
        // we use that method to keep everything accurate.
        
        // Calculate quadratic coeffs
        float a = localRay.m_direction.length2();
        float b = 2.0f * dot(localRay.m_direction, localRay.m_origin);
        float c = localRay.m_origin.length2() - m_radius * m_radius;
        
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f)
        {
            // Discriminant less than zero?  No solution => no intersection.
            return false;
        }
        discriminant = std::sqrt(discriminant);
        
        // Compute a more stable form of our param t (t0 = q/a, t1 = c/q)
        // q = -0.5 * (b - sqrt(b * b - 4.0 * a * c)) if b < 0, or
        // q = -0.5 * (b + sqrt(b * b - 4.0 * a * c)) if b >= 0
        float q = (b < 0.0f) ? (-0.5f * (b - discriminant)) : (-0.5f * (b + discriminant));
        
        // Get our final parametric values
        float t0 = q / a;
        float t1 = (q != 0.0f) ? (c / q) : intersection.m_t;
        
        // Swap them so they are ordered closest-to-farthest
        if (t0 > t1)
        {
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        
        // Check our intersection for validity against this ray's extents
        if (t0 >= kRayTMin && t0 < intersection.m_t)
        {
            intersection.m_t = t0;
        }
        else if (t1 >= kRayTMin && t1 < intersection.m_t)
        {
            intersection.m_t = t1;
        }
        else
        {
            // Both intersections are outside of the ray's extent
            return false;
        }
        
        // Create our intersection data
        Point localPos = localRay.calculate(intersection.m_t);
        Vector worldNorm = localPos.normalized();
        
        intersection.m_pShape = this;
        intersection.m_pMaterial = m_pMaterial;
        intersection.m_normal = worldNorm;
        intersection.m_colorModifier = Color(1.0f, 1.0f, 1.0f);
        
        return true;
    }
    
    virtual bool doesIntersect(const Ray& ray)
    {
        // Transform ray to local space.  In this case it's just moving the
        // sphere center to the origin (and the ray along with it).   This makes
        // the intersection logic easier to follow.
        Ray localRay = ray;
        localRay.m_origin -= m_position;
        
        // Ray-sphere intersection can result in either zero, one or two points
        // of intersection.  It turns into a quadratic equation, so we just find
        // the solution using the quadratic formula.  Note that there is a
        // slightly more stable form of it when computing it on a computer, and
        // we use that method to keep everything accurate.
        
        // Calculate quadratic coeffs
        float a = localRay.m_direction.length2();
        float b = 2.0f * dot(localRay.m_direction, localRay.m_origin);
        float c = localRay.m_origin.length2() - m_radius * m_radius;
        
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f)
        {
            // Discriminant less than zero?  No solution => no intersection.
            return false;
        }
        discriminant = std::sqrt(discriminant);
        
        // Compute a more stable form of our param t (t0 = q/a, t1 = c/q)
        // q = -0.5 * (b - sqrt(b * b - 4.0 * a * c)) if b < 0, or
        // q = -0.5 * (b + sqrt(b * b - 4.0 * a * c)) if b >= 0
        float q = (b < 0.0f) ? (-0.5f * (b - discriminant)) : (-0.5f * (b + discriminant));
        
        // Get our final parametric values, see if any intersect
        float t0 = q / a;
        if (t0 >= kRayTMin && t0 < ray.m_tMax)
        {
            return true;
        }
        float t1 = c / q;
        if (q != 0.0f && t1 < ray.m_tMax && t1 >= kRayTMin)
        {
            return true;
        }
        return false;
    }
    
    // Given two random numbers between 0.0 and 1.0, find a location + surface
    // normal on the surface of the *light*.
    virtual bool sampleSurface(const Point& refPosition,
                               const Vector& refNormal,
                               float u1,
                               float u2,
                               Point& outPosition,
                               Vector& outNormal,
                               float& outPdf)
    {
        Vector toCenter = m_position - refPosition;
        float dist2 = toCenter.length2();
        if (dist2 < m_radius * m_radius * 1.00001f)
        {
            // Inside or on the surface of the sphere?  Just pick a spot to sample.
            outNormal = uniformToSphere(u1, u2);
            outPosition = m_position + outNormal * m_radius;
            Vector toSurf = refPosition - outPosition;
            outPdf = toSurf.length2() * surfaceAreaPdf() / std::fabs(dot(toSurf.normalized(), outNormal));
            return true;
        }
        // Outside the surface of the sphere; fit a cone around the sphere to
        // sample more efficiently within the cone
        float sinThetaMax2 = m_radius * m_radius / dist2;
        float cosThetaMax = std::sqrt(std::max(0.0f, 1.0f - sinThetaMax2));
        Vector x, y, z, w;
        makeCoordinateSpace(toCenter, x, y, z, w);
        Vector localCone = uniformToCone(u1, u2, cosThetaMax);
        Vector cone = transformFromLocalSpace(localCone, x, y, z, w).normalized();
        // Make sure the generated direction actually hits the sphere; if not, adjust
        Ray ray(refPosition, cone);
        Intersection isect(ray);
        if (!intersect(isect))
        {
            isect.m_t = dot(toCenter, cone);
        }
        outPosition = ray.calculate(isect.m_t);
        outNormal = (outPosition - m_position).normalized();
        outPdf = uniformConePdf(cosThetaMax);
        return true;
    }
    
    virtual float pdfSA(const Point &refPosition,
                        const Vector &refNormal,
                        const Point &surfPosition,
                        const Vector &surfNormal) const
    {
        Vector toCenter = m_position - refPosition;
        float dist2 = toCenter.length2();
        if (dist2 < m_radius * m_radius * 1.00001f)
        {
            // Inside or on the surface of the sphere?  Just pick a spot to sample.
            Vector toSurf = refPosition - surfPosition;
            return toSurf.length2() * surfaceAreaPdf() / std::fabs(dot(toSurf.normalized(), surfNormal));
        }
        float sinThetaMax2 = m_radius * m_radius / dist2;
        float cosThetaMax = std::sqrt(std::max(0.0f, 1.0f - sinThetaMax2));
        return uniformConePdf(cosThetaMax);
    }
    
    virtual float surfaceAreaPdf() const
    {
        return 3.0f / (4.0f * M_PI * m_radius * m_radius);
    }

protected:
    Point m_position;
    float m_radius;
    Material *m_pMaterial;
};

class Tesseract : public Shape
{
public:
    Tesseract(const Point& position, float sideLength, Material *pMaterial)
        : m_position(position),
          m_sideLength(sideLength),
          m_pMaterial(pMaterial)
    {
        float half_side = sideLength / 2.0f;
        extents[0] = Vector(-half_side, -half_side, -half_side, -half_side);
        extents[1] = Vector(half_side, half_side, half_side, half_side);
        m_transform.translate(position);
    }

    Tesseract(){}

    virtual ~Tesseract(){}

    virtual bool intersect(Intersection& intersection){
        //implement the ray-tesseract intersection test
        Ray localRay = intersection.m_ray;
        //transform the ray into object space
        localRay.transform(m_transform.m_matrix);
        //some local copies for easier code
        Point rayOrig = localRay.m_origin;
        //Vector rayDir = localRay.m_direction;
        Vector invDir = localRay.m_invDir;

        float tmin, tmax, tymin, tymax, tzmin, tzmax, twmin, twmax;
        //perform the intersection test
        tmin = (extents[localRay.m_sign[0]].m_x - rayOrig.m_x) * invDir.m_x;
        tmax = (extents[1 - localRay.m_sign[0]].m_x - rayOrig.m_x) * invDir.m_x;
        tymin = (extents[localRay.m_sign[1]].m_y - rayOrig.m_y) * invDir.m_y;
        tymax = (extents[1 - localRay.m_sign[1]].m_y - rayOrig.m_y) * invDir.m_y;
        if(tmin > tymax || tymin > tmax)return false;
        if(tymin > tmin)
            tmin = tymin;
        if(tymax < tmax)
            tmax = tymax;
        tzmin = (extents[localRay.m_sign[2]].m_z - rayOrig.m_z) * invDir.m_z;
        tzmax = (extents[1 - localRay.m_sign[2]].m_z - rayOrig.m_z) * invDir.m_z;
        if(tmin > tzmax || tzmin > tmax)return false;
        if(tzmin > tmin)
            tmin = tzmin;
        if(tzmax < tmax)
            tmax = tzmax;
        twmin = (extents[localRay.m_sign[3]].m_w - rayOrig.m_w) * invDir.m_w;
        twmax = (extents[1 - localRay.m_sign[3]].m_w - rayOrig.m_w) * invDir.m_w;
        if ((tmin > twmax) || (twmin > tmax))return false;
        if (twmin > tmin){
            tmin = twmin;
        }
        if (twmax < tmax){
            tmax = twmax;
        }
        if(tmax < 0)return false;   //the tesseract is behind the ray
        if(tmax > kRayTMax || tmin < kRayTMin)return false;   //the intersection is too far away or too close

        float _dist = (tmin >= 0)? tmin : tmax;
        if(_dist < intersection.m_t)        //if this is the closest object so far
            intersection.m_t = _dist;
        else
            return false;
        //printf("we've been hit, captain!");
        //Create our intersection data
        Point localPos = localRay.calculate(intersection.m_t);

        Vector worldNorm = Vector(0, 0, 0, 0);
        int maxIdx = localPos.absMaxIdx();
        if(localPos[maxIdx] >= 0)
            worldNorm.set(maxIdx, 1);
        else
            worldNorm.set(maxIdx, -1);
        //transform the normal back into world space
        worldNorm.mmult(m_transform.m_invMatrix, false);

        intersection.m_pShape = this;
        intersection.m_pMaterial = m_pMaterial;
        intersection.m_normal = worldNorm;
        intersection.m_colorModifier = Color(1.0f, 1.0f, 1.0f);
        return true;
    }

    virtual bool doesIntersect(const Ray &ray){
        //implement, once again, the ray-tesseract intersection test
        Ray localRay = ray;
        //transform the ray into object space
        localRay.transform(m_transform.m_matrix);
        //some local copies for easier code
        Point rayOrig = localRay.m_origin;
        //Vector rayDir = localRay.m_direction;
        Vector invDir = localRay.m_invDir;

        float tmin, tmax, tymin, tymax, tzmin, tzmax, twmin, twmax;
        //perform the intersection test
        tmin = (extents[localRay.m_sign[0]].m_x - rayOrig.m_x) * invDir.m_x;
        tmax = (extents[1 - localRay.m_sign[0]].m_x - rayOrig.m_x) * invDir.m_x;
        tymin = (extents[localRay.m_sign[1]].m_y - rayOrig.m_y) * invDir.m_y;
        tymax = (extents[1 - localRay.m_sign[1]].m_y - rayOrig.m_y) * invDir.m_y;
        if(tmin > tymax || tymin > tmax)return false;
        if(tymin > tmin)
            tmin = tymin;
        if(tymax < tmax)
            tmax = tymax;
        tzmin = (extents[localRay.m_sign[2]].m_z - rayOrig.m_z) * invDir.m_z;
        tzmax = (extents[1 - localRay.m_sign[2]].m_z - rayOrig.m_z) * invDir.m_z;
        if(tmin > tzmax || tzmin > tmax)return false;
        if(tzmin > tmin)
            tmin = tzmin;
        if(tzmax < tmax)
            tmax = tzmax;
        twmin = (extents[localRay.m_sign[3]].m_w - rayOrig.m_w) * invDir.m_w;
        twmax = (extents[1 - localRay.m_sign[3]].m_w - rayOrig.m_w) * invDir.m_w;
        if ((tmin > twmax) || (twmin > tmax))return false;
        if (twmin > tmin){
            tmin = twmin;
        }
        if (twmax < tmax){
            tmax = twmax;
        }
        if(tmax < 0)return false;   //the tesseract is behind the ray
        if(tmax > kRayTMax || tmin < kRayTMin)return false;   //the intersection is too far away or too close

        //float _dist = (tmin >= 0)? tmin : tmax;
        return true;
    }

    virtual float pdfSA(const Point &refPosition, const Vector &refNormal, const Point &surfPosition, const Vector &surfNormal) const{
        return 0.5f;
    }

    virtual float surfaceAreaPdf() const{
        return m_sideLength * m_sideLength * 24.0f; //TODO: the surface area of a tesseract
    }

    virtual bool infiniteExtent() const{ return false; }

    Transform4D m_transform;
protected:
    Point m_position;
    float m_sideLength;
    Material *m_pMaterial;
    Vector extents[2];      //the min and max bounds of the tesseract in OBJECT space
};

} // namespace Rayito


#endif // __RSCENE_H__
