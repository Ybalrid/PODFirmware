#pragma once

#include <array>
#include <algorithm>

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
                estimation.x = 1;
                estimation.y = 2;
                //Do magic here!
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
