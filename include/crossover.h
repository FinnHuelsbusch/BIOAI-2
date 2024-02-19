#pragma once
#include "structures.h"


std::pair<Genome, Genome> order1Crossover(const Genome& parent1, const Genome& parent2);

std::pair<Genome, Genome> partiallyMappedCrossover(const Genome& parent1, const Genome& parent2);

Genome edgeRecombination(const Genome& parent1, const Genome& parent2);
