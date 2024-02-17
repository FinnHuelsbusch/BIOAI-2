#pragma once
#include <vector>
#include "structures.h"

// Custom comparator for sorting the population
bool compareByFitness(const Individual& a, const Individual& b);

// Function to sort the population based on fitness in ascending or descending order
Population sort_population(Population population, bool ascending);

// Function to print the genome
void print_genome(Genome genome);

// Function to flatten the genome
std::vector<int> flatten_genome(Genome genome);

// Function to unflatten the genome
Genome unflatten_genome(std::vector<int> flat_genome, Genome parent);