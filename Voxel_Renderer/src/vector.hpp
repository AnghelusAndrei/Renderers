#pragma once
#include <bits/stdc++.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)


class Vector{
    public:
        float x,y,z,w;
        Vector(){
            x=0;
            y=0;
            z=0;
        }
        Vector(float x1, float y1, float z1){
            x=x1;
            y=y1;
            z=z1;
        }

        Vector(const Vector &vec);
        void EulerAngles(cl_float2 rotation);
        cl_float3 CL();
        cl_uint3 CL_ui();
        void out();


        Vector operator+(const Vector &vec);
        Vector &operator+=(const Vector &vec);
        Vector operator-(const Vector &vec);
        Vector &operator-=(const Vector &vec);
        Vector operator*(float value);
        Vector &operator*=(float value);
        Vector operator/(float value);
        Vector &operator/=(float value);
        Vector &operator=(const Vector &vec);

        float dot_product(const Vector &vec);
        Vector cross_product(const Vector &vec);
        float magnitude();
        Vector normalization();
        float square();

};



Vector::Vector(const Vector &vec){
    x=vec.x;
    y=vec.y;
    z=vec.z;
}

void Vector::EulerAngles(cl_float2 rotation){
    x = cosf(degToRad(rotation.x))*cosf(degToRad(rotation.y));
    y = sinf(degToRad(rotation.x))*cosf(degToRad(rotation.y));
    z = sinf(degToRad(rotation.y));
}

cl_float3 Vector::CL(){
    return (cl_float3){x,y,z};
}

cl_uint3 Vector::CL_ui(){
    return (cl_uint3){x,y,z};
}

void Vector::out(){
    std::cout<<x<<' '<<y<<' '<<z<<' '<<std::endl;
}


Vector Vector::operator+(const Vector &vec)
{
    return Vector(x+vec.x,y+vec.y,z+vec.z);
}

Vector &Vector::operator+=(const Vector &vec)
{
    x+=vec.x;
    y+=vec.y;
    z+=vec.z;
    return *this;
}


Vector Vector::operator-(const Vector &vec)
{
    return Vector(x-vec.x,y-vec.y,z-vec.z);
}

Vector &Vector::operator-=(const Vector &vec)
{
    x-=vec.x;
    y-=vec.y;
    z-=vec.z;
    return *this;
}


Vector Vector::operator*(float value)
{
    return Vector(x*value,y*value,z*value);
}



Vector &Vector::operator*=(float value)
{
    x*=value;
    y*=value;
    z*=value;
    return *this;
}


Vector Vector::operator/(float value)
{
    assert(value!=0);
    return Vector(x/value,y/value,z/value);
}

Vector &Vector::operator/=(float value)
{
    assert(value!=0);
    x/=value;
    y/=value;
    z/=value;
    return *this;
}


Vector &Vector::operator=(const Vector &vec)
{
    x=vec.x;
    y=vec.y;
    z=vec.z;
    return *this;
}



float Vector::dot_product(const Vector &vec)
{
    return x*vec.x+vec.y*y+vec.z*z;
}

Vector Vector ::cross_product(const Vector &vec)
{
    float ni=y*vec.z-z*vec.y;
    float nj=z*vec.x-x*vec.z;
    float nk=x*vec.y-y*vec.x;
    return Vector(ni,nj,nk);
}

float Vector::magnitude()
{
    return sqrt(square());
}

float Vector::square()
{
    return x*x+y*y+z*z;
}

Vector Vector:: normalization()
{
    if(magnitude()==0)return *this;
    *this/=magnitude();
    return *this;
}