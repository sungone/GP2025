#pragma once
#include <random>

namespace RandomUtils
{
    inline std::mt19937& GetRandomEngine()
    {
        static std::mt19937 dre(std::random_device{}());
        return dre;
    }

    inline int GetRandomInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(GetRandomEngine());
    }

    inline float GetRandomFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(GetRandomEngine());
    }

    inline bool Chance(float probability)
    {
        return GetRandomFloat(0.0f, 1.0f) < probability;
    }
}
