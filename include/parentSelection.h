#pragma once
#include "structures.h"


auto no_selection(const Population& population, const function_parameters& parameters) -> Population;
auto roulette_wheel_selection(const Population& population, const function_parameters& parameters) -> Population;
auto tournament_selection(const Population& population, const function_parameters& parameters) -> Population;

