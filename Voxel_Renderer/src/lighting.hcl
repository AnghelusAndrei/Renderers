#pragma once

#include "shd/functions.hcl"
#include "shd/Data.hcl"
#include "shd/ray.hcl"

const float shadowCoefficent = 0.4;

float3 CalculateLighting(float3 vector, ray_data voxel, float3 light, float reflectionCoefficent, int n, uint4 *map){
    float3 c = convert_float3(voxel.color);

    if(voxel.hit){

        ray_data illumination;

        float3 reflection_vec = vector - dot(vector,voxel.normal) * voxel.normal * 2;
        float3 add_v = voxel.normal * f_error * 2;
        voxel.position = voxel.position + add_v;
        float3 light_vector = normalize(light-voxel.position);
        illumination = RayCast(voxel.position, light, light_vector,n,map,50);

        /*if(reflectionCoefficent > 0){
            reflectionCoefficent -= 0.5;

            ray_data reflection = RayCast(voxel.position, voxel.position + reflection_vec * (float)1e9, normalize(reflection_vec),n,map,35);

            c = (c + CalculateLighting(reflection_vec, reflection, light, reflectionCoefficent, n, map) * reflectionCoefficent) / (float)( 1 + reflectionCoefficent + 0.05);
        }*/

        if(!illumination.hit){
            float lighting = (dot(light_vector, reflection_vec) + 1) / 2 + shadowCoefficent;

            c = c * lighting;

        }else{
            c = c * shadowCoefficent;
        }

    }else{
        return (float3){0,0,0};
    }

    return c;
}   
