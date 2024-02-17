#include "RandomGenerator.h"

#include <iostream>

RandomGenerator::RandomGenerator() : isSeeded(false) {}

RandomGenerator &RandomGenerator::getInstance()
{
    static RandomGenerator instance;
    return instance;
}

void RandomGenerator::setSeed(unsigned int seed)
{
    if (!isSeeded)
    {
        generator.seed(seed);
        isSeeded = true;
    }
    else
    {
        std::cout << "The PRNG was already seeded. Ignoring new seed" << std::endl;
    }
}

int RandomGenerator::generateRandomInt(int min, int max)
{
    if (!isSeeded)
    {
        std::random_device rd;
        setSeed(rd());
    }

    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(generator);
}

double RandomGenerator::generateRandomDouble(double min, double max)
{
    if (!isSeeded)
    {
        std::random_device rd;
        setSeed(rd());
    }

    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

template void RandomGenerator::shuffle(std::vector<int> &);
