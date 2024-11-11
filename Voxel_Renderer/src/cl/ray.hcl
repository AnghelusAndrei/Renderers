#pragma once
#include "./src/cl/Data.hcl"
#include "./src/cl/functions.hcl"


const float f_error = 0.001;
const float b_error = 0;


typedef struct voxel voxel;
struct voxel{
  uint3 color;
  float3 position;
  float3 normal;
  bool hit;
  uint type;
  uint q;
  uint octree_index;
}; 



voxel RayCast(float3 pos, float dist, float3 vector, int n, uint4 *octree, int max_steps){

    float3 normal = {0,0,0};
    float3 ray = pos;
    float3 x,y,z;
    float3 v_add;
    int3 found = {1,1,1};
    int q = 0;
    bool inQ = true;
    bool out = true;

    while(q<max_steps) {

        float d = 1e9;
        float xD,yD,zD;

        x=(float3){0,0,0};
        y=(float3){0,0,0};
        z=(float3){0,0,0};
        v_add = (float3){0,0,0};

        if(inBoundsD(ray,n+1,-1))out = false;

        while(!inBounds(ray,n)){
            inQ = false;

            if(found.x == 1){
                if(ray.x < 0){
                    if(vector.x <= 0)found.x = 0;
                    else{
                    x.x = f_error-ray.x;
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
                if(ray.y < 0){
                    if(vector.y <= 0)found.y = 0;
                    else{
                    y.y = f_error-ray.y;
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
                if(ray.z < 0){
                    if(vector.z <= 0)found.z = 0;
                    else{
                    z.z = f_error-ray.z;
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
                return (voxel){(uint3){1,1,1},(float3){(int)out,0,0},(float3){0,0,0},false,0,q};
            }
 
            ray = ray+v_add;
            d = 1e9;

            x=(float3){0,0,0};
            y=(float3){0,0,0};
            z=(float3){0,0,0};
            v_add = (float3){0,0,0};

            q++;
        }

        if(vec3Distance(pos,ray) >= dist){
            return (voxel){(uint3){1,1,1},(float3){(int)out,0,0},(float3){0,0,0},false,0,q};
        }

        leaf target = Lookup(convert_uint3(floor(ray)),octree,n);

        if(target.type > 1){
            if(inQ == false){
                float3 v_normal = {
                    (float)target.data.x / 100000 - 1,
                    (float)target.data.y / 100000 - 1,
                    (float)target.data.z / 100000 - 1
                };

                    return (voxel){
                        (uint3){target.data.x,target.data.y,target.data.z},
                        ray,
                        normal,
                        true,
                        target.type,
                        q,
                        target.index
                    };

                /*if(dot(v_normal, vector) < 0.5){
                    return (voxel){
                        (uint3){target.data.x,target.data.y,target.data.z},
                        ray,
                        normal,
                        true,
                        target.type,
                        q,
                        target.index
                    };
                }else{
                    target.data = floor(ray);
                }*/
            }else{
                target.data = (float3){floor(ray.x), floor(ray.y), floor(ray.z)};
            }
        }else{
            inQ = false;
        }


        x.x = (target.data.x-ray.x) + (float)(vector.x>0)*target.size + semn(vector.x)*f_error;
        x.y = (x.x*vector.y)/vector.x;
        x.z = (x.x*vector.z)/vector.x;

        y.y = (target.data.y-ray.y) + (float)(vector.y>0)*target.size + semn(vector.y)*f_error;
        y.x = (y.y*vector.x)/vector.y;
        y.z = (y.y*vector.z)/vector.y;

        z.z = (target.data.z-ray.z) + (float)(vector.z>0)*target.size + semn(vector.z)*f_error;
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

    return (voxel){(uint3){1,1,1},(float3){(int)out,0,0},(float3){0,0,0},false,0,q};
}



voxel TraceShadow(float3 pos, float3 dest, int n, uint4 *octree, int max_steps){

    float3 vector = normalize(dest - pos);

    float3 normal = {0,0,0};
    float3 ray = pos;
    float3 x,y,z;
    float3 v_add;
    int3 found = {1,1,1};
    int q = 0;
    bool inQ = true;


    while(q<max_steps) {

        float d = 1e9;
        float xD,yD,zD;

        x=(float3){0,0,0};
        y=(float3){0,0,0};
        z=(float3){0,0,0};
        v_add = (float3){0,0,0};


        if(vec3Distance(pos,ray) >= vec3Distance(pos,dest)){
            return (voxel){(uint3){1, 1, 1},(float3){0,0,0},(float3){0,0,0},false,0};
        }

        if(!inBounds(ray,n)){
            return (voxel){(uint3){1, 1, 1},(float3){0,0,0},(float3){0,0,0},false,0};
        }

        leaf target = Lookup(convert_uint3(floor(ray)),octree,n);

        if(target.type > 1){
            if(inQ == false){
                float3 v_normal = {
                    (float)target.data.x / 100000 - 1,
                    (float)target.data.y / 100000 - 1,
                    (float)target.data.z / 100000 - 1
                };

                if(dot(v_normal, vector) < 0.1){
                    return (voxel){
                        (uint3){target.data.x,target.data.y,target.data.z},
                        ray,
                        normal,
                        true,
                        target.type,
                        q
                    };
                }else{
                    target.data = floor(ray);
                }
            }else{
                target.data = (float3){floor(ray.x), floor(ray.y), floor(ray.z)};
            }
        }else{
            inQ = false;
        }


        x.x = (target.data.x-ray.x) + (float)(vector.x>0)*target.size + semn(vector.x)*f_error;
        x.y = (x.x*vector.y)/vector.x;
        x.z = (x.x*vector.z)/vector.x;

        y.y = (target.data.y-ray.y) + (float)(vector.y>0)*target.size + semn(vector.y)*f_error;
        y.x = (y.y*vector.x)/vector.y;
        y.z = (y.y*vector.z)/vector.y;

        z.z = (target.data.z-ray.z) + (float)(vector.z>0)*target.size + semn(vector.z)*f_error;
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

    return (voxel){(uint3){1, 1, 1},(float3){0,0,0},(float3){0,0,0},false,0};
}

