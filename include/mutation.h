#pragma once

#include "structures.h"


auto reassignOnePatient(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto swapWithinJourney(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto swapBetweenJourneys(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto insertWithinJourney(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto twoOpt(Genome& genome, const FunctionParameters& parameters) -> Genome;