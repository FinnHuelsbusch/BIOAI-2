#pragma once
#include "structures.h"

Population full_replacement(const Population& parents, const Population& children, const function_parameters& parameters);
Population roulette_wheel_replacement(const Population& parents, const Population& children, const function_parameters& parameters);
