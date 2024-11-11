#pragma once
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_CHANNELS 50
#define MAX_LIGHTS 50

typedef struct channel
{
    cl_uint type;

    cl_bool pregenerated_normals;
    cl_bool reflective;
    cl_float roughness;
    cl_float ambient_intensity;
    cl_float reflectivity;
    cl_float opacity;
}channel;

typedef struct light
{
    cl_int index;
    cl_float3 position;
    cl_int3 color;
    cl_float radius;
    cl_float area;
}light;

class properties{
    public:
        cl_int8 * data = new cl_int8[MAX_CHANNELS];
        cl_float8 * lights = new cl_float8[MAX_LIGHTS];
        int index_materials = 0;
        int index_lights = 0;

        void AddChannel(channel &_data);
        void AddLight(light &l);

        //global data
        cl_uint3 background;
        //----
    private:
};

void properties::AddChannel(channel &_data){
    if(index_materials < MAX_CHANNELS){
        _data.type = index_materials + 2;
        data[index_materials] = {
            (cl_int)_data.pregenerated_normals, 
            (cl_int)_data.reflective, 
            (cl_int)(_data.roughness*100),
            (cl_int)(_data.ambient_intensity*100),
            (cl_int)(_data.reflectivity*100),
            (cl_int)(_data.opacity*100),
            0,
            0
        };
        index_materials++;
    }
    return;
}

void properties::AddLight(light &l){
    if(index_lights < MAX_LIGHTS){
        l.index = index_lights;
        lights[index_lights] = {
            (cl_float)l.position.x, 
            (cl_float)l.position.y, 
            (cl_float)l.position.z,
            (cl_float)l.radius,
            (cl_float)l.area,
            (cl_float)l.color.x,
            (cl_float)l.color.y,
            (cl_float)l.color.z
        };
        index_lights++;
    }
    return;
}