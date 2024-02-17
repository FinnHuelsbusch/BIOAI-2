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

    virtual int generateRandomInt(int min, int max);

    virtual double generateRandomDouble(double min, double max);

    virtual void shuffle(std::vector<int> &elements); 
    
    #ifdef TESTING_MODE
    static void setInstance(RandomGenerator *instance);
    static void resetInstance();
    #endif
};

