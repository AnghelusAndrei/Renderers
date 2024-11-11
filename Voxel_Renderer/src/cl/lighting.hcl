#pragma once

#include "./src/cl/functions.hcl"
#include "./src/cl/Data.hcl"
#include "./src/cl/ray.hcl"

const float3 background = {40, 30, 10};
#define randoms 5324


float3 VXGI(float3 pos, voxel target, float8 *lights, int n, uint4 *map, int8 voxel_data, int l_len){
    float3 c = background;

    if(target.hit){


        float3 normal;

        if(voxel_data.x){
            normal.x = (float)target.color.x / 100000 - 1;
            normal.y = (float)target.color.y / 100000 - 1;
            normal.z = (float)target.color.z / 100000 - 1;

            target.color.x = target.color.x % 1000;
            target.color.y = target.color.y % 1000;
            target.color.z = target.color.z % 1000;

            if(normal.x == 0 && normal.y == 0 && normal.z == 0)normal = target.normal;
        }else{
            for(int x = -1; x <= 1; x++){
                for(int y = -1; y <= 1; y++){
                    for(int z = -1; z <= 1; z++){
                        leaf q = Lookup((uint3){floor(target.position.x) + x,floor(target.position.y) + y,floor(target.position.z) + z},map,n);
                        if(q.type <= 1 || !inBounds((float3){floor(target.position.x) + x,floor(target.position.y) + y,floor(target.position.z) + z}, n-1))normal += (float3)(x,y,z);
                    }
                }
            }
            if(normal.x != 0 || normal.y != 0 || normal.z != 0) normal = normalize(normal);
            else normal = target.normal;
        }

        float3 pixel = target.position + target.normal*f_error*2;
        target.position = (float3)(floor(target.position.x)+0.5, floor(target.position.y)+0.5, floor(target.position.z)+0.5);

        float3 lighting = {0,0,0};

        float3 ambient = convert_float3(target.color) * ((float)(voxel_data.w)/100);
        c = ambient;

        for(int i = 0; i < l_len; i++){
            float3 light = {lights[i].s0, lights[i].s1, lights[i].s2};

            if(dot(normal, normalize(light - target.position)) < 0)continue;

            float area = lights[i].s4;
            float3 local_color = {0,0,0};
            float r = lights[i].s3;
            float3 l_col = {lights[i].s5/255, lights[i].s6/255, lights[i].s7/255};

            float3 light_vector = normalize(light - target.position);
            voxel illumination = TraceShadow(target.position, light, n,map,100);

            float3 diffuse = multiply_vec(convert_float3(target.color), dot_clamped(light_vector, normal));

            if(!illumination.hit){
                float3 reflection_vec = normalize(-light_vector - dot(-light_vector,normal) * normal * 2);

                float3 c_y = c_y = normalize(cross((float3){0,0,1}, reflection_vec));
                float3 r_vec = normalize(cross(c_y, reflection_vec)) * r;

                float c_angle = dot_clamped(reflection_vec, normalize(reflection_vec+r_vec));
                
                float specular = dot_clamped(reflection_vec, normalize(pos - (target.position + target.normal/2 + target.normal * (float)0.03)))/c_angle;
                if(specular > 1)specular += (specular-1)*10;
                specular = pow(specular, 100-voxel_data.z);

                local_color += (diffuse + specular)*l_col;
            }else{
                /*int4 random;

                ulong seed = randoms + target.position.x;
                seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
                random.x = seed >> 16%10-5;

                seed = randoms + target.octree_index/3;
                seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
                random.y = seed >> 16%10-5;

                seed = randoms + target.octree_index/2;
                seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
                random.z = seed >> 16%10-5;

                seed = randoms + target.position.y;
                seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
                random.w = seed >> 16%10-5;


                float3 sample_vec1,sample_vec2;
                voxel sample_1,sample_2;

                float3 v_x = ((float)(random.x)/10) * normalize(cross((float3){0,0,1}, light_vector));
                float3 c_y = normalize(cross((float3){0,0,1}, light_vector));
                float3 v_y = ((float)(random.y)/10) * normalize(cross(c_y, light_vector));

                sample_vec1 = normalize(v_x + v_y) * r;

                v_x = ((float)(random.z)/10) * normalize(cross((float3){0,0,1}, light_vector));
                v_y = ((float)(random.w)/10) * normalize(cross(c_y, light_vector));

                sample_vec2 = normalize(v_x + v_y) * r;

                sample_1 = TraceShadow(target.position, light+sample_vec1, n,map,100);
                sample_2 = TraceShadow(target.position, light+sample_vec2, n,map,100);

                float shadow_accumulation = 0;

                if(!sample_1.hit)shadow_accumulation+=0.35;
                if(!sample_2.hit)shadow_accumulation+=0.35;

                local_color += shadow_accumulation * diffuse * l_col;*/


                voxel reverse = TraceShadow(light, target.position, n,map,100);

                float accumulation_delta = length(illumination.position - reverse.position)/length(target.position - reverse.position);
                float relative_depth = (length(target.position - illumination.position)/length(target.position - reverse.position));
                float soft_shadow = pow(clamp((float)(1-accumulation_delta), (float)0, (float)1), (1 - r + (relative_depth))*10);

                if(soft_shadow < 1){
                    local_color += soft_shadow * diffuse * l_col;
                }else{
                    float3 reflection_vec = normalize(-light_vector - dot(-light_vector,normal) * normal * 2);

                    float3 c_y = c_y = normalize(cross((float3){0,0,1}, reflection_vec));
                    float3 r_vec = normalize(cross(c_y, reflection_vec)) * r;

                    float c_angle = dot_clamped(reflection_vec, normalize(reflection_vec+r_vec));
                    
                    float specular = dot_clamped(reflection_vec, normalize(pos - (target.position + target.normal/2 + target.normal * (float)0.03)))/c_angle;
                    specular = pow(specular, (100-voxel_data.z)*10);

                    local_color += (diffuse + specular)*l_col;
                }
            }

            lighting+=local_color;
        }

        c+=lighting/l_len;

    }else if(target.position.x == 1)c-=background;

    return c;
} 

float3 PXGI(float3 pos, voxel target, float8 *lights, int n, uint4 *map, int8 voxel_data, int l_len){
    float3 c = background;

    if(target.hit){


        float3 normal;

        /*if(voxel_data.x){
            normal.x = (float)target.color.x / 100000 - 1;
            normal.y = (float)target.color.y / 100000 - 1;
            normal.z = (float)target.color.z / 100000 - 1;

            target.color.x = target.color.x % 1000;
            target.color.y = target.color.y % 1000;
            target.color.z = target.color.z % 1000;

            if(normal.x == 0 && normal.y == 0 && normal.z == 0)normal = target.normal;
        }else{
            for(int x = -1; x <= 1; x++){
                for(int y = -1; y <= 1; y++){
                    for(int z = -1; z <= 1; z++){
                        leaf q = Lookup((uint3){floor(target.position.x) + x,floor(target.position.y) + y,floor(target.position.z) + z},map,n);
                        if(q.type <= 1 || !inBounds((float3){floor(target.position.x) + x,floor(target.position.y) + y,floor(target.position.z) + z}, n-1))normal += (float3)(x,y,z);
                    }
                }
            }
            if(normal.x != 0 || normal.y != 0 || normal.z != 0) normal = normalize(normal);
            else normal = target.normal;
        }*/
        if(voxel_data.x){
            target.color.x = target.color.x % 1000;
            target.color.y = target.color.y % 1000;
            target.color.z = target.color.z % 1000;
        }

        normal = target.normal;

        float3 pixel = target.position + target.normal*f_error*2;
        //target.position = (float3)(floor(target.position.x)+0.5, floor(target.position.y)+0.5, floor(target.position.z)+0.5);
        target.position=pixel;

        float3 lighting = {0,0,0};

        float3 ambient = convert_float3(target.color) * ((float)(voxel_data.w)/100);
        c = ambient;

        for(int i = 0; i < l_len; i++){
            float3 light = {lights[i].s0, lights[i].s1, lights[i].s2};

            if(dot(normal, normalize(light - target.position)) < 0)continue;

            float area = lights[i].s4;
            float3 local_color = {0,0,0};
            float r = lights[i].s3;
            float3 l_col = {lights[i].s5/255, lights[i].s6/255, lights[i].s7/255};

            float3 light_vector = normalize(light - target.position);
            voxel illumination = TraceShadow(target.position, light, n,map,100);

            float3 diffuse = multiply_vec(convert_float3(target.color), dot_clamped(light_vector, normal));

            if(!illumination.hit){
                float3 reflection_vec = normalize(-light_vector - dot(-light_vector,normal) * normal * 2);

                float3 c_y = c_y = normalize(cross((float3){0,0,1}, reflection_vec));
                float3 r_vec = normalize(cross(c_y, reflection_vec)) * r;

                float c_angle = dot_clamped(reflection_vec, normalize(reflection_vec+r_vec));
                
                float specular = dot_clamped(reflection_vec, normalize(pos - (target.position + target.normal/2 + target.normal * (float)0.03)))/c_angle;
                if(specular > 1)specular += (specular-1)*10;
                specular = pow(specular, 100-voxel_data.z);

                local_color += (diffuse + specular)*l_col;
            }else{

                local_color += diffuse * l_col * voxel_data.s3/100;
            }

            lighting+=local_color;
        }

        c+=lighting/l_len;

    }else if(target.position.x == 1)c-=background;

    return c;
}