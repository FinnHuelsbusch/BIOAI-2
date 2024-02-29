#pragma once
#include "structures.h"

auto fullReplacement(const Population& parents, const Population& children, const FunctionParameters& parameters, const int population_size) -> Population;
auto rouletteWheelReplacement(const Population& parents, const Population& children, const FunctionParameters& parameters, const int population_size) -> Population;
auto elitismWithFill(const Population& parents, const Population& children, const FunctionParameters& parameters, const int population_size) -> Population;
