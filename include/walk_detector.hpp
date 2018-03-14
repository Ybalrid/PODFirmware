#pragma once

#include <iostream>
#include <array>
#include <algorithm>
#include <cmath>

namespace walk_detector
{

    struct raw_data
    {
        raw_data() = default;
        raw_data(long long t, int nx, int ny, float ax, float ay, float az) : 
            time(t), x(nx), y(ny), accx(ax), accy(ay), accz(az)
        {}
        long long time;
        int x, y;
        float accx, accy, accz;
    };

    struct walk_speed_vector
    {
        float x, y;

        float squaredLenght()
        {
            return x*x+y*y;
        }

        float length()
        {
            return sqrt(squaredLenght());
        }

        void normalize()
        {
            auto l = length();
            if(l > 0)
            {
                x /= l;
                y /= l;
            }
        }

        walk_speed_vector operator*(float s)
        {
            auto newVect = *this;
            newVect.x *= s;
            newVect.y *= s;
            return newVect;
        }

        walk_speed_vector operator*=(float s)
        {
            *this = *this * s;
            return *this;
        }
    };

    template <size_t buffer_size>
        class analyser 
        {
            std::array<raw_data, buffer_size> buffer;
            walk_speed_vector estimation;
            size_t initial;
            
            float detectionThreshold;

            void compute()
            {

                auto& latest = buffer[0];
                auto angle = atan2(latest.x, latest.y);

                walk_speed_vector vect;
                vect.x = latest.x;
                vect.y = latest.y;

                float lsquared = vect.squaredLenght();
                std::cerr << "squared lenght of vector = " << lsquared << '\n';
                //TODO calculate frequency of latest oscilations
                
                //if lengh > thresold (platform tilted to some angle), and freq > fthreshold
                //  calculate vector from polat to cartesian
                //  set estimation to that vector
                //else
                //  vector is null
                //


                if(lsquared > detectionThreshold)
                {
                    std::cerr << "PLATFORM TILTED!!\n";

                    const auto s = sin(angle);
                    const auto c = cos(angle);

                    static const float nx = 1, ny = 0;
                    estimation.x = nx * c - ny * s;
                    estimation.y = nx * s + ny * c;

                    //TODO calculate some actual speed!
                    
                    static const float some_speed = 1.23;
                    estimation *= some_speed;

                }
                else
                {
                    estimation.x = 0.F;
                    estimation.y = 0.F;
                }

            }

            public:

            analyser()
            {
                initial = 0;
                detectionThreshold = 100;
            }

            walk_speed_vector get_estimated_walk()
            {
                if(initial < buffer.size())
                    return {0.f, 0.f};
                compute();
                return estimation;
            }
            void push_data(const raw_data& data_point)
            {
                initial++;
                std::rotate(buffer.rbegin(), buffer.rbegin()+1, buffer.rend());
                buffer[0] = data_point;
            }
        };

}
