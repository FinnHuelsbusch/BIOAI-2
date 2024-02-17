#pragma once
#include <gmock/gmock.h>
#include "RandomGenerator.h"

class MockRandomGenerator : public RandomGenerator
{

public:
    MOCK_METHOD(void, setSeed, (unsigned int seed));
    MOCK_METHOD(int, generateRandomInt, (int min, int max));
    MOCK_METHOD(double, generateRandomDouble, (double min, double max));

    // Mock the shuffle method
    MOCK_METHOD(void, shuffle, (std::vector<int> & elements));
    // Make the constructor public
    MockRandomGenerator() : RandomGenerator() {}; 
};