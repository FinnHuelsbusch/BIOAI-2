#include "structures.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

void print_genome(Genome genome){
    for (int i = 0; i < genome.size(); i++) {
        std::cout << "Nurse " << i << " has patients: ";
        for (int j = 0; j < genome[i].size(); j++) {
            std::cout << genome[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

bool isSolutionValid(Genome genome, Problem_Instance problem_instance){

    for (int nurse_id = 0; nurse_id < genome.size(); nurse_id++) {
        std::vector<int> nurse = genome[nurse_id];
        int total_time_spent = 0;
        int total_demand = 0;
        for (int j = 0; j < nurse.size(); j++) {
            int patient_id = nurse[j] ;
            int previous_patient_id = nurse[j - 1];
            if (j == 0) {
                total_time_spent += problem_instance.travel_time[0][patient_id];
            }
            else {
                total_time_spent += problem_instance.travel_time[previous_patient_id][patient_id];
            }
            if (total_time_spent < problem_instance.patients[patient_id].start_time) {
                std::cout << "Patient " << patient_id << " is visited too early" << std::endl;
                return false;
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
        if (nurse.size() > 0) {
            total_time_spent += problem_instance.travel_time[nurse[nurse.size()-1]][0];
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
        std::vector<int> nurse = genome[nurse_id];
        int nurse_trip_time = 0;
        int nurse_used_capacity = 0;
        for (int j = 0; j < nurse.size(); j++) {
            int patient_id = nurse[j] ;
            int previous_patient_id = nurse[j - 1];
            if (j == 0) {
                nurse_trip_time += problem_instance.travel_time[0][patient_id];
            }
            else {
                nurse_trip_time += problem_instance.travel_time[previous_patient_id][patient_id];
            }
            if (nurse_trip_time < problem_instance.patients[patient_id].start_time) {
                combined_penalty += problem_instance.patients[patient_id].start_time - nurse_trip_time; 
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
        if (nurse.size() > 0) {
            nurse_trip_time += problem_instance.travel_time[nurse[nurse.size()-1]][0];
        }
        combined_trip_time += nurse_trip_time;

    }
    return -combined_trip_time - combined_penalty *100; 
}

population initialize_random_population(int population_size, Problem_Instance problem_instance, bool distribute_equally, int seed = -1){
    population pop = std::vector<Individual>();
    pop.reserve(population_size);
    // Seed the random number generator
    if (seed == -1) {
        std::random_device rd;
        seed = rd();
    }
    std::mt19937 g(seed);
    for (int i = 0; i < population_size; i++) {
       // Generate patient
        std::vector<int> IDs = std::vector<int>();
        IDs.reserve(problem_instance.patients.size());

        for (std::pair<int, Patient> kv : problem_instance.patients){
            IDs.push_back(kv.first);
        }
        std::shuffle(IDs.begin(), IDs.end(), g);

        // Distribute the patients over the nurses

        Genome genome = std::vector<std::vector<int>>(problem_instance.number_of_nurses);
        
        // pop from IDs until empty
        for (int i = 0; i < problem_instance.patients.size(); i++) {
            int id = IDs[i];
            if (distribute_equally) {
                int nurse_id = i % problem_instance.number_of_nurses;
                genome[nurse_id].push_back(id);
            } else {
                int nurse_id = g() % problem_instance.number_of_nurses;
                genome[nurse_id].push_back(id);
            }
        }
        // Create the Individual
        Individual individual = {genome, evaluate_genome(genome, problem_instance)}; 
        pop.push_back(individual);

        }
    return pop;

}

pair<Genome, Genome> crossover(Genome parent1, Genome parent2, float crossover_rate, int seed = -1){
    // Seed the random number generator
    if (seed == -1) {
        std::random_device rd;
        seed = rd();
    }
    std::mt19937 g(seed);
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
    if (distribution(g) > crossover_rate) {
        return {parent1, parent2};
    }
    // Create the children
    Genome child1 = std::vector<std::vector<int>>(parent1.size());
    Genome child2 = std::vector<std::vector<int>>(parent2.size());
    for (int i = 0; i < parent1.size(); i++) {
        std::vector<int> nurse1 = parent1[i];
        std::vector<int> nurse2 = parent2[i];
        std::uniform_int_distribution<int> distribution(0, nurse1.size());
        int crossover_point = distribution(g);
        for (int j = 0; j < crossover_point; j++) {
            child1[i].push_back(nurse1[j]);
            child2[i].push_back(nurse2[j]);
        }
        for (int j = crossover_point; j < nurse1.size(); j++) {
            child1[i].push_back(nurse2[j]);
            child2[i].push_back(nurse1[j]);
        }
    }
    return {child1, child2};
}




void SGA(Problem_Instance problem_instance, int seed = -1, int number_of_generations = 100, int population_size = 100, float mutation_rate = 0.01, float crossover_rate = 0.8, int tournament_size = 5, int elite_size = 5){
    // print fitness of the population
    int sum = 0; 
    while (sum == 0)
    {
        population pop = initialize_random_population(population_size, problem_instance, true, seed);
        for (int current_generation = 0; current_generation < number_of_generations; current_generation++) {
            // Sort the population by fitness
            std::sort(pop.begin(), pop.end(), [](Individual a, Individual b) {
                return a.fitness > b.fitness;
            });
            // Print the best fitness
            std::cout << "Generation " << current_generation << " best fitness: " << pop[0].fitness << std::endl;
            // Create the new population
            population new_pop = std::vector<Individual>();
            new_pop.reserve(population_size);
            // Elitism
            for (int i = 0; i < elite_size; i++) {
                new_pop.push_back(pop[i]);
            }
            // Crossover
            

    }
    
}