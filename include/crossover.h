#pragma once
#include "structures.h"


auto order1Crossover(const Genome& parent1, const Genome& parent2) -> std::pair<Genome, std::optional<Genome>>;
auto partiallyMappedCrossover(const Genome& parent1, const Genome& parent2) -> std::pair<Genome, std::optional<Genome>> ;
auto edgeRecombination(const Genome& parent1, const Genome& parent2) -> std::pair<Genome, std::optional<Genome>>;
