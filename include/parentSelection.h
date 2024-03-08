#pragma once
#include "structures.h"
#include <functional>

auto rouletteWheelSelection(const Population& population, const FunctionParameters& parameters, const  int populationSize) -> Population;
auto tournamentSelection(const Population& population, const FunctionParameters& parameters, const  int populationSize) -> Population;

// static final map of parent selection functions
static const std::map<std::string, ParentSelectionFunction> parentSelectionFunctions = {
    {"rouletteWheelSelection", rouletteWheelSelection},
    {"tournamentSelection", tournamentSelection}
};