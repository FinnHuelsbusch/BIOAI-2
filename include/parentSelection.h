#pragma once
#include "structures.h"


Population no_selection(const Population& population, const function_parameters& parameters);
Population roulette_wheel_selection(const Population& population, const function_parameters& parameters);
Population tournament_selection(const Population& population, const function_parameters& parameters);

