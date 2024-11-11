#pragma once


#define LEAF 0
#define NODE 1

int depth;

typedef struct leaf leaf;
struct leaf{
    float3 data;
    uint size;
    uint type;
    uint index;
}; 

int Locate(uint3 pos, int depth, int n, int p2){
    float a=n/p2;
    float b=n/(p2*2);

    return (int)((pos.x - floor(pos.x/a)*a)/b) + 
        2*(int)((pos.y - floor(pos.y/a)*a)/b)+
        4*(int)((pos.z - floor(pos.z/a)*a)/b);
}


leaf Lookup(uint3 pos, uint4 octree[], int m){
    int d=1;
    int offset = 0;
    int p2=1;
    while(d<=depth){
        int i = offset+Locate(pos,d,m,p2);
        int s = m/(p2*2);

        switch (octree[i].w)
        {
        case LEAF:
            return (leaf){
                (float3){(pos.x/s)*s,(pos.y/s)*s,(pos.z/s)*s},
                (uint){s},
                octree[i].w,
                i
            };
        case NODE:
            offset = octree[i].x;
            break;
        default:
            return (leaf){
                (float3){octree[i].x,octree[i].y,octree[i].z},
                (uint){s},
                octree[i].w,
                i
            };
        }

        d++;
        p2*=2;
    }
}
