#pragma once
#include "SDL2/SDL.h"
#include <bits/stdc++.h>
#include "debug.h"
#include "math.h"
#include "vector.hpp"

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

class CameraHandler{
    private:
        struct cam{
            Vector pos = Vector(-5,-5,-5);
            cl_float2 rotation = {0,0};
            Vector vector = Vector(1,0,0);
        };

        bool FIRST_FRAME_END = false;
        int START_ANGLEX;
        int START_ANGLEY;

        bool KEY_W = false;
        bool KEY_A = false;
        bool KEY_S = false;
        bool KEY_D = false;
        bool KEY_SPACE = false;
        bool KEY_SHIFT = false;
        bool KEY_F = false;


        float start_time;
        float end_time;

        float FixX(float ang)
        {
            if (ang > 359)
            {
                float mult = floor(ang / 360);
                ang = ang - (360 * mult);
            }
            if (ang < 0)
            {
                float ap = abs(ang);
                float mult = ceil(ap / 360);
                ang = (360 * mult) - ap;
            }
            return ang;
        }
        float Limit_90(float ang){
        if(ang<-45)return -45;
        if(ang>45)return 45;
        return ang;}
        float FixY(float ang){
            if (ang > 89)
            {
                ang = 89;
            }
            if (ang < -89)
            {
                ang = -89;
            }
            return ang;
        }


    public:
        const float PLAYER_SPEED_CONST= 0.00003;
        float PLAYER_SPEED = 0.000003;
        float sensitivity = 0.7;
        bool FULLSCREEN = false;
        Vector light;
        cam Camera;
        SDL_Event e;
        

        void Frame(SDL_Event event, Debug console);
        void GetPosition();
        void KeyDown();
        void KeyUp();
        void MouseMotion();

        void StartRenderFrame();
        void EndRenderFrame();
};





void CameraHandler::Frame(SDL_Event event, Debug console){
    e = event;

    GetPosition();

    switch (e.type)
    {
        case SDL_KEYDOWN:
            KeyDown();
            break;
        case SDL_KEYUP:
            KeyUp();
            break;
        case SDL_MOUSEMOTION:
            MouseMotion();
            break;
    }
}

void CameraHandler::GetPosition(){
    Vector flat_vector = Vector(Camera.vector.x, Camera.vector.y, 0);
    flat_vector.normalization();

    Vector Cross = flat_vector.cross_product(Vector(0,0,1));
    Cross.normalization();
    

    if(KEY_W){
        Camera.pos += flat_vector * PLAYER_SPEED;
    }
    if(KEY_A){
        Camera.pos += Cross * PLAYER_SPEED;
    }
    if(KEY_S){
        Camera.pos -= flat_vector * PLAYER_SPEED;
    }
    if(KEY_D){
        Camera.pos -= Cross * PLAYER_SPEED;
    }
    if(KEY_SPACE){
        Camera.pos.z += PLAYER_SPEED;
    }
    if(KEY_SHIFT){
        Camera.pos.z -= PLAYER_SPEED;
    }
}

void CameraHandler::KeyDown(){
    if(e.key.keysym.sym == SDLK_w) KEY_W = true;
    if(e.key.keysym.sym == SDLK_a) KEY_A = true;
    if(e.key.keysym.sym == SDLK_s) KEY_S = true;
    if(e.key.keysym.sym == SDLK_d) KEY_D = true;
    if(e.key.keysym.sym == SDLK_d) KEY_D = true;
    if(e.key.keysym.sym == 32) KEY_SPACE = true;
    if(e.key.keysym.sym == 1073742049) KEY_SHIFT = true;
    if(e.key.keysym.sym == SDLK_f && !KEY_F) {KEY_F = true; FULLSCREEN = true;}
    if(e.key.keysym.sym == SDLK_p) {light = Camera.pos;}
}

void CameraHandler::KeyUp(){
    if(e.key.keysym.sym == SDLK_w) KEY_W = false;
    if(e.key.keysym.sym == SDLK_a) KEY_A = false;
    if(e.key.keysym.sym == SDLK_s) KEY_S = false;
    if(e.key.keysym.sym == SDLK_d) KEY_D = false;
    if(e.key.keysym.sym == SDLK_d) KEY_D = false;
    if(e.key.keysym.sym == 32) KEY_SPACE = false;
    if(e.key.keysym.sym == 1073742049) KEY_SHIFT = false;
    if(e.key.keysym.sym == SDLK_f && KEY_F) KEY_F = false;
}

void CameraHandler::MouseMotion(){
        if(FIRST_FRAME_END == false){
            FIRST_FRAME_END = true;
            START_ANGLEX = e.motion.x;
            START_ANGLEY = e.motion.y;

            Camera.rotation.x = 0;
            Camera.rotation.y = 0;
        }else{
            Camera.rotation.x = FixX(Camera.rotation.x + (e.motion.x - START_ANGLEX) * sensitivity);
            Camera.rotation.y = FixY(Camera.rotation.y - (e.motion.y - START_ANGLEY) * sensitivity);

            Camera.vector.EulerAngles(Camera.rotation);

            START_ANGLEX = e.motion.x;
            START_ANGLEY = e.motion.y;
        }
}

void CameraHandler::StartRenderFrame(){
    start_time = SDL_GetTicks();
}

void CameraHandler::EndRenderFrame(){
    PLAYER_SPEED = ((float)SDL_GetTicks() - start_time)*PLAYER_SPEED_CONST;
}
