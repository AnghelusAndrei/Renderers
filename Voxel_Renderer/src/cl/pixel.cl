#include "./src/cl/functions.hcl"
#include "./src/cl/Data.hcl"
#include "./src/cl/ray.hcl"
#include "./src/cl/lighting.hcl"

__kernel void compute_pixels_kernel(__global uint4 *map, __global uchar3 *doutput,
                                    const int4 config,
                                    const float3 pos, const float3 vec, __global int8 *voxel_data, __global float8 *lights) {
    const int n = config.x;
    const int FOV = config.y;
    const int l_len = config.w;

    uint index_x= get_global_id(0);
    uint index_y = get_global_id(1);
    float2 resolution = {get_num_groups(0) * get_local_size(0), get_num_groups(1) * get_local_size(1)};
    size_t index = index_y * convert_int(resolution.x) + index_x;

    depth = config.z;

    float f = tan(radians((float)FOV/2));

    float2 PlaneScale;
    PlaneScale.x = 2 * (float)(index_x) / resolution.x - 1;
    PlaneScale.y = 2 * (float)(index_y) / resolution.y - 1;

    float aspectRatio = resolution.y/resolution.x;
    float3 v_x,v_y, c_y;

    v_x = PlaneScale.x * normalize(cross((float3){0,0,1}, vec)) * f;
    c_y = normalize(cross((float3){0,0,1}, vec));
    v_y = PlaneScale.y * normalize(cross(c_y, vec)) * aspectRatio * f;

    float3 vector = vec + v_x + v_y;
    vector = normalize(vector);
    
    voxel target;
    
    target = RayCast(pos, vec3Distance(0,0 + vector * (float)1e9),vector,n,map,70);

    //doutput[index] = Color((float3){target.q*5, target.q*5, target.q*5}); // OCTREE VISUALIZATION
    doutput[index] = Color(VXGI(pos, target, lights, n, map, voxel_data[target.type-2], l_len));
}