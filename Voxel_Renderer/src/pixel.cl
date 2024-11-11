#include "shd/functions.hcl"
#include "shd/Data.hcl"
#include "shd/ray.hcl"
#include "shd/lighting.hcl"


const float reflectionCoefficent = 0;



__kernel void compute_pixels_kernel(__global uint4 *map, __global uchar3 *doutput,
                                    const int n, const int FOV,
                                    const float3 pos, const float3 vec, const float3 light, const int d) {
                                      
    size_t index_x= get_global_id(0);
    size_t index_y = get_global_id(1);
    float2 resolution = {get_num_groups(0) * get_local_size(0), get_num_groups(1) * get_local_size(1)};
    size_t index = index_y * convert_int(resolution.x) + index_x;
    depth = d;

    float f = tan(radians((float)FOV/2));

    float2 PlaneScale;
    PlaneScale.x = 2 * (float)index_x / resolution.x - 1;
    PlaneScale.y = 2 * (float)index_y / resolution.y - 1;

    float aspectRatio = resolution.y/resolution.x;
    float3 v_x,v_y, c_y;

    v_x = PlaneScale.x * normalize(cross((float3){0,0,1}, vec)) * f;
    c_y = normalize(cross((float3){0,0,1}, vec));
    v_y = PlaneScale.y * normalize(cross(c_y, vec)) * aspectRatio * f;

    float3 vector = vec + v_x + v_y;
    vector = normalize(vector);
    
    ray_data voxel;
    
    voxel = RayCast(pos, pos + vector * (float)1e9,vector,n,map,50);

    
    doutput[index] = Color(CalculateLighting(vector, voxel, light, reflectionCoefficent, n, map));
}