#pragma once
#include "structures.h"


auto noSelection(const Population& population, const FunctionParameters& parameters) -> Population;
auto rouletteWheelSelection(const Population& population, const FunctionParameters& parameters) -> Population;
auto tournamentSelection(const Population& population, const FunctionParameters& parameters) -> Population;

