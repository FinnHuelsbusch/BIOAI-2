#pragma once

#include "structures.h"


Genome reassignOnePatient(Genome& genome, const function_parameters& parameters);
Genome swapWithinJourney(Genome& genome, const function_parameters& parameters);
Genome swapBetweenJourneys(Genome& genome, const function_parameters& parameters);
Genome insertWithinJourney(Genome& genome, const function_parameters& parameters);
Genome twoOpt(Genome& genome, const function_parameters& parameters);