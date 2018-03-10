#include <array>
#include <algorithm>

template <typename T, typename S> T lerp(T a, T b, S w)
{
    return a + w * (b - a);
}

template <typename T, size_t N>
struct filter
{
    std::array<T, N> buffer;
    size_t dim;
    T discardDistanceA, discardDistanceB;
    float scale = 0.9f;
    T accumulator;
    filter()
    {
        dim = buffer.size();
        discardDistanceA = 5;
        discardDistanceB = 10;

        std::generate(buffer.begin(), buffer.end(), []{return 0;});
    }

    T operator()(T input)
    {
        std::rotate(buffer.rbegin(), buffer.rbegin()+1, buffer.rend());

        buffer[0] = input;

        if(std::abs(buffer[0] - buffer[1]) > discardDistanceA &&
                std::abs(buffer[0]) > discardDistanceB)
        {
            //This is probably a reading glitch. Smooth that out.
            buffer[0] = lerp(buffer[0], buffer[2], scale);
        }

    accumulator = 0;
    for(size_t i{0}; i < dim; ++i) 
        accumulator += buffer[i];

    accumulator /= dim;
    return accumulator;
    }
};
