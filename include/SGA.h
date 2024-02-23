#pragma once
#include "structures.h"

auto isJourneyValid(const Journey& nurse_journey, const Problem_Instance& problem_instance, bool print_errors) -> bool; 
auto isSolutionValid(const Genome& genome, const Problem_Instance& problem_instance, bool print_errors=false) -> bool;
auto evaluate_genome(const Genome& genome, const Problem_Instance& problem_instance) -> double;
auto initialize_random_population(const Problem_Instance& problem_instance, const Config& config) -> Population;
auto initialize_feasible_population( const Problem_Instance& problem_instance, const Config& config) -> Population;
auto getTotalTravelTime(const Genome& genome, const Problem_Instance& problem_instance) -> double;
auto applyCrossover(Population& parents, crossover_configuration& crossover, Problem_Instance& problem_instance) -> Population;
auto applyMutation(Population &population, mutation_configuration &mutation, Problem_Instance &problem_instance) -> Population;
auto SGA(Problem_Instance& problem_instance, Config& config) -> void; 


