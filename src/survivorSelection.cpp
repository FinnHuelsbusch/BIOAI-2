#include "structures.h"
#include "survivorSelection.h"
#include <parentSelection.h>

auto full_replacement(const Population &parents, const Population &children, const function_parameters &parameters) -> Population
{
    return children;
}

auto roulette_wheel_replacement(const Population &parents, const Population &children, const function_parameters &parameters) -> Population
{
    Population combined_population = parents;
    combined_population.insert(combined_population.end(), children.begin(), children.end());
    Population survivors = roulette_wheel_selection(combined_population, parameters);
    // limit survivors to the population size
    survivors.resize(parents.size());
    return survivors;
}