#pragma once
#include "structures.h"
#include <functional>


auto order1Crossover(const Genome& parent1, const Genome& parent2) -> std::pair<Genome, std::optional<Genome>>;
auto partiallyMappedCrossover(const Genome& parent1, const Genome& parent2) -> std::pair<Genome, std::optional<Genome>> ;
auto edgeRecombination(const Genome& parent1, const Genome& parent2) -> std::pair<Genome, std::optional<Genome>>;

// static final map of crossover functions 
static const std::map<std::string, CrossoverFunction> crossoverFunctions = {
    {"order1Crossover", order1Crossover},
    {"partiallyMappedCrossover", partiallyMappedCrossover},
    {"edgeRecombination", edgeRecombination}
};