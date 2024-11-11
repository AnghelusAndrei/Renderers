#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"

#include "renderer.h"
#include "physics.h"

#include "Cl_utils.h"
#include "Input.h"
#include "debug.h"
#include "octree.h"
#include "object.h"
#include "channels.h"


#define TEAPOT_FILE "./assets/teapot.obj"
#define WORLD_FILE "./assets/world.obj"
#define WORLD_TEXTURE "./assets/texture.jpg"


static int PhysicsEngine(void *data){

    console.StartTime();


    channel VOX1 = {
        .pregenerated_normals = true,
        .reflective = false,
        .roughness = 0.4,
        .ambient_intensity = 0.4,
        .reflectivity = 0,
        .opacity = 1
    };

    channel VOX2 = {
        .pregenerated_normals = true,
        .reflective = false,
        .roughness = 0.7,
        .ambient_intensity = 0.2,
        .reflectivity = 0,
        .opacity = 1
    };

    channel MAP = {
        .pregenerated_normals = true,
        .reflective = false,
        .roughness = 0.8,
        .ambient_intensity = 0.2,
        .reflectivity = 0,
        .opacity = 1
    };

    light lm1 = {
        .position = {octree.n/2, octree.n/2, octree.n/2},
        .color = {112,229,252},
        .radius = 0.12,
        .area = octree.n
    };

    light lm2 = {
        .position = {octree.n/2, 0, octree.n/2},
        .color = {255,255,255},
        .radius = 0.15,
        .area = octree.n
    };

    voxel_data.AddChannel(VOX1);
    voxel_data.AddChannel(VOX2);
    voxel_data.AddChannel(MAP);
    voxel_data.AddLight(lm1);
    voxel_data.AddLight(lm2);

    Object map = Object(Vector(0,0,2), Vector(octree.n,octree.n,1), {200,150,100}, octree, MAP.type);
    map.LoadObj("./assets/cube.obj", false, false);
    
    Object obj1 = Object(Vector(0,0,0), Vector(octree.n/2,octree.n/2,octree.n/2), {200,150,0}, octree, VOX1.type);
    obj1.LoadObj(TEAPOT_FILE, false, false);

    /*Object obj2 = Object(Vector(octree.n/3,octree.n/2,40), Vector(octree.n/4,octree.n/4,octree.n/4), {0,150,200}, octree, VOX2.type);
    obj2.LoadObj("./assets/sphere.obj", false, false);

    Object obj3 = Object(Vector(0,-octree.n/2,0), Vector(octree.n/2,octree.n/2,octree.n/2), {60,60,255}, octree, VOX1.type);
    obj3.LoadObj(TEAPOT_FILE, false, false);*/

    //obj2.Remove();

    octree.upToDate = false;

    console.ShowMs();

    physics_ready = true;

    double start_time, end_time;

    while(SDL_AtomicGet(&running)){
        start_time = SDL_GetTicks();

        /*if(octree.upToDate == true){
            if(obj1.Position.z+obj1.Size.z < octree.n-1){
                obj1.SetPosition({(uint)obj1.Position.x,(uint)obj1.Position.y,(uint)obj1.Position.z+1});
                octree.upToDate = false;
            }

        }*/


        end_time = SDL_GetTicks();
        console.TMS = end_time-start_time;
        console.TPS = 1000/(end_time-start_time);
    }

    return 0;
}
