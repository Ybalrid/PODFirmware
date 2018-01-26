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

    TTF_Init(); //TODO check error;


    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    auto screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 1024,768);
    SDL_SetRenderDrawColor(renderer, 255,255,255,255); //fill white

    auto font = TTF_OpenFont("DejaVuSansMono.ttf", 12);
    SDL_Color blackColor{0,0,0,255};
    SDL_Color redColor{255,0,0,255};
    auto Xsurface = TTF_RenderText_Solid(font, "+", blackColor);
    auto Xtexture = SDL_CreateTextureFromSurface(renderer, Xsurface);
    auto Asurface = TTF_RenderText_Solid(font, "+", redColor);
    auto Atexture = SDL_CreateTextureFromSurface(renderer, Asurface);
    sensorArray sensors;
    
    auto last = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();

    auto running(true);
    SDL_Event e;

    SDL_Rect destination
    {
        200,200,50,50
    };

    SDL_Rect acceleration = destination;
    
    auto distance = sensors.getDistanceReadout();
    auto zero = distance;

    int scaler = 10;

    auto acc = sensors.getAccelerationReadout();

    while(running)
    {
        sensors.updateAll();
        std::cout << sensors.getAccelerationReadout() << '\n';

        last = now;
        now = std::chrono::system_clock::now();

        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count() << " ms\n";
        distance = sensors.getDistanceReadout();
        acc = sensors.getAccelerationReadout();
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT: running = false; break;
                case SDL_KEYUP:
                {
                    switch(e.key.keysym.sym)
                    {
                        //poor man's calibration
                        case SDLK_SPACE:
                            zero = sensors.getDistanceReadout();
                        default:break;
                    }
                }
                default:break;
            }
        }
        //Render to the texture
        SDL_SetRenderTarget(renderer, screenTexture);
        SDL_RenderClear(renderer); //Clear
        
        distance.x -= zero.x;
        distance.y -= zero.y;
        std::cout << "DISTANCE " << distance << "\n";


        destination.x = (1024/2) + scaler * distance.x;
        destination.y = (768/2)  + scaler * distance.y;
        acceleration.x = (1024/2) + 500 * scaler* acc.x;
        acceleration.y = (768/2) + 500 * scaler * acc.y;
        //Draw stuff here!!!
        SDL_RenderCopy(renderer, Xtexture, nullptr, &destination);
        SDL_RenderCopy(renderer, Atexture, nullptr, &acceleration);
        //Render the texture to the screen
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
    TTF_CloseFont(font); 
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(screenTexture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

}
