#pragma once
#include "../utils/PerlinNoise.hpp"
#include <iostream>
#include <stack>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define LEAF 0
#define NODE 1

typedef struct leaf leaf;
struct leaf{
    cl_float3 data;
    cl_uint size;
    cl_uint type;
    int index;
}; 

typedef struct voxel voxel;
struct voxel{
  cl_uchar3 color;
  cl_float3 position;
  cl_float3 normal;
}; 

long OCTREE_INDEX = 8;


class Octree{
    private:
        std::stack<int> free_mem;
        long OCTREE_LENGTH = 0;

        int Locate(cl_uint3 pos, int depth, int p2){
            int a=n/p2;
            int b=n/(p2*2);

            return (pos.x%a)/b + 
                2*((pos.y%a)/b)+
                4*((pos.z%a)/b);
        }

    public:

        int depth;
        int n;
        cl_uint4 * octree;
        bool upToDate = true;

        void Initialize(int d);
        void Insert(cl_uint3 pos, cl_uint3 col, cl_uint type);
        void Remove(cl_uint3 pos);
        leaf Lookup(cl_uint3 pos);
};

void Octree::Initialize(int d){
    depth = d;
    n = 1<<depth;
    int o_m = 8;

    for(int i = 0;i<depth;i++){
        OCTREE_LENGTH += o_m;
        o_m *= 8;
    }

    octree = new cl_uint4[OCTREE_LENGTH];
}


void Octree::Insert(cl_uint3 pos, cl_uint3 col, cl_uint type){
    int d=1;
    int offset = 0;
    int p2 = 1;
    int j = 0;

    while(d<depth){
        int i = offset+Locate(pos,d,p2);

        switch (octree[i].w)
        {
            case LEAF:
                if(free_mem.empty()){
                    offset = OCTREE_INDEX;
                    OCTREE_INDEX+=8;
                }else{
                    offset = free_mem.top();
                    free_mem.pop();
                }
                octree[i].x=offset;
                octree[i].w = NODE;
                octree[i].z = 0;
                if(d>1)octree[j].z++;
                j = i;
                break;
            case NODE:
                j = i;
                offset = octree[i].x;
                break;
        }

        d++;
        p2*=2;
    }

    int i = offset+Locate(pos,d,p2);
    if(octree[i].w <= 1)octree[j].z++;
    octree[i]={
        col.x,
        col.y,
        col.z,
        type
    };
}


void Octree::Remove(cl_uint3 pos){
    int d=1;
    int offset = 0;
    int p2=1;
    std::stack<int> nodes;

    while(d<=depth){
        int i = offset+Locate(pos,d,p2);

        switch (octree[i].w)
        {
        case LEAF:
            return;
        case NODE:
            nodes.push(i);
            offset = octree[i].x;
            break;
        default:
            octree[i] = {0,0,0,0};
            octree[nodes.top()].z--;

            while(octree[nodes.top()].z <= 0 && !nodes.empty()){
                free_mem.push(octree[nodes.top()].x);
                octree[nodes.top()] = {0,0,0,0};
                nodes.pop();
                octree[nodes.top()].z--;
            }
            return;
        }

        d++;
        p2*=2;
    }

}

leaf Octree::Lookup(cl_uint3 pos){
    int d=1;
    int offset = 0;
    int p2=1;
    while(d<=depth){
        int i = offset+Locate(pos,d,p2);
        int s = n/(p2*2);

        switch (octree[i].w)
        {
        case LEAF:
            return (leaf){
                (cl_float3){(pos.x/s)*s,(pos.y/s)*s,(pos.z/s)*s},
                (uint){s},
                octree[i].w,
                i
            };
        case NODE:
            offset = octree[i].x;
            break;
        default:
            return (leaf){
                (cl_float3){octree[i].x,octree[i].y,octree[i].z},
                (uint){1},
                octree[i].w,
                i
            };
        }

        d++;
        p2*=2;
    }
}