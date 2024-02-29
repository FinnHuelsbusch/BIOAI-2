#include "structures.h"
#include "survivorSelection.h"
#include <parentSelection.h>
#include <stdexcept>
#include <cassert> 
#include "utils.h"
#include <cmath>

auto fullReplacement(const Population &parents, const Population &children, const FunctionParameters &parameters, const  int populationSize) -> Population
{   
    if (populationSize != children.size())
    {
        throw std::invalid_argument("Population size must be equal to the number of children in order to use full replacement");
    }
    return children;
}



auto rouletteWheelReplacement(const Population &parents, const Population &children, const FunctionParameters &parameters, const  int populationSize) -> Population
{
    Population combinedPopulation = parents;
    combinedPopulation.insert(combinedPopulation.end(), children.begin(), children.end());
    Population survivors = rouletteWheelSelection(combinedPopulation, parameters, populationSize);
    assert(survivors.size() == populationSize);
    return survivors;
}

auto elitismWithFill(const Population &parents, const Population &children, const FunctionParameters &parameters, const  int populationSize) -> Population
{
    double percentage = std::get<double>(parameters.at("elitism_percentage"));
    std::string fillFunctionString = std::get<std::string>(parameters.at("fillFunction"));
    SurvivorSelectionFunction fillFunction;
    if (fillFunctionString == "rouletteWheel")
    {
        fillFunction = rouletteWheelReplacement;
    }
    else
    {
        throw std::runtime_error("Fill function not implemented");
    }

    int numberOfElites = std::floor(percentage * populationSize);   
    Population survivors;
    survivors.reserve(parents.size());

    Population combinedPopulation = parents;
    combinedPopulation.insert(combinedPopulation.end(), children.begin(), children.end());
    sortPopulationByFitness(combinedPopulation, false);
    for (int i = 0; i < numberOfElites; i++)
    {
        survivors.push_back(combinedPopulation[i]);
    }
    Population fillPopulation = fillFunction(parents, children, {}, populationSize - numberOfElites);
    survivors.insert(survivors.end(), fillPopulation.begin(), fillPopulation.end());
    assert(survivors.size() == populationSize);
    return survivors;
}
