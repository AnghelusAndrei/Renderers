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



int main()
{
    console.GetOctree(config);
    octree.Initialize(config.depth);

    Camera.light = Vector(octree.n, octree.n, octree.n);

    SDL_AtomicSet(&running, 1);

    SDL_Thread *PhysicsThread;
    SDL_Thread *RenderingThread;
    PhysicsThread = SDL_CreateThread(PhysicsEngine, "physics_engine", NULL);
    RenderingThread = SDL_CreateThread(RenderEngine, "render_engine", NULL);

    SDL_Event e;

    while (true)
    {
        SDL_PollEvent(&e);
        
        if (e.type == SDL_QUIT)
        {
            SDL_AtomicSet(&running, 0);
            break;
        }

        Camera.Frame(e,console);
    }

    SDL_WaitThread(RenderingThread, &console.RenderStatus);
    SDL_WaitThread(PhysicsThread, &console.PhysicsStatus);

    return 0;
}