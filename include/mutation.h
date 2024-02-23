#pragma once

#include "structures.h"


auto reassignOnePatient(Genome& genome, const function_parameters& parameters) -> Genome;
auto swapWithinJourney(Genome& genome, const function_parameters& parameters) -> Genome;
auto swapBetweenJourneys(Genome& genome, const function_parameters& parameters) -> Genome;
auto insertWithinJourney(Genome& genome, const function_parameters& parameters) -> Genome;
auto twoOpt(Genome& genome, const function_parameters& parameters) -> Genome;