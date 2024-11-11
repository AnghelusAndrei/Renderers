#pragma once
#include <bits/stdc++.h>
#include "octree.h"
#include "debug.h"
#include "config.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


void GenerateOctree3D(Octree octree, config_ d){
    const siv::PerlinNoise perlin{ d.seed };
    if(d.density<0)d.density=0;
    if(d.density>255)d.density=255;
    
    int q = 0;

    for(int k=0;k<octree.n;k++){
        if((int)(((float)k/(float)octree.n)*100)>q){q=(int)(((float)k/(float)octree.n)*100);std::cout<<q<<'%'<<std::endl;}

        for(int i=0;i<octree.n;i++){
            for(int j=0;j<octree.n;j++){
                int x=j,y=i,z=k;
                int noiseData = (int)(perlin.octave3D_01((x*0.01f), (y*0.01f), (z*0.01f), d.octaves)*255);
                if(noiseData<d.density){

                    cl_uint3 col = {
                        (int)(perlin.octave3D_01((x*0.01f), (y*0.01f), (z*0.02f), d.octaves)*255),
                        (int)(perlin.octave3D_01((x*0.01f), (y*0.02f), (z*0.01f), d.octaves)*255),
                        (int)(perlin.octave3D_01((x*0.02f), (y*0.01f), (z*0.01f), d.octaves)*255)
                    };
                    
                    octree.Insert((cl_uint3){x,y,z}, col);
                }
            }
        }
    }
    
}



void GenerateOctree2D(Octree octree, config_ d){
    const siv::PerlinNoise perlin{ d.seed };
    
    int q = 0;

    for(int k=0;k<octree.n;k++){
        if((int)(((float)k/(float)octree.n)*100)>q){q=(int)(((float)k/(float)octree.n)*100);std::cout<<q<<'%'<<std::endl;}

        for(int i=0;i<octree.n;i++){
            int x=k,y=i;
            int z = (int)(perlin.octave2D_01((x*0.01f), (y*0.01f), d.octaves)*d.height);

            cl_uint3 col = {
                (int)(perlin.octave2D_01((x*0.01f), (y*0.02f), d.octaves)*255),
                (int)(perlin.octave2D_01((x*0.005f), (y*0.005f), d.octaves)*255),
                (int)(perlin.octave2D_01((x*0.02f), (y*0.01f), d.octaves)*255)
            };
                    
            for(int j = z; j > z-10 && j > 1; j--){
                octree.Insert((cl_uint3){x,y,j}, col);
            }
        }
    }
    
}