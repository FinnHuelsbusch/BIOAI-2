#pragma once
#include "structures.h"

auto fullReplacement(const Population& parents, const Population& children, const FunctionParameters& parameters) -> Population;
auto rouletteWheelReplacement(const Population& parents, const Population& children, const FunctionParameters& parameters) -> Population;
