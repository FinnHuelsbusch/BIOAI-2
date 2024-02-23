#include "structures.h"
#include "survivorSelection.h"
#include <parentSelection.h>

auto fullReplacement(const Population &parents, const Population &children, const FunctionParameters &parameters) -> Population
{
    return children;
}

auto rouletteWheelReplacement(const Population &parents, const Population &children, const FunctionParameters &parameters) -> Population
{
    Population combinedPopulation = parents;
    combinedPopulation.insert(combinedPopulation.end(), children.begin(), children.end());
    Population survivors = rouletteWheelSelection(combinedPopulation, parameters);
    // limit survivors to the population size
    survivors.resize(parents.size());
    return survivors;
}