#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"

#include <bits/stdc++.h>
#include <strstream>

#include "Cl_utils.h"
#include "./Input.h"
#include "config.h"
#include "octree.h"
#include "channels.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <sys/stat.h>

#define PROGRAM_FILE "./src/cl/pixel.cl"
#define FONT_FILE "./assets/font.ttf"
#define KERNEL_FUNC "compute_pixels_kernel"

config_ config {
    .SCREEN_HEIGHT = 600,
    .SCREEN_WIDTH = 800,
    .res_y = 600,
    .res_x = 800,
    .FOV = 90
};

bool physics_ready = false;

SDL_atomic_t running;

Octree octree;
Debug console;
CameraHandler Camera;

properties voxel_data;


static int RenderEngine(void *data){

    while (!physics_ready){SDL_Delay(100);}


    double start_time = SDL_GetTicks();

    SDL_Window *window;
    SDL_Renderer *renderer;
    bool isFullScreen = false;
    window = SDL_CreateWindow("SDL Render Window", 200, 200, config.SCREEN_WIDTH, config.SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_ShowWindow(window);

    std::cout<<"Initializing OpenCL data"<<std::endl;

    
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int err,n_mem=octree.n,l_mem=voxel_data.index_lights;
    size_t global_size[3] = {config.res_x, config.res_y, 0};

    cl_mem ddata, doutput, dproperties, dlights;

    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

    program = build_program(context, device, PROGRAM_FILE);
    queue = clCreateCommandQueue(context, device, 0, &err);

    dproperties = clCreateBuffer(context, CL_MEM_READ_ONLY, (size_t)(voxel_data.index_materials)*sizeof(cl_int8), NULL, &err); CheckErr(err);
    dlights = clCreateBuffer(context, CL_MEM_READ_ONLY, (size_t)(voxel_data.index_lights)*sizeof(cl_float8), NULL, &err); CheckErr(err);
    doutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, config.res_y*config.res_x*sizeof(cl_uchar3), NULL, &err); CheckErr(err);

    err = clEnqueueWriteBuffer(queue, dproperties, CL_TRUE, 0,(size_t)(voxel_data.index_materials)*sizeof(cl_int8), voxel_data.data, 0, NULL, NULL); CheckErr(err);
    err = clEnqueueWriteBuffer(queue, dlights, CL_TRUE, 0,(size_t)(voxel_data.index_lights)*sizeof(cl_float8), voxel_data.lights, 0, NULL, NULL); CheckErr(err);
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);

    cl_int2 res = {config.res_x,config.res_y};
    cl_float3 light_cl = Camera.light.CL();

    cl_int4 data_config = {n_mem, config.FOV, config.depth, l_mem};

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&doutput); CheckErr(err);// <=====OUTPUT
    err = clSetKernelArg(kernel, 2, sizeof(cl_int4), (void *)&data_config); CheckErr(err);
    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&dproperties); CheckErr(err);
    err = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&dlights); CheckErr(err);

    TTF_Init();
    TTF_Font * font = TTF_OpenFont(FONT_FILE, 50);

    double end_time = SDL_GetTicks();
    std::cout<<"Initialized all data in "<<end_time-start_time<<" ms"<<std::endl;

    cl_uchar3* houtput;
    SDL_Texture* SCREEN = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, config.res_x, config.res_y);

    while (SDL_AtomicGet(&running))
    {

        Camera.StartRenderFrame();
        start_time = SDL_GetTicks();

        if(Camera.FULLSCREEN){
            isFullScreen = !isFullScreen;
            SDL_SetWindowFullscreen(window, isFullScreen);
            SDL_SetRelativeMouseMode((SDL_bool)isFullScreen);
            Camera.FULLSCREEN = false;

            SDL_DisplayMode DM;
            SDL_GetCurrentDisplayMode(0, &DM);
            config.SCREEN_WIDTH = DM.w;
            config.SCREEN_HEIGHT = DM.h;
        }

        cl_float3 pos = Camera.Camera.pos.CL();
        cl_float3 vec = Camera.Camera.vector.CL();

        err = clSetKernelArg(kernel, 3, sizeof(cl_float3), (void *)&pos); CheckErr(err);
        err = clSetKernelArg(kernel, 4, sizeof(cl_float3), (void *)&vec); CheckErr(err);

        if(!octree.upToDate){
            ddata = clCreateBuffer(context, CL_MEM_READ_ONLY, (size_t)(OCTREE_INDEX)*sizeof(cl_uint4), NULL, &err); CheckErr(err);
            err = clEnqueueWriteBuffer(queue, ddata, CL_TRUE, 0,(size_t)(OCTREE_INDEX)*sizeof(cl_uint4), octree.octree, 0, NULL, NULL); CheckErr(err);
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&ddata); CheckErr(err);

            octree.upToDate = true;
        }


        //rendering
        int pitch = 0;
        SDL_LockTexture(SCREEN, NULL, reinterpret_cast<void**>(&houtput), &pitch);

        err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL); CheckErr(err);
        err = clFinish(queue); CheckErr(err);
        err = clEnqueueReadBuffer(queue, doutput, CL_TRUE, 0, config.res_x*config.res_y*sizeof(cl_uchar3), houtput, 0, NULL, NULL); CheckErr(err);

        SDL_UnlockTexture(SCREEN);
        SDL_RenderCopy(renderer, SCREEN, NULL, NULL);
        //----------


        end_time = SDL_GetTicks();


        console.FMS = (end_time-start_time);
        console.FPS = 1000/(end_time-start_time);


        console.add_data("FMS", console.FMS, font, renderer);
        console.add_data("TMS", console.TMS, font, renderer);
        console.add_data("FPS", console.FPS, font, renderer);
        console.add_data("TPS", console.TPS, font, renderer);
        console.add_data("OCTREE_LENGTH", OCTREE_INDEX, font, renderer);
        console.add_data("OCTREE_upToDate", octree.upToDate, font, renderer);
        console.update();

        SDL_RenderPresent(renderer);

        Camera.EndRenderFrame();
    }

    clReleaseKernel(kernel);
    clReleaseMemObject(ddata);
    clReleaseMemObject(doutput);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    TTF_CloseFont(font);
    TTF_Quit();

    return 0;
}