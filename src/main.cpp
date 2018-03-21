#include <chrono>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <iomanip>
#include <limits>
#include "sensor.hpp"
#include "filter.hpp"
#include "packet_sender.hpp"
#include "walk_detector.hpp"
#include <vector>
#include <string>

///structure to contains the data outputed by the sensors
/*struct sample
{
    long long time;
    int x, y;
    float accx, accy, accz;
};*/

using sample = walk_detector::raw_data;


///Function to print that data to an output stream
std::ostream& operator<<(std::ostream& out, const sample& s)
{
    out << s.time << ", " << s.x << ", " << s.y << ", "
        << std::setprecision(std::numeric_limits<float>::digits10+1)
        << s.accx << ", " 
        << std::setprecision(std::numeric_limits<float>::digits10+1)
        << s.accy << ", " 
        << std::setprecision(std::numeric_limits<float>::digits10+1)
        << s.accz;
    return out;
}

int main(int argc, char* argv[])
{
    //Template parameter is the time depth of the analyser (in samples)
    //22 samples is roughly 1/4th a second if sampling is done at 11 ms. (as it is more or less the case here)
    walk_detector::analyser<22*4> wd;
    
    //Network config
    packet_sender network_sender;
    {
        std::string server = argc > 1 ? argv[1] : "255.255.255.255";
        network_sender.setServerAddress(server);
    }


    //Where to store some samples
    std::vector<sample> samples;
    samples.reserve(4096); //This already assign memory into the vector(making 4096 later's push_back free of memory alloc)

    //Initialize the graphical debug with SDL
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
    auto accZero = acc;

    srand(time(NULL));


    auto start = std::chrono::system_clock::now();
    
    filter<int, 6> xFilter, yFilter;
    filter<float, 3> xAccFilter, zAccFilter;
    
    //Set to true the first time you hit spacebar
    bool calibrated = false;
    while(running)
    {
        //Get data from the sensor array
        sensors.updateAll();
        //std::cout << sensors.getAccelerationReadout() << '\n';
        
        //Calculate timing
        last = now;
        now = std::chrono::system_clock::now();
        auto timepoint = now - start;


        std::cout << "time difference " << std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count() << " ms\n";
        distance = sensors.getDistanceReadout();
        acc = sensors.getAccelerationReadout();

        //For every event in the quue
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                //Window closed:
                case SDL_QUIT: running = false; break;
                //Any keypress
                case SDL_KEYUP:
                               {
                                   switch(e.key.keysym.sym)
                                   {
                                       //spacebar
                                       case SDLK_SPACE:
                                           zero = sensors.getDistanceReadout();
                                           accZero = acc;
                                           calibrated = true;
                                           break;
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
        distance.x = xFilter(distance.x);
        distance.y = yFilter(distance.y);

        acc.x -= accZero.x;
        //acc.y -= accZero.y;
        acc.z -= accZero.z;

        acc.x = xAccFilter(acc.x);
        acc.z = zAccFilter(acc.z);

        //std::cout << "DISTANCE " << distance << "\n";
        //Theses scalings are done for graphics only
        destination.x = (1024/2) + scaler * distance.x;
        destination.y = (768/2)  + scaler * distance.y;
        acceleration.x = (1024/2) + 500 * scaler* acc.x;
        acceleration.y = (768/2) + 500 * scaler * acc.y;

        //Draw stuff on the screen
        SDL_RenderCopy(renderer, Xtexture, nullptr, &destination);
        SDL_RenderCopy(renderer, Atexture, nullptr, &acceleration);
        //Render the texture to the screen
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        //If the platform isn't calibrated, the measured data is useless
        if(!calibrated) continue;
   
        //Construct a sample from raw_data
        sample s = {
                (long long)std::chrono::duration_cast<std::chrono::nanoseconds>
                (timepoint).count(),
                distance.x,
                distance.y,
                acc.x, acc.y, acc.z};

        //Record it for latter analysis
        samples.push_back(s);

        //Push that sample into the "walk detector"
        wd.push_data(s);
        auto walk = wd.get_estimated_walk();
        network_sender.sendWalkVector(s.time, walk.x, walk.y);
    }

    //generate (bad) random filename
    std::string filename;
    for(int i = 0; i < 20; i++)
    {
        filename.push_back((char)(97 + (rand() % 26)));
    }

    filename += ".csv";

    {
        //output to file
        auto file = std::ofstream(filename);

        for(auto& s : samples)
        {
            file << s;
            file << '\n';
        }
    }

    TTF_CloseFont(font); 
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(screenTexture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

}
