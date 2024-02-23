#pragma once
#include "structures.h"

bool isJourneyValid(const Journey& nurse_journey, const Problem_Instance& problem_instance, bool print_errors);
bool isSolutionValid(const Genome& genome, const Problem_Instance& problem_instance, bool print_errors=false);
double evaluate_genome(const Genome& genome, const Problem_Instance& problem_instance);
Population initialize_random_population(const Problem_Instance& problem_instance, const Config& config);
Population initialize_feasible_population( const Problem_Instance& problem_instance, const Config& config);
double getTotalTravelTime(const Genome& genome, const Problem_Instance& problem_instance);
Population applyCrossover(Population& parents, crossover_configuration& crossover, Problem_Instance& problem_instance);
Population applyMutation(Population& children, mutation_configuration& mutation, Problem_Instance& problem_instance);
void SGA(Problem_Instance& problem_instance, Config& config);


