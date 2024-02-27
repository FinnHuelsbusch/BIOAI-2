#pragma once
#include <vector>
#include "structures.h"

// Function to calculate the total travel time of a genome
auto getTotalTravelTime(const Genome &genome, const ProblemInstance &problemInstance) -> double;

// Function to sort the population based on fitness in ascending or descending order
auto sortPopulationByFitness(Population& population, bool ascending) -> void;

// Function to sort the population based on travel time in ascending or descending order
auto sortPopulationByTravelTime(Population population, bool ascending, const ProblemInstance &problemInstance) -> void;

// Function to print the genome
void printGenome(const Genome& parent);

// Function to flatten the genome

auto flattenGenome(const Genome& parent) -> std::vector<int>;

// Function to unflatten the genome
auto unflattenGenome(std::vector<int> flatGenome, const Genome& parent) -> Genome;

// Export individual to json given a Individual and a path
auto exportIndividual(const Individual& individual, const std::string& path) -> void;