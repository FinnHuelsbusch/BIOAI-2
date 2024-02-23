#include "RandomGenerator.h"

#include <iostream>
RandomGenerator* RandomGenerator::instance = nullptr;

RandomGenerator::RandomGenerator() = default;

auto RandomGenerator::getInstance() -> RandomGenerator &
{
    if (instance == nullptr)
    {
        instance = new RandomGenerator();
    }
    return *instance;
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
        std::cout << "The PRNG was already seeded. Ignoring new seed" << '\n';
    }
}

auto RandomGenerator::generateRandomInt(int min, int max) -> int
{
    if (!isSeeded)
    {
        std::random_device randomDevice;
        setSeed(randomDevice());
    }

    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(generator);
}

auto RandomGenerator::generateRandomDouble(double min, double max) -> double
{
    if (!isSeeded)
    {
        std::random_device randomDevice;
        setSeed(randomDevice());
    }

    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

void RandomGenerator::shuffle(std::vector<int> &elements){
    if (!isSeeded)
    {
        std::random_device randomDevice;
        setSeed(randomDevice());
    }

    std::shuffle(elements.begin(), elements.end(), generator);
}

#ifdef TESTING_MODE
void RandomGenerator::setInstance(RandomGenerator *instance)
{
    // Set the instance to the provided instance (useful for testing)
    RandomGenerator::instance = instance;
}

void RandomGenerator::resetInstance() {
    instance = nullptr;
}
#endif
