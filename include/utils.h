#pragma once
#include <vector>
#include "structures.h"

// Custom comparator for sorting the population
auto compareByFitness(const Individual &individualA, const Individual &individualB) -> bool;

// Function to sort the population based on fitness in ascending or descending order
auto sortPopulation(Population population, bool ascending) -> Population;

// Function to print the genome
void printGenome(const Genome& parent);

// Function to flatten the genome

auto flattenGenome(const Genome& parent) -> std::vector<int>;

// Function to unflatten the genome
auto unflattenGenome(std::vector<int> flatGenome, const Genome& parent) -> Genome;