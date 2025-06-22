#pragma once
#include <random>

struct FVector;
namespace RandomUtils
{
    inline std::mt19937& GetRandomEngine()
    {
        static std::mt19937 engine(std::random_device{}());
        return engine;
    }

    inline int GetRandomInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(GetRandomEngine());
    }

    inline uint8 GetRandomUint8(uint8 min, uint8 max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return static_cast<uint8>(dist(GetRandomEngine()));
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

    inline bool GetRandomBool()
    {
        std::uniform_int_distribution<int> dist(0, 1);
        return dist(GetRandomEngine()) == 1;
    }

    inline FVector GetRandomOffset(float minDist = 100.f, float maxDist = 200.f, float z = 20.f)
    {
        const float PI = 3.14159265358979323846f;
        float angle = GetRandomFloat(0.f, 2 * PI);
        float dist = GetRandomFloat(minDist, maxDist);
        float x = std::cos(angle) * dist;
        float y = std::sin(angle) * dist;
        return FVector(x, y, z);
    }
}
