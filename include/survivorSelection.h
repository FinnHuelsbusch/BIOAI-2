#pragma once
#include "structures.h"
#include <functional>

auto fullReplacement(const Population& parents, const Population& children, const FunctionParameters& parameters, const int population_size) -> Population;
auto rouletteWheelReplacement(const Population& parents, const Population& children, const FunctionParameters& parameters, const int population_size) -> Population;
auto elitismWithFill(const Population& parents, const Population& children, const FunctionParameters& parameters, const int population_size) -> Population;

// static final map of survivor selection functions
static const std::map<std::string, SurvivorSelectionFunction> survivorSelectionFunctions = {
    {"fullReplacement", fullReplacement},
    {"rouletteWheelReplacement", rouletteWheelReplacement},
    {"elitismWithFill", elitismWithFill}
};