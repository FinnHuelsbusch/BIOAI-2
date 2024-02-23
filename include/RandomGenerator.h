#pragma once
#include <random>
#include <vector>
#include <algorithm>

class RandomGenerator
{
private:
    std::mt19937_64 generator; // Mersenne Twister 64-bit engine
    bool isSeeded{false};
    // Static instance variable
    static RandomGenerator* instance;

protected:
    RandomGenerator();
    
public:
    static auto getInstance() -> RandomGenerator &;

    void setSeed(unsigned int seed);

    virtual auto generateRandomInt(int min, int max) -> int;

    virtual auto generateRandomDouble(double min, double max) -> double;

    virtual void shuffle(std::vector<int> &elements); 
    
    #ifdef TESTING_MODE
    static void setInstance(RandomGenerator *instance);
    static void resetInstance();
    #endif
};

