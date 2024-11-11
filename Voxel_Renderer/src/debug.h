#pragma once
#include "../utils/PerlinNoise.hpp"
#include "config.h"



class Debug{
    private:
        float start_time;
        int num_data = 0;

    public:
        int RenderStatus;
        int PhysicsStatus;

        int FPS;
        int TPS;

        int FMS;
        int TMS;

        void GetData3D(config_ &config){
            std::cout<<"Please insert a seed value: "; std::cin>>config.seed;
            std::cout<<"Please insert the number of noise octaves (must be between 1 and 10): "; std::cin>>config.octaves;
            std::cout<<"Please insert a density value (must be between 0 and 255): "; std::cin>>config.density;
        }

        void GetData2D(config_ &config){
            std::cout<<"Please insert a seed value: "; std::cin>>config.seed;
            std::cout<<"Please insert the number of noise octaves (must be between 1 and 10): "; std::cin>>config.octaves;
            std::cout<<"Please insert a height value (must be between 50 and 250): "; std::cin>>config.height;
        }

        void GetOctree(config_ &config){
            std::cout<<"Please insert a depth value: "; std::cin>>config.depth;
        }

        void StartTime();
        void ShowMs();
        float GetTime();

        void add_data(std::string caption, int value, TTF_Font * font, SDL_Renderer * renderer);
        void update();
};

void Debug::StartTime(){
    start_time = SDL_GetTicks();
}

void Debug::ShowMs(){
    std::cout<<(float)SDL_GetTicks()-start_time<<" ms"<<std::endl;
}

float Debug::GetTime(){
    return (float)SDL_GetTicks()-start_time;
}

void Debug::add_data(std::string caption, int value, TTF_Font * font, SDL_Renderer * renderer){
    SDL_Color color = {255, 255, 255, 255};
    std::stringstream VALUE_TEXT;
    VALUE_TEXT<<value;
    std::string str = caption + ": "+VALUE_TEXT.str();
    SDL_Rect text_rect = {0, num_data * 15, str.length() * 8, 15};
    SDL_Surface * surface = TTF_RenderText_Solid(font, str.c_str(), color);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    num_data++;
}

void Debug::update(){
    num_data = 0;
};