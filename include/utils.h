#pragma once
#include <vector>
#include "structures.h"

// Function to calculate the total travel time of a genome
auto getTotalTravelTime(const Genome &genome, const ProblemInstance &problemInstance) -> double;

// Function to sort the population based on fitness in ascending or descending order
auto sortPopulationByFitness(Population& population, bool ascending) -> void;

// Function to sort the population based on travel time in ascending or descending order
auto sortPopulationByTravelTime(Population& population, bool ascending, const ProblemInstance &problemInstance) -> void;

// Function to print the genome
auto logGenome(const Genome& genome, const std::string& IndividualName, const int generation) -> void;

// Function to flatten the genome
auto flattenGenome(const Genome& parent) -> std::vector<int>;

// Function to unflatten the genome
auto unflattenGenome(std::vector<int> flatGenome, const Genome& parent) -> Genome;

// Export individual to json given a Individual and a path
auto exportIndividual(const Individual& individual, const std::string& path) -> void;

// Function to check if a journey is valid
auto isJourneyValid(const Journey &nurseJourney, const ProblemInstance &problemInstance) -> bool;

// Function to check if a genome is valid
auto isSolutionValid(const Genome &genome, const ProblemInstance &problemInstance) -> bool;

// Function to initialize the logger
auto initLogger() -> void;

// Function to calcute the cosine similarity between two Journeys
auto euclideanDistance(const Journey &journey1, const Journey &journey2) -> double;

// Function to calculate the diversity of a population
auto calculateDiversity(const Population &population) -> double;