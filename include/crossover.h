#pragma once
#include "structures.h"


std::pair<Genome, std::optional<Genome>> order1Crossover(const Genome& parent1, const Genome& parent2);

std::pair<Genome, std::optional<Genome>> partiallyMappedCrossover(const Genome& parent1, const Genome& parent2);

std::pair<Genome, std::optional<Genome>> edgeRecombination(const Genome& parent1, const Genome& parent2);
