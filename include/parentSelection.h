#pragma once
#include "structures.h"


auto noSelection(const Population& population, const FunctionParameters& parameters, const  int populationSize) -> Population;
auto rouletteWheelSelection(const Population& population, const FunctionParameters& parameters, const  int populationSize) -> Population;
auto tournamentSelection(const Population& population, const FunctionParameters& parameters, const  int populationSize) -> Population;

