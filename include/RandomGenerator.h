#pragma once
#include <random>
#include <vector>
#include <algorithm>

class RandomGenerator
{
private:
    std::mt19937_64 generator; // Mersenne Twister 64-bit engine
    bool isSeeded;
    // Static instance variable
    static RandomGenerator* instance;

protected:
    RandomGenerator();

public:
    static RandomGenerator &getInstance();

    void setSeed(unsigned int seed);

    int generateRandomInt(int min, int max);

    double generateRandomDouble(double min, double max);

    template <typename T>
    void shuffle(std::vector<T> &elements)
    {
        if (!isSeeded)
        {
            std::random_device rd;
            setSeed(rd());
        }

        std::shuffle(elements.begin(), elements.end(), generator);
    }
    #ifdef TESTING_MODE
    static void setInstance(RandomGenerator *instance);
    static void resetInstance();
    #endif
};

