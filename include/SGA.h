#pragma once
#include "structures.h"

auto isJourneyValid(const Journey &nurseJourney, const ProblemInstance &problemInstance, bool printErrors) -> bool;
auto isSolutionValid(const Genome &genome, const ProblemInstance &problemInstance, bool printErrors = false) -> bool;
auto evaluateIndividual(Individual *individual, const ProblemInstance &problemInstance) -> void;
auto initializeRandomPopulation(const ProblemInstance &problemInstance, const Config &config) -> Population;
auto initializeFeasiblePopulation(const ProblemInstance &problemInstance, const Config &config) -> Population;
auto getTotalTravelTime(const Genome &genome, const ProblemInstance &problemInstance) -> double;
auto applyCrossover(Population &parents, CrossoverConfiguration &crossover, ProblemInstance &problemInstance) -> Population;
auto applyMutation(Population &population, MuationConfiguration &mutation, ProblemInstance &problemInstance) -> Population;
auto SGA(ProblemInstance &problemInstance, Config &config) -> Individual;
