#pragma once
#include "shd/Data.hcl"
#include "shd/functions.hcl"


const float f_error = 0.0001;
const float b_error = 0;


typedef struct ray_data ray_data;
struct ray_data{
  uchar3 color;
  float3 position;
  float3 normal;
  bool hit;
}; 



ray_data RayCast(float3 pos, float3 dest, float3 vector, int n, uint4 *octree, int max_steps){

    float3 normal = {0,0,0};
    float3 ray = pos;
    float3 x,y,z;
    float3 v_add;
    int3 found = {1,1,1};
    int3 checked = {0,0,0};
    int q = 0;

    while(q<max_steps) {

        float d = 1e9;
        float xD,yD,zD;

        x=(float3){0,0,0};
        y=(float3){0,0,0};
        z=(float3){0,0,0};
        v_add = (float3){0,0,0};

        while(!inBounds(ray,n,b_error)){

            if(found.x == 1){
                if(ray.x < b_error){
                    if(vector.x <= 0)found.x = 0;
                    else{
                    x.x = b_error+f_error-ray.x;
                    x.y = (x.x*vector.y)/vector.x;
                    x.z = (x.x*vector.z)/vector.x;
                    }
                }
                if(ray.x > n){
                    if(vector.x >= 0)found.x = 0;
                    else{
                    x.x = n-f_error-ray.x;
                    x.y = (x.x*vector.y)/vector.x;
                    x.z = (x.x*vector.z)/vector.x;
                    }
                }
                if(found.x == 1){
                    xD = vec3Distance(ray,ray+x);
                    if(xD < d){d = xD; v_add = x;}
                }
            }


            if(found.y == 1){
                if(ray.y < b_error){
                    if(vector.y <= 0)found.y = 0;
                    else{
                    y.y = b_error+f_error-ray.y;
                    y.x = (y.y*vector.x)/vector.y;
                    y.z = (y.y*vector.z)/vector.y;
                    }
                }
                if(ray.y > n){
                    if(vector.y >= 0)found.y = 0;
                    else{
                    y.y = n-f_error-ray.y;
                    y.x = (y.y*vector.x)/vector.y;
                    y.z = (y.y*vector.z)/vector.y;
                    }
                }
                if(found.y == 1){
                    yD = vec3Distance(ray,ray+y);
                    if(yD < d){d = yD; v_add = y;}
                }
            }


            if(found.z == 1){
                if(ray.z < b_error){
                    if(vector.z <= 0)found.z = 0;
                    else{
                    z.z = b_error+f_error-ray.z;
                    z.x = (z.z*vector.x)/vector.z;
                    z.y = (z.z*vector.y)/vector.z;
                    }
                }
                if(ray.z > n){
                    if(vector.z >= 0)found.z = 0;
                    else{
                    z.z = n-f_error-ray.z;
                    z.x = (z.z*vector.x)/vector.z;
                    z.y = (z.z*vector.y)/vector.z;
                    }
                }
                if(found.z == 1){
                    zD = vec3Distance(ray,ray+z);
                    if(zD < d){d = zD; v_add = z;}
                }
            }

            if((found.x == 0 && found.y == 0 && found.z == 0) || (v_add.x == 0 && v_add.y == 0 && v_add.z == 0)){
                return (ray_data){(uchar3){2,0,0},(float3){0,0,0},(float3){0,0,0},false};
            }
 
            ray = ray+v_add;
            d = 1e9;

            x=(float3){0,0,0};
            y=(float3){0,0,0};
            z=(float3){0,0,0};
            v_add = (float3){0,0,0};
        }

        if(vec3Distance(pos,ray) >= vec3Distance(pos,dest)){
            return (ray_data){(uchar3){1, 0, 0},(float3){0,0,0},(float3){0,0,0},false};
        }

        leaf_data leaf = Lookup(convert_uint3(floor(ray)),octree,n);

        if(leaf.type == CUBE){
            return (ray_data){
                (uchar3){leaf.data.x,leaf.data.y,leaf.data.z},
                ray,
                normal,
                true
            };
        }


        x.x = (leaf.data.x-ray.x) + (float)(vector.x>0)*leaf.size + semn(vector.x)*f_error;
        x.y = (x.x*vector.y)/vector.x;
        x.z = (x.x*vector.z)/vector.x;

        y.y = (leaf.data.y-ray.y) + (float)(vector.y>0)*leaf.size + semn(vector.y)*f_error;
        y.x = (y.y*vector.x)/vector.y;
        y.z = (y.y*vector.z)/vector.y;

        z.z = (leaf.data.z-ray.z) + (float)(vector.z>0)*leaf.size + semn(vector.z)*f_error;
        z.x = (z.z*vector.x)/vector.z;
        z.y = (z.z*vector.y)/vector.z;


        xD = sqrt(x.x*x.x + x.y*x.y + x.z*x.z);
        yD = sqrt(y.x*y.x + y.y*y.y + y.z*y.z);
        zD = sqrt(z.x*z.x + z.y*z.y + z.z*z.z);

        if(xD < d){
            d = xD; 
            v_add = x; 
            normal = (float3){-semn(vector.x),0,0};
        }
        if(yD < d){
            d = yD; 
            v_add = y; 
            normal = (float3){0,-semn(vector.y),0};
        }
        if(zD < d){
            d = zD; 
            v_add = z; 
            normal = (float3){0,0,-semn(vector.z)};
        }
        

        ray += v_add;
        q++;
    }

    return (ray_data){(uchar3){0, 0, 0},(float3){0,0,0},(float3){0,0,0},false};
}
