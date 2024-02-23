#pragma once
#include "structures.h"

auto full_replacement(const Population& parents, const Population& children, const function_parameters& parameters) -> Population;
auto roulette_wheel_replacement(const Population& parents, const Population& children, const function_parameters& parameters) -> Population;
