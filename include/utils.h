#pragma once
#include <vector>
#include "structures.h"

// Custom comparator for sorting the population
auto compareByFitness(const Individual &individualA, const Individual &individualB) -> bool;

// Function to sort the population based on fitness in ascending or descending order
auto sort_population(Population population, bool ascending) -> Population;

// Function to print the genome
void print_genome(Genome genome);

// Function to flatten the genome
auto flatten_genome(Genome genome) -> std::vector<int>;

// Function to unflatten the genome
auto unflatten_genome(std::vector<int> flat_genome, Genome parent) -> Genome;