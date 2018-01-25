#include <chrono>
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

#include "sensor.hpp"


int main()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << SDL_GetError() << '\n';
        return -1;
    }

    auto window = SDL_CreateWindow("POD Test", 100, 100, 1024, 768, SDL_WINDOW_SHOWN);
    if(!window)
    {

        std::cout << SDL_GetError() << '\n';
        SDL_Quit();
        return -2;
    }

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    auto screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 1024,768);


    sensorArray sensors;
    
    auto last = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();

    auto running(true);
    SDL_Event e;
    while(running)
    {
        sensors.updateAll();
        std::cout << sensors.getDistanceReadout() << '\n';
        std::cout << sensors.getAccelerationReadout() << '\n';

        last = now;
        now = std::chrono::system_clock::now();

        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count() << " ms\n";

        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT: running = false; break;
                default:break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();

}
