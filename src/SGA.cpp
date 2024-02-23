#include "SGA.h"
#include "structures.h"

#include "utils.h"
#include "RandomGenerator.h"    
#include <iostream>
#include <climits>

auto isJourneyValid(const Journey &nurse_journey, const Problem_Instance &problem_instance, bool print_errors) -> bool
{
    if(nurse_journey.empty()){
        return true;
    }
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
        if (total_time_spent < problem_instance.patients.at(patient_id).start_time) {
            total_time_spent = problem_instance.patients.at(patient_id).start_time;
        }
        total_time_spent += problem_instance.patients.at(patient_id).care_time;
        if (total_time_spent > problem_instance.patients.at(patient_id).end_time) {
            if (print_errors) {
                std::cout << "Patient " << patient_id << " treatment finishes too late" << '\n';
            }
            return false;
        }
        total_demand += problem_instance.patients.at(patient_id).demand;
        if (total_demand > problem_instance.nurse_capacity) {
            if (print_errors) {
                std::cout << "Nurse exceeds the capacity" << '\n';
            }
            return false;
        }
    }
    // add the driving time from the last patient to the depot if there is at least one patient
    if (nurse_journey.empty()) {
        total_time_spent += problem_instance.travel_time[nurse_journey[nurse_journey.size()-1]][0];
    }
    if (total_time_spent > problem_instance.depot.return_time) {
        if (print_errors) {
            std::cout << "Nurse exceeds the return time" << '\n';
        }
        return false;
    }
    return true;
}

auto isSolutionValid(const Genome &genome, const Problem_Instance &problem_instance, bool print_errors) -> bool
{
    // validate that every patient is visited exactly once
    std::map<int, bool> visited_patients;
    for (const Journey& nurse_journey : genome) {
        for (int patient_id : nurse_journey) {
            if (visited_patients[patient_id]) {
                if (print_errors) {
                    std::cout << "Patient " << patient_id << " is visited more than once" << '\n';
                }
                return false;
            }
            visited_patients[patient_id] = true;
        }
    }
    // validate that every patient is visited
    for (const auto& [id, patient] : problem_instance.patients) {
        if (!visited_patients[id]) {
            if (print_errors) {
                std::cout << "Patient " << id << " is not visited" << '\n';
            }
            return false;
        }
    }

    for (int nurse_id = 0; nurse_id < genome.size(); nurse_id++) {
        const Journey &nurse_journey = genome[nurse_id];
        if (!isJourneyValid(nurse_journey, problem_instance, print_errors)) {
            std::cout << "Nurse " << nurse_id << " journey is invalid" << '\n';
            return false;
        }
    }
    return true;
}

auto evaluate_genome(const Genome &genome, const Problem_Instance &problem_instance) -> double
{
    int combined_penalty = 0;
    int combined_trip_time = 0;

    const auto& travel_time = problem_instance.travel_time;

    for (const Journey& nurse_journey : genome) {
        int nurse_trip_time = 0;
        int nurse_used_capacity = 0;

        for (size_t j = 0; j < nurse_journey.size(); j++) {
            int patient_id = nurse_journey[j];
            if (j == 0) {
                nurse_trip_time += travel_time[0][patient_id];
            }
            else {
                nurse_trip_time += travel_time[nurse_journey[j - 1]][patient_id];
            }
            nurse_trip_time = std::max(nurse_trip_time, problem_instance.patients.at(patient_id).start_time);
            nurse_trip_time += problem_instance.patients.at(patient_id).care_time;

            if (nurse_trip_time > problem_instance.patients.at(patient_id).end_time) {
                combined_penalty += nurse_trip_time - problem_instance.patients.at(patient_id).end_time;
            }

            nurse_used_capacity += problem_instance.patients.at(patient_id).demand;
            if (nurse_used_capacity > problem_instance.nurse_capacity) {
                combined_penalty += (nurse_used_capacity - problem_instance.nurse_capacity) * 100;
            }
        }

        // add the driving time from the last patient to the depot if there is at least one patient
        if (!nurse_journey.empty()) {
            nurse_trip_time += travel_time[nurse_journey.back()][0];
        }

        combined_penalty += std::max(0, nurse_trip_time - problem_instance.depot.return_time);
        combined_trip_time += nurse_trip_time;
    }

    double fitness = -combined_trip_time - (combined_penalty * 100000.0);
    return fitness;
}

auto initialize_random_population(const Problem_Instance &problem_instance, const Config &config) -> Population
{
    Population pop = std::vector<Individual>();
    pop.reserve(config.population_size);
    // Seed the random number generator
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int i = 0; i < config.population_size; i++) {
       // Generate patient
        std::vector<int> PatientIDs = std::vector<int>();
        PatientIDs.reserve(problem_instance.patients.size());

        for (std::pair<int, Patient> PatientID_Patient : problem_instance.patients){
            PatientIDs.push_back(PatientID_Patient.first);
        }
        rng.shuffle(PatientIDs);

        // Distribute the patients over the nurses

        Genome genome = std::vector<std::vector<int>>(problem_instance.number_of_nurses);
        
        // pop from PatientIDs until empty
        for (int i = 0; i < problem_instance.patients.size(); i++) {
            int patientID = PatientIDs[i];
            if (config.initial_population_distirbute_patients_equally) {
                int nurse_id = i % problem_instance.number_of_nurses;
                genome[nurse_id].push_back(patientID);
            } else {
                int nurse_id = rng.generateRandomInt(0, problem_instance.number_of_nurses - 1);
                genome[nurse_id].push_back(patientID);
            }
        }
        // Create the Individual
        Individual individual = {genome, evaluate_genome(genome, problem_instance)}; 
        pop.push_back(individual);

        }
    return pop;
}

auto initialize_feasible_population(const Problem_Instance &problem_instance, const Config &config) -> Population
{
    Population pop = std::vector<Individual>();
    pop.reserve(config.population_size);
    // Seed the random number generator
    RandomGenerator& rng = RandomGenerator::getInstance();

    std::map<int, std::vector<const Patient*>> Patients_by_endTime;
    for (const auto& [id, patient] : problem_instance.patients) {
        Patients_by_endTime[patient.end_time].push_back(&patient);
    }
    std::vector<int> start_times;
    start_times.reserve(Patients_by_endTime.size());
    for (const auto &[start_time, patients] : Patients_by_endTime)
    {
        start_times.push_back(start_time);
    }
    // sort the start times
    std::sort(start_times.begin(), start_times.end());


    for (int i = 0; i < config.population_size; i++) {
        // copy the  Patients_by_endTime 
        std::map<int, std::vector<const Patient*>> Patients_by_endTime_copy(Patients_by_endTime.begin(), Patients_by_endTime.end());
        // init genome with number of nurses x empty vector
        Genome genome = std::vector<std::vector<int>>(problem_instance.number_of_nurses);
        int current_start_time_index = 0;
        int current_start_time = start_times[current_start_time_index];
        int index;

        for(int j = 0; j < problem_instance.patients.size(); j++){

            index = rng.generateRandomInt(0, Patients_by_endTime_copy[current_start_time].size() - 1);
            const Patient* patient = Patients_by_endTime_copy[current_start_time][index];
            Patients_by_endTime_copy[current_start_time].erase(Patients_by_endTime_copy[current_start_time].begin() + index);
            // insert patient in genome
            int min_detour = INT_MAX;
            int min_detour_index = -1;
            for (int k = 0; k < genome.size(); k++) {
                if(genome[k].empty()){
                    min_detour_index = k;
                    min_detour = problem_instance.travel_time[k][patient->id] + problem_instance.travel_time[patient->id][0];
                }else{
                    genome[k].push_back(patient->id);
                    if (isJourneyValid(genome[k], problem_instance, false)){
                        int detour = problem_instance.travel_time[k-1][patient->id] + problem_instance.travel_time[patient->id][0] - problem_instance.travel_time[k-1][0];
                        if (detour < min_detour) {
                            min_detour = detour;
                            min_detour_index = k;
                        }
                    }
                    genome[k].pop_back();
                }
                
            }
            if (min_detour_index != -1) {
                genome[min_detour_index].push_back(patient->id);
            } else {
                break;
            }
            
            // check if the current start time is empty
            if (Patients_by_endTime_copy[current_start_time].empty())
            {
                // remove the current start time from the map
                Patients_by_endTime_copy.erase(current_start_time);
                // check if the map is empty
                if (Patients_by_endTime_copy.empty())
                {
                    break;
                }
                current_start_time_index++;
                current_start_time = start_times[current_start_time_index];
            }
        }
        // Create the Individual
        if(isSolutionValid(genome, problem_instance, true)){
            Individual individual = {genome, evaluate_genome(genome, problem_instance)};
            pop.push_back(individual);
        }
        else{
            i--;
        }
    }
    return pop;
}

auto getTotalTravelTime(const Genome &genome, const Problem_Instance &problem_instance) -> double
{
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
        if (!nurse_journey.empty())
        {
            total_travel_time += problem_instance.travel_time[nurse_journey[nurse_journey.size()-1]][0];
        }
    }
    return total_travel_time;
}

auto applyCrossover(Population &parents, std::vector<std::pair<crossover_function, double>> &crossover, Problem_Instance &problem_instance) -> Population
{
    Population children = std::vector<Individual>();
    children.reserve(parents.size());
    for (const Individual &parent : parents)
    {
        children.push_back(parent);
    }
    RandomGenerator& rng = RandomGenerator::getInstance();

    for (std::pair<crossover_function, double> crossover_pair : crossover) {
        crossover_function crossover_function = crossover_pair.first;
        int number_of_crossovers = std::ceil(parents.size() * crossover_pair.second);

        for (int i = 0; i < number_of_crossovers; i++) {
            int individual_index_1 = rng.generateRandomInt(0, parents.size() - 1);
            int individual_index_2 = rng.generateRandomInt(0, parents.size() - 1);
            while (individual_index_1 == individual_index_2) {
                individual_index_2 = rng.generateRandomInt(0, parents.size() - 1);
            }
            Individual individual_1 = parents[individual_index_1];
            Individual individual_2 = parents[individual_index_2];
            std::pair<Genome, std::optional<Genome>> children_genomes = crossover_function(individual_1.genome, individual_2.genome);
            Individual child_1 = {children_genomes.first, evaluate_genome(children_genomes.first, problem_instance)};
            children[individual_index_1] = child_1;
            if(children_genomes.second.has_value()){
                Individual child_2 = {children_genomes.second.value(), evaluate_genome(children_genomes.second.value(), problem_instance)};
                children[individual_index_2] = child_2;
            }
        }
    }
    return children;
}

auto applyMutation(Population &population, std::vector<std::tuple<mutation_function, function_parameters &, double>> &mutation, Problem_Instance &problem_instance) -> Population
{
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (std::tuple<mutation_function, function_parameters, double> mutation_tuple : mutation) {
        mutation_function mutation_function = std::get<0>(mutation_tuple);
        function_parameters parameters = std::get<1>(mutation_tuple);
        double mutation_rate = std::get<2>(mutation_tuple);
        int number_of_mutations = std::ceil(population.size() * mutation_rate);
        for (int i = 0; i < number_of_mutations; i++) {
            int individual_index = rng.generateRandomInt(0, population.size() - 1);
            Individual individual = population[individual_index];
            Genome mutated_genome = mutation_function(individual.genome, parameters);
            Individual mutated_individual = {mutated_genome, evaluate_genome(mutated_genome, problem_instance)};
            population[individual_index] = mutated_individual;
        }
    }
    return population;
}

void SGA(Problem_Instance& problem_instance, Config& config){

    Population pop = initialize_feasible_population(problem_instance, config);
    // check if population only contains valid solutions
    bool valid = std::all_of(pop.begin(), pop.end(), [&](const Individual &individual)
                             { return isSolutionValid(individual.genome, problem_instance); });
    if (valid) {
        std::cout << "The initial population only contains valid solutions" << '\n';
    }
    else {
        std::cout << "The initial population contains invalid solutions" << '\n';
    }
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int current_generation = 0; current_generation < config.number_of_generations; current_generation++) {
        // Average fitness
        double average_fitness = std::accumulate(pop.begin(), pop.end(), 0.0, [](double sum, const Individual &individual)
                                                 { return sum + individual.fitness; }) /
                                 pop.size();
        std::cout << "Generation " << current_generation << " average population fitness: " << average_fitness << '\n';

        // Parent selection 
        std::cout << "Parent selection";
        Population parents = config.parent_selection.first(pop, config.parent_selection.second);
        // Average parent fitness
        double average_parent_fitness = std::accumulate(parents.begin(), parents.end(), 0.0, [](double sum, const Individual &individual)
                                                        { return sum + individual.fitness; }) /
                                        parents.size();
        std::cout << "Generation " << current_generation << " average parent fitness: " << average_parent_fitness << '\n';
        // Crossover
        std::cout << "Crossover";
        Population children = applyCrossover(parents, config.crossover, problem_instance);
        // Mutation
        std::cout << "Mutation";
        children = applyMutation(children, config.mutation, problem_instance);
        // log children fitness
        double average_children_fitness = std::accumulate(children.begin(), children.end(), 0.0, [](double sum, const Individual &individual) {
            return sum + individual.fitness;
        }) / children.size();
        std::cout << "Generation " << current_generation << " average children fitness: " << average_children_fitness << '\n';
        // Survivor selection
        std::cout << "Survivor selection";
        pop = config.survivor_selection.first(pop, children, config.survivor_selection.second);
        pop = sort_population(pop, false);
        std::cout << "Generation " << current_generation << "\nBest fitness: " << pop[0].fitness << "\nWorst fitness: " << pop[pop.size() - 1].fitness << '\n';
    }
    valid = isSolutionValid(pop[0].genome, problem_instance);
    if (valid) {
        std::cout << "The solution is valid" << '\n';
    }
    else {
        std::cout << "The solution is invalid" << '\n';
    }
    double total_travel_time = getTotalTravelTime(pop[0].genome, problem_instance);
    if (total_travel_time < problem_instance.benchmark) {
        std::cout << "The solution is " << problem_instance.benchmark - total_travel_time << " time units better than the benchmark" << '\n';
    }
    else {
        std::cout << "The solution is " << total_travel_time - problem_instance.benchmark << " time units worse than the benchmark" << '\n';
    }

    
}