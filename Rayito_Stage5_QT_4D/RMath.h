////////////////////////////////////////////////////////////////////////////////
//
// Very simple ray tracing example
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RMATH_H__
#define __RMATH_H__

#include <cmath>
#include <algorithm>


#ifndef M_PI
    // For some reason, MSVC doesn't define this when <cmath> is included
    #define M_PI 3.14159265358979
#endif


namespace Rayito
{


//
// RGB color class
//
// Operators supported:
//     color = color
//     color + color, color += color
//     color - color, color -= color
//     color * color, color *= color, float * color, color * float, color *= float
//     color / color, color /= color, color / float
//

struct Color
{
    float m_r, m_g, m_b;

    Color()                          : m_r(0.0f), m_g(0.0f), m_b(0.0f)    { }
    Color(const Color& c)            : m_r(c.m_r), m_g(c.m_g), m_b(c.m_b) { }
    Color(float r, float g, float b) : m_r(r), m_g(g), m_b(b)             { }
    explicit Color(float f)          : m_r(f), m_g(f), m_b(f)             { }


    void clamp(float min = 0.0f, float max = 1.0f)
    {
        m_r = std::max(min, std::min(max, m_r));
        m_g = std::max(min, std::min(max, m_g));
        m_b = std::max(min, std::min(max, m_b));
    }


    Color& operator =(const Color& c)
    {
        m_r = c.m_r;
        m_g = c.m_g;
        m_b = c.m_b;
        return *this;
    }

    Color& operator +=(const Color& c)
    {
        m_r += c.m_r;
        m_g += c.m_g;
        m_b += c.m_b;
        return *this;
    }

    Color& operator -=(const Color& c)
    {
        m_r -= c.m_r;
        m_g -= c.m_g;
        m_b -= c.m_b;
        return *this;
    }

    Color& operator *=(const Color& c)
    {
        m_r *= c.m_r;
        m_g *= c.m_g;
        m_b *= c.m_b;
        return *this;
    }

    Color& operator /=(const Color& c)
    {
        m_r /= c.m_r;
        m_g /= c.m_g;
        m_b /= c.m_b;
        return *this;
    }

    Color& operator *=(float f)
    {
        m_r *= f;
        m_g *= f;
        m_b *= f;
        return *this;
    }

    Color& operator /=(float f)
    {
        m_r /= f;
        m_g /= f;
        m_b /= f;
        return *this;
    }
};


inline Color operator +(const Color& c1, const Color& c2)
{
    return Color(c1.m_r + c2.m_r,
                 c1.m_g + c2.m_g,
                 c1.m_b + c2.m_b);
}


inline Color operator -(const Color& c1, const Color& c2)
{
    return Color(c1.m_r - c2.m_r,
                 c1.m_g - c2.m_g,
                 c1.m_b - c2.m_b);
}


inline Color operator *(const Color& c1, const Color& c2)
{
    return Color(c1.m_r * c2.m_r,
                 c1.m_g * c2.m_g,
                 c1.m_b * c2.m_b);
}


inline Color operator /(const Color& c1, const Color& c2)
{
    return Color(c1.m_r / c2.m_r,
                 c1.m_g / c2.m_g,
                 c1.m_b / c2.m_b);
}


inline Color operator *(const Color& c, float f)
{
    return Color(f * c.m_r,
                 f * c.m_g,
                 f * c.m_b);
}


inline Color operator *(float f, const Color& c)
{
    return Color(f * c.m_r,
                 f * c.m_g,
                 f * c.m_b);
}


inline Color operator /(const Color& c, float f)
{
    return Color(c.m_r / f,
                 c.m_g / f,
                 c.m_b / f);
}

//  Mat5 class and associated functions

struct Mat5{
    float m_mat[5][5];    //[column][row]
    Mat5(){
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                m_mat[i][j] = 0;
                if(i == j)
                    m_mat[i][j] = 1;  //identity matrix
            }
        }
    }

    Mat5(float mat[5][5]){
        for(int c = 0; c < 5; ++c){
            for(int r = 0; r < 5; ++r){
                m_mat[c][r] = mat[c][r];
            }
        }
    }
    Mat5(const Mat5& mat){
        for(int c = 0; c < 5; ++c){
            for(int r = 0; r < 5; ++r){
                m_mat[c][r] = mat.m_mat[c][r];
            }
        }
    }

    Mat5& operator =(const Mat5& m){
        for(int c = 0; c < 5; ++c){
            for(int r = 0; r < 5; ++r){
                m_mat[c][r] = m.m_mat[c][r];
            }
        }
        return *this;
    }

    Mat5& operator *=(const Mat5& m){
        float result[5][5];
        for(int c = 0; c < 5; ++c){
            for(int r = 0; r < 5; ++r){
                result[c][r] = 0;
                for(int i = 0; i < 5; ++i){
                    result[c][r] += m_mat[i][r] * m.m_mat[c][i];
                }
            }
        }
        for(int c = 0; c < 5; ++c){
            for(int r = 0; r < 5; ++r){
                m_mat[c][r] = result[c][r];
            }
        }
        return *this;
    }

    void printMat(){
        for(int r = 0; r < 5; ++r){
            printf("|%f, %f, %f, %f, %f|\n", m_mat[0][r], m_mat[1][r], m_mat[2][r], m_mat[3][r], m_mat[4][r]);
        }
        printf("\n");
    }
};


//
// 4D vector class (and associated operations)
//
// Operators supported:
//     vector = vector
//     vector + vector, vector += vector
//     vector - vector, vector -= vector
//     vector * float, vector *= float, float * vector
//     vector / float, vector /= float
//     vector[]: 0->x, 1->y, 2->z, 3->w
//

struct Vector
{
    float m_x, m_y, m_z, m_w;

    Vector()                          : m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f)      { }
    Vector(const Vector& v)           : m_x(v.m_x), m_y(v.m_y), m_z(v.m_z), m_w(v.m_w)  { }
    Vector(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w)         { }
    explicit Vector(float f)          : m_x(f), m_y(f), m_z(f), m_w(f)                  { }


    float length2() const { return m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w; }
    float length()  const { return std::sqrt(length2()); }

    // Returns old length from before normalization (ignore the return value if you don't need it)
    float  normalize()        { float len = length(); if (len > 0) *this /= len; return len; }
    // Return a vector in this same direction, but normalized
    Vector normalized() const { Vector r(*this); r.normalize(); return r; }

    float maxComponent() const { return std::max(std::max(std::max(m_x, m_y), m_z), m_w); }
    float minComponent() const { return std::min(std::min(std::min(m_x, m_y), m_z), m_w); }


    Vector& operator =(const Vector& v)
    {
        m_x = v.m_x;
        m_y = v.m_y;
        m_z = v.m_z;
        m_w = v.m_w;
        return *this;
    }

    Vector& operator +=(const Vector& v)
    {
        m_x += v.m_x;
        m_y += v.m_y;
        m_z += v.m_z;
        m_w += v.m_w;
        return *this;
    }

    Vector& operator -=(const Vector& v)
    {
        m_x -= v.m_x;
        m_y -= v.m_y;
        m_z -= v.m_z;
        m_w -= v.m_w;
        return *this;
    }

    Vector& operator *=(const Vector& v)
    {
        m_x *= v.m_x;
        m_y *= v.m_y;
        m_z *= v.m_z;
        m_w *= v.m_w;
        return *this;
    }

    Vector& operator *=(float f)
    {
        m_x *= f;
        m_y *= f;
        m_z *= f;
        m_w *= f;
        return *this;
    }

    Vector& operator /=(const Vector& v)
    {
        m_x /= v.m_x;
        m_y /= v.m_y;
        m_z /= v.m_z;
        m_w /= v.m_w;
        return *this;
    }

    Vector& operator /=(float f)
    {
        m_x /= f;
        m_y /= f;
        m_z /= f;
        m_w /= f;
        return *this;
    }

    Vector operator -() const
    {
        return Vector(-m_x, -m_y, -m_z, -m_w);
    }

    float operator [](const int i) const{
        switch(i){
        case 0:
            return m_x;
        case 1:
            return m_y;
        case 2:
            return m_z;
        case 3:
            return m_w;
        default:
            return 0;
        }
    }

    void set(int i, int val){
        switch(i){
        case 0:
            m_x = val;
            break;
        case 1:
            m_y = val;
            break;
        case 2:
            m_z = val;
            break;
        case 3:
            m_w = val;
            break;
        }
    }

    inline int maxIdx(){    //return the index of the largest element in the Vector
        int result = 0;
        float maxVal = m_x;
        if(m_y > maxVal){
            result = 1;
            maxVal = m_y;
        }
        if(m_z > maxVal){
            result = 2;
            maxVal = m_z;
        }
        if(m_w > maxVal){
            result = 3;
        }
        return result;
    }

    inline int absMaxIdx(){ //return the index of the element with the largest absolute value
        int result = 0;
        float maxVal = abs(m_x);
        if(abs(m_y) > maxVal){
            result = 1;
            maxVal = abs(m_y);
        }
        if(abs(m_z) > maxVal){
            result = 2;
            maxVal = abs(m_z);
        }
        if(abs(m_w) > maxVal){
            result = 3;
        }
        return result;
    }

    void printVec(){
        printf("(%f, %f, %f, %f)\n", m_x, m_y, m_z, m_w);
    }

    void mmult(Mat5& m, bool isPoint){ //multiply a column-major matrix by a Vector
        float res[5];
        float v[5] = {m_x, m_y, m_z, m_w, 0};
        if(isPoint)v[4] = 1;

        for(int r = 0; r < 5; ++r){
            res[r] = 0;
            for(int c = 0; c < 5; ++c){
                res[r] += m.m_mat[c][r] * v[c];
            }
        }

        m_x = res[0];
        m_y = res[1];
        m_z = res[2];
        m_w = res[3];
    }
};

inline Vector operator +(const Vector& v1, const Vector& v2)
{
    return Vector(v1.m_x + v2.m_x,
                  v1.m_y + v2.m_y,
                  v1.m_z + v2.m_z,
                  v1.m_w + v2.m_w);
}


inline Vector operator -(const Vector& v1, const Vector& v2)
{
    return Vector(v1.m_x - v2.m_x,
                  v1.m_y - v2.m_y,
                  v1.m_z - v2.m_z,
                  v1.m_w - v2.m_w);
}


inline Vector operator *(const Vector& v1, const Vector& v2)
{
    return Vector(v1.m_x * v2.m_x,
                  v1.m_y * v2.m_y,
                  v1.m_z * v2.m_z,
                  v1.m_w * v2.m_w);
}


inline Vector operator *(const Vector& v, float f)
{
    return Vector(f * v.m_x,
                  f * v.m_y,
                  f * v.m_z,
                  f * v.m_w);
}


inline Vector operator *(float f, const Vector& v)
{
    return Vector(f * v.m_x,
                  f * v.m_y,
                  f * v.m_z,
                  f * v.m_w);
}

inline Vector operator /(const Vector& v1, const Vector& v2)
{
    return Vector(v1.m_x / v2.m_x,
                  v1.m_y / v2.m_y,
                  v1.m_z / v2.m_z,
                  v1.m_w / v2.m_w);
}


inline Vector operator /(float f, const Vector& v)
{
    return Vector(f / v.m_x,
                  f / v.m_y,
                  f / v.m_z,
                  f / v.m_w);
}


// dot(v1, v2) = length(v1) * length(v2) * cos(angle between v1, v2)
inline float dot(const Vector& v1, const Vector& v2)
{
    // In cartesian coordinates, it simplifies to this simple calculation:
    return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z + v1.m_w * v2.m_w;
}


// cross(v1, v2) = length(v1) * length(v2) * sin(angle between v1, v2);
// result is perpendicular to both v1, v2.
inline Vector cross(const Vector& v1, const Vector& v2)
{
    // In cartesian coordinates, it simplifies down to this calculation:
    return Vector(v1.m_y * v2.m_z - v1.m_z * v2.m_y,
                  v1.m_z * v2.m_x - v1.m_x * v2.m_z,
                  v1.m_x * v2.m_y - v1.m_y * v2.m_x,
                  0.0f);    //TODO: Should I get rid of this 3D cross product?
}

//result is perpendicular to U, V, and W
inline Vector cross4(const Vector& U, const Vector& V, const Vector& W){
    double A, B, C, D, E, F;
    A = (V.m_x * W.m_y) - (V.m_y * W.m_x);
    B = (V.m_x * W.m_z) - (V.m_z * W.m_x);
    C = (V.m_x * W.m_w) - (V.m_w * W.m_x);
    D = (V.m_y * W.m_z) - (V.m_z * W.m_y);
    E = (V.m_y * W.m_w) - (V.m_w * W.m_y);
    F = (V.m_z * W.m_w) - (V.m_w * W.m_z);

    Vector result;
    result.m_x =   (U.m_y * F) - (U.m_z * E) + (U.m_w * D);
    result.m_y = - (U.m_x * F) + (U.m_z * C) - (U.m_w * B);
    result.m_z =   (U.m_x * E) - (U.m_y * C) + (U.m_w * A);
    result.m_w = - (U.m_x * D) + (U.m_y * B) - (U.m_z * A);

    return result;
}


inline Vector max(const Vector& v1, const Vector& v2)
{
    return Vector(std::max(v1.m_x, v2.m_x),
                  std::max(v1.m_y, v2.m_y),
                  std::max(v1.m_z, v2.m_z),
                  std::max(v1.m_w, v2.m_w));
}

inline Vector min(const Vector& v1, const Vector& v2)
{
    return Vector(std::min(v1.m_x, v2.m_x),
                  std::min(v1.m_y, v2.m_y),
                  std::min(v1.m_z, v2.m_z),
                  std::min(v1.m_w, v2.m_w));
}


// Oh, by the way, a point can be thought of as just a vector, but where you
// refrain from doing dot/cross/normalize operations on it.
typedef Vector Point;


//
// Transformations
//

// Given a single direction, generate a coordinate space where the direction
// becomes the Z axis, and the X and Y axes are consistently made to match
inline void makeCoordinateSpace(const Vector& normalRef,
                                Vector& outXAxis, Vector& outYAxis, Vector& outZAxis, Vector& outWAxis)
{
    outZAxis = normalRef.normalized();
    Vector v2 = (outZAxis.m_x != 0.0f || outZAxis.m_z != 0.0f) ?
                    Vector(0.0f, 1.0f, 0.0f, 0.0f) :
                    Vector(1.0f, 0.0f, 0.0f, 0.0f);
    outXAxis = cross(v2, outZAxis).normalized();
    outYAxis = cross(outZAxis, outXAxis);
    //TODO: modify this function for 4D
}

// Given a two directions, generate a coordinate space where the normal becomes
// the Z axis, and the X and Y axes are consistently made to match and align the
// X axis as much as possible with the tangent direction
inline void makeCoordinateSpace(const Vector& normalRef, const Vector& tangent,
                                Vector& outXAxis, Vector& outYAxis, Vector& outZAxis, Vector& outWAxis)
{
    outZAxis = normalRef.normalized();
    outYAxis = cross(tangent, outZAxis).normalized();
    outXAxis = cross(outZAxis, outYAxis);
    //TODO: modify this function for 4D
}

// Transform a vector into the local space defined by X,Y,Z,W orthonormal axes
inline Vector transformToLocalSpace(const Vector& v,
                                    const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& wAxis)
{
    return Vector(dot(v, xAxis), dot(v, yAxis), dot(v, zAxis), dot(v, wAxis));
}

// Transform a vector out of the local space defined by X,Y,Z,W orthonormal axes
inline Vector transformFromLocalSpace(const Vector& v,
                                      const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& wAxis)
{
    return Vector(v.m_x * xAxis.m_x + v.m_y * yAxis.m_x + v.m_z * zAxis.m_x + v.m_w * wAxis.m_x,
                  v.m_x * xAxis.m_y + v.m_y * yAxis.m_y + v.m_z * zAxis.m_y + v.m_w * wAxis.m_y,
                  v.m_x * xAxis.m_z + v.m_y * yAxis.m_z + v.m_z * zAxis.m_z + v.m_w * wAxis.m_z,
                  v.m_x * xAxis.m_w + v.m_y * yAxis.m_w + v.m_z * zAxis.m_w + v.m_w * wAxis.m_w);
}



struct Rotate4{
    float m_xy, m_xz, m_xw, m_yz, m_yw, m_zw;

    Rotate4()                         : m_xy(0), m_xz(0), m_xw(0), m_yz(0), m_yw(0), m_zw(0){}
    Rotate4(float a)                  : m_xy(a), m_xz(a), m_xw(a), m_yz(a), m_yw(a), m_zw(a){}
    Rotate4(float a, float b, float c, float d, float e, float f): m_xy(a), m_xz(b), m_xw(c), m_yz(d), m_yw(e), m_zw(f){}

};

struct Transform4D{
    Mat5 m_matrix;          //multiplying this by a Vector transforms the Vector into object space
    Mat5 m_invMatrix;       //multiplying this by a Vector transforms the Vector into world space
    Point m_translate;
    float m_xy, m_xz, m_xw, m_yz, m_yw, m_zw;   //the rotations

    Transform4D(){
        m_matrix = Mat5();
        m_invMatrix = Mat5();
    }

    void rotate(float xy, float xz, float xw, float yz, float yw, float zw){
        float pi_180 = M_PI / 180.0f;
        m_xy = xy * pi_180;
        m_xz = xz * pi_180;
        m_xw = xw * pi_180;
        m_yz = yz * pi_180;
        m_yw = yw * pi_180;
        m_zw = zw * pi_180;
        calcMatrices();
    }

    void translate(const Point& t){
        m_translate = t;
        calcMatrices();
    }

    void translate(float x, float y, float z, float w){
        translate(Point(x, y, z, w));
    }

    //this needs to be called before trying to transform Points and Vectors
    void calcMatrices(){    //perform all of the matrix multiplication to generate the m_matrix and m_invMatrix

        /*                 x  y  z  w
        float yw[5][5] = {{1, 0, 0, 0, 0},  //x
                          {0, 1, 0, 0, 0},  //y
                          {0, 0, 1, 0, 0},  //z
                          {0, 0, 0, 1, 0},  //w
                          {0, 0, 0, 0, 1}};*/
        //printf("Start composing the transformation matrix.\n");
        //get an identity matrix
        m_matrix = Mat5();
        //m_matrix.printMat();
        //multiply it by all of the rotation matrices
        //zw
        float zw[5][5] = {{1, 0, 0, 0, 0},
                          {0, 1, 0, 0, 0},
                          {0, 0, cos(m_zw), sin(m_zw), 0},
                          {0, 0, -sin(m_zw), cos(m_zw), 0},
                          {0, 0, 0, 0, 1}};
        m_matrix *= Mat5(zw);
        //m_matrix.printMat();
        //yw
        //                    y     w
        float yw[5][5] = {{1, 0, 0, 0, 0},
                          {0, cos(m_yw), 0, sin(m_yw), 0},  //y
                          {0, 0, 1, 0, 0},
                          {0, -sin(m_yw), 0, cos(m_yw), 0},  //w
                          {0, 0, 0, 0, 1}};
        m_matrix *= Mat5(yw);
        //m_matrix.printMat();
        //yz
        //                    y  z
        float yz[5][5] = {{1, 0, 0, 0, 0},
                          {0, cos(m_yz), sin(m_yz), 0, 0},  //y
                          {0, -sin(m_yz), cos(m_yz), 0, 0},  //z
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}};
        m_matrix *= Mat5(yz);
        //m_matrix.printMat();
        //xw
        //                 x        w
        float xw[5][5] = {{cos(m_xw), 0, 0, sin(m_xw), 0},  //x
                          {0, 1, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {-sin(m_xw), 0, 0, cos(m_xw), 0},  //w
                          {0, 0, 0, 0, 1}};
        m_matrix *= Mat5(xw);
        //m_matrix.printMat();
        //xz
        //                 x     z
        float xz[5][5] = {{cos(m_xz), 0, -sin(m_xz), 0, 0},  //x
                          {0, 1, 0, 0, 0},
                          {sin(m_xz), 0, cos(m_xz), 0, 0},  //z
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}};
        m_matrix *= Mat5(xz);
        //m_matrix.printMat();
        //xy
        //                 x  y
        float xy[5][5] = {{cos(m_xy), sin(m_xy), 0, 0, 0},  //x
                          {-sin(m_xy), cos(m_xy), 0, 0, 0},  //y
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}};
        m_matrix *= Mat5(xy);
        //m_matrix.printMat();
        //multiply it by the translation matrix
        float trans[5][5] = {{1, 0, 0, 0, 0},
                            {0, 1, 0, 0, 0},
                            {0, 0, 1, 0, 0},
                            {0, 0, 0, 1, 0},
                            {-m_translate.m_x, -m_translate.m_y, -m_translate.m_z, -m_translate.m_w, 1}};
        m_matrix *= Mat5(trans);
        //m_matrix.printMat();

        //get an identity matrix
        m_invMatrix = Mat5();
        //multiply it by the inverse translation matrix
        float invtrans[5][5] = {{1, 0, 0, 0, 0},
                            {0, 1, 0, 0, 0},
                            {0, 0, 1, 0, 0},
                            {0, 0, 0, 1, 0},
                            {m_translate.m_x, m_translate.m_y, m_translate.m_z, m_translate.m_w, 1}};
        m_invMatrix *= Mat5(invtrans);
        //multiply it by all of the inverse rotation matrices
        //xy
        //                 x  y
        float xy2[5][5] = {{cos(m_xy), -sin(m_xy), 0, 0, 0},  //x
                          {sin(m_xy), cos(m_xy), 0, 0, 0},  //y
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}};
        m_invMatrix *= Mat5(xy2);
        //xz
        //                 x     z
        float xz2[5][5] = {{cos(m_xz), 0, -sin(m_xz), 0, 0},  //x
                          {0, 1, 0, 0, 0},
                          {sin(m_xz), 0, cos(m_xz), 0, 0},  //z
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}};
        m_invMatrix *= Mat5(xz2);
        //xw
        //                 x        w
        float xw2[5][5] = {{cos(m_xw), 0, 0, -sin(m_xw), 0},  //x
                          {0, 1, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {sin(m_xw), 0, 0, cos(m_xw), 0},  //w
                          {0, 0, 0, 0, 1}};
        m_invMatrix *= Mat5(xw2);
        //yz
        //                    y  z
        float yz2[5][5] = {{1, 0, 0, 0, 0},
                          {0, cos(m_yz), -sin(m_yz), 0, 0},  //y
                          {0, sin(m_yz), cos(m_yz), 0, 0},  //z
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}};
        m_invMatrix *= Mat5(yz2);
        //yw
        //                    y     w
        float yw2[5][5] = {{1, 0, 0, 0, 0},
                          {0, cos(m_yw), 0, -sin(m_yw), 0},  //y
                          {0, 0, 1, 0, 0},
                          {0, sin(m_yw), 0, cos(m_yw), 0},  //w
                          {0, 0, 0, 0, 1}};
        m_invMatrix *= Mat5(yw2);
        //zw
        float zw2[5][5] = {{1, 0, 0, 0, 0},
                          {0, 1, 0, 0, 0},
                          {0, 0, cos(m_zw), -sin(m_zw), 0},
                          {0, 0, sin(m_zw), cos(m_zw), 0},
                          {0, 0, 0, 0, 1}};
        m_invMatrix *= Mat5(zw2);
    }
};


} // namespace Rayito


#endif // __RMATH_H__
