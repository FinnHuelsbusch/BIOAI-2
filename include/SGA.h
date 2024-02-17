#pragma once
#include "structures.h"

bool isSolutionValid(Genome genome, Problem_Instance problem_instance); 
double evaluate_genome(Genome genome, Problem_Instance problem_instance);
Population initialize_random_population(Problem_Instance problem_instance, Config config);
double getTotalTravelTime(const Genome& genome, const Problem_Instance& problem_instance);
Population applyCrossover(Population& parents, crossover_configuration& crossover, Problem_Instance& problem_instance);
Population applyMutation(Population& children, mutation_configuration& mutation, Problem_Instance& problem_instance);
void SGA(Problem_Instance& problem_instance, Config& config);


