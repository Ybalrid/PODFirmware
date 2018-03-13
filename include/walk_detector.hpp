#pragma once

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
    };

    template <size_t buffer_size>
        class analyser 
        {
            std::array<raw_data, buffer_size> buffer;
            walk_speed_vector estimation;
            size_t initial;
            void compute()
            {

                auto& latest = buffer[0];
                auto angle = atan2(latest.x, latest.y);

                auto squaredLenght = (latest.x * latest.x) + (latest.y * latest.y);

                //TODO calculate frequency of latest oscilations
                
                //if lengh > thresold (platform tilted to some angle), and freq > fthreshold
                //  calculate vector from polat to cartesian
                //  set estimation to that vector
                //else
                //  vector is null
                //

                //in the mean time, to test communication, set vector to [1, 2]
                estimation.x = 1;
                estimation.y = 2;

            }

            public:

            analyser()
            {
                initial = 0;
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
