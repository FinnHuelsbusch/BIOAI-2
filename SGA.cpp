#include "structures.h"
#include "crossover.cpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "utils.cpp"
#include "RandomGenerator.h"    


bool isSolutionValid(Genome genome, Problem_Instance problem_instance){

    for (int nurse_id = 0; nurse_id < genome.size(); nurse_id++) {
        Journey nurse_journey = genome[nurse_id];
        int total_time_spent = 0;
        int total_demand = 0;
        for (int j = 0; j < nurse_journey.size(); j++) {
            int patient_id = nurse_journey[j] ;
            int previous_patient_id = nurse_journey[j - 1];
            if (j == 0) {
                total_time_spent += problem_instance.travel_time[0][patient_id];
            }
            else {
                total_time_spent += problem_instance.travel_time[previous_patient_id][patient_id];
            }
            if (total_time_spent < problem_instance.patients[patient_id].start_time) {
                total_time_spent = problem_instance.patients[patient_id].start_time;
            }
            total_time_spent += problem_instance.patients[patient_id].care_time;
            if (total_time_spent > problem_instance.patients[patient_id].end_time) {
                std::cout << "Patient " << patient_id << " treatment finishes too late" << std::endl;
                return false;
            }
            total_demand += problem_instance.patients[patient_id].demand;
            if (total_demand > problem_instance.nurse_capacity) {
                std::cout << "Nurse " << nurse_id << " exceeds the capacity" << std::endl;
                return false;
            }
        }
        // add the driving time from the last patient to the depot if there is at least one patient
        if (nurse_journey.size() > 0) {
            total_time_spent += problem_instance.travel_time[nurse_journey[nurse_journey.size()-1]][0];
        }
        if (total_time_spent > problem_instance.depot.return_time) {
            std::cout << "Nurse " << nurse_id << " exceeds the return time" << std::endl;
            return false;
        }
        if (problem_instance.nurse_capacity > problem_instance.nurse_capacity) {
            std::cout << "Nurse " << nurse_id << " exceeds the capacity" << std::endl;
            return false;
        }
    }
    return true;
}

double evaluate_genome(Genome genome, Problem_Instance problem_instance){
    int combined_penalty = 0;
    int combined_trip_time = 0;
    for (int nurse_id = 0; nurse_id < genome.size(); nurse_id++) {
        Journey nurse_journey = genome[nurse_id];
        int nurse_trip_time = 0;
        int nurse_used_capacity = 0;
        for (int j = 0; j < nurse_journey.size(); j++) {
            int patient_id = nurse_journey[j] ;
            int previous_patient_id = nurse_journey[j - 1];
            if (j == 0) {
                nurse_trip_time += problem_instance.travel_time[0][patient_id];
            }
            else {
                nurse_trip_time += problem_instance.travel_time[previous_patient_id][patient_id];
            }
            if (nurse_trip_time < problem_instance.patients[patient_id].start_time) {
                nurse_trip_time = problem_instance.patients[patient_id].start_time;
            }
            nurse_trip_time += problem_instance.patients[patient_id].care_time;
            if (nurse_trip_time > problem_instance.patients[patient_id].end_time) {
                combined_penalty += nurse_trip_time - problem_instance.patients[patient_id].end_time;
            }
            nurse_used_capacity += problem_instance.patients[patient_id].demand;
            if (nurse_used_capacity > problem_instance.nurse_capacity) {
                // time 10 as the range is smaller than the time constraint penalty
                combined_penalty += (nurse_used_capacity - problem_instance.nurse_capacity) * 10;
            }
        }
        // add the driving time from the last patient to the depot if there is at least one patient
        if (nurse_journey.size() > 0) {
            nurse_trip_time += problem_instance.travel_time[nurse_journey[nurse_journey.size()-1]][0];
        }
        if (nurse_trip_time > problem_instance.depot.return_time) {
            combined_penalty += nurse_trip_time - problem_instance.depot.return_time;
        }
        combined_trip_time += nurse_trip_time;

    }
    double fitness = -combined_trip_time - (combined_penalty * 100000.0);
    return fitness;
}

Population initialize_random_population(Problem_Instance problem_instance, Config config){
    Population pop = std::vector<Individual>();
    pop.reserve(config.population_size);
    // Seed the random number generator
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int i = 0; i < config.population_size; i++) {
       // Generate patient
        std::vector<int> IDs = std::vector<int>();
        IDs.reserve(problem_instance.patients.size());

        for (std::pair<int, Patient> kv : problem_instance.patients){
            IDs.push_back(kv.first);
        }
        rng.shuffle(IDs);

        // Distribute the patients over the nurses

        Genome genome = std::vector<std::vector<int>>(problem_instance.number_of_nurses);
        
        // pop from IDs until empty
        for (int i = 0; i < problem_instance.patients.size(); i++) {
            int id = IDs[i];
            if (config.initial_population_distirbute_patients_equally) {
                int nurse_id = i % problem_instance.number_of_nurses;
                genome[nurse_id].push_back(id);
            } else {
                int nurse_id = rng.generateRandomInt(0, problem_instance.number_of_nurses - 1);
                genome[nurse_id].push_back(id);
            }
        }
        // Create the Individual
        Individual individual = {genome, evaluate_genome(genome, problem_instance)}; 
        pop.push_back(individual);

        }
    return pop;

}

double getTotalTravelTime(const Genome& genome, const Problem_Instance& problem_instance){
    double total_travel_time = 0;
    for (Journey nurse_journey : genome) {
        for (int i = 0; i < nurse_journey.size(); i++) {
            int patient_id = nurse_journey[i] ;
            int previous_patient_id = nurse_journey[i - 1];
            if (i == 0) {
                total_travel_time += problem_instance.travel_time[0][patient_id];
            }
            else {
                total_travel_time += problem_instance.travel_time[previous_patient_id][patient_id];
            }
        }
        // add the driving time from the last patient to the depot if there is at least one patient
        if (nurse_journey.size() > 0) {
            total_travel_time += problem_instance.travel_time[nurse_journey[nurse_journey.size()-1]][0];
        }
    }
    return total_travel_time;
}




void SGA(Problem_Instance& problem_instance, Config& config){

    Population pop = initialize_random_population(problem_instance, config);
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int current_generation = 0; current_generation < config.number_of_generations; current_generation++) {
        // Average fitness
        double average_fitness = std::accumulate(pop.begin(), pop.end(), 0.0, [](double sum, Individual individual) {
            return sum + individual.fitness;
        }) / pop.size();
        std::cout << "Generation " << current_generation << " average population fitness: " << average_fitness << std::endl;

        // Parent selection 
        std::cout << "Parent selection";
        Population parents = config.parent_selection(pop);
        // Average parent fitness
        double average_parent_fitness = std::accumulate(parents.begin(), parents.end(), 0.0, [](double sum, Individual individual) {
            return sum + individual.fitness;
        }) / parents.size();
        std::cout << "Generation " << current_generation << " average parent fitness: " << average_parent_fitness << std::endl;
        // Crossover
        std::cout << "Crossover";
        Population children = std::vector<Individual>();
        children.reserve(config.population_size);
        int crossovers = config.crossover_rate * config.population_size / 2;
        for (int i = 0; i < crossovers; i++) {
            
            
            int parent1_index = rng.generateRandomInt(0, parents.size() - 1);
            int parent2_index = rng.generateRandomInt(0, parents.size() - 1);
            Individual parent1 = parents[parent1_index];
            Individual parent2 = parents[parent2_index];
            std::pair<Genome, Genome> children_genomes = config.crossover(parent1.genome, parent2.genome);
            Individual child1 = {children_genomes.first, evaluate_genome(children_genomes.first, problem_instance)};
            Individual child2 = {children_genomes.second, evaluate_genome(children_genomes.second, problem_instance)};
            // replace the parents with the children
            children.push_back(child1);
            children.push_back(child2);
        }
        // Mutation
        std::cout << "Mutation";
        int mutations = config.mutation_rate * config.population_size;
        for (int i = 0; i < mutations; i++) {
            int child_index = rng.generateRandomInt(0, children.size() - 1);
            Individual child = children[child_index];
            Genome mutated_genome = config.mutation(child.genome);
            Individual mutated_child = {mutated_genome, evaluate_genome(mutated_genome, problem_instance)};
            children[child_index] = mutated_child;
        }
        // Survivor selection
        std::cout << "Survivor selection";
        pop = config.survivor_selection(pop, children);
        pop = sort_population(pop, false);
        std::cout << "Generation " << current_generation << "\nBest fitness: " << pop[0].fitness << "\nWorst fitness: " << pop[pop.size()-1].fitness << std::endl;
        
    }
    bool valid = isSolutionValid(pop[0].genome, problem_instance);
    if (valid) {
        std::cout << "The solution is valid" << std::endl;
    }
    else {
        std::cout << "The solution is invalid" << std::endl;
    }
    double total_travel_time = getTotalTravelTime(pop[0].genome, problem_instance);
    if (total_travel_time < problem_instance.benchmark) {
        std::cout << "The solution is " << problem_instance.benchmark - total_travel_time << " time units better than the benchmark" << std::endl;
    }
    else {
        std::cout << "The solution is " << total_travel_time - problem_instance.benchmark << " time units worse than the benchmark" << std::endl;
    }

    
}