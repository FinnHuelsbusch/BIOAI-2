#include "mutation.h"
#include "RandomGenerator.h"
#include <iostream>

Genome reassignOnePatient(Genome& genome, const function_parameters& parameters) {
    
    RandomGenerator& rng = RandomGenerator::getInstance();
    int source_nurse = rng.generateRandomInt(0, genome.size() - 1);
    // check that the source has at least one patient
    if (genome[source_nurse].size() == 0) {
        return reassignOnePatient(genome, parameters);
    }
    int destination_nurse ;
    do {
        destination_nurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (destination_nurse == source_nurse);
    int patient_index = rng.generateRandomInt(0, genome[source_nurse].size() - 1);
    int patient = genome[source_nurse][patient_index];
    Journey::iterator it = genome[source_nurse].begin() + patient_index;
    genome[source_nurse].erase(it); 
    //randomly sample insertion point
    int insertion_point;
    if (genome[destination_nurse].size() == 0) {
        insertion_point = 0;
    } else {
        insertion_point = rng.generateRandomInt(0, genome[destination_nurse].size());
    }
    genome[destination_nurse].insert(genome[destination_nurse].begin() + insertion_point, patient);
    return genome;
}

Genome swapWithinJourney(Genome& genome, const function_parameters& parameters) {
    RandomGenerator& rng = RandomGenerator::getInstance();
    int nurse;
    do{
        nurse = rng.generateRandomInt(0, genome.size() - 1);
    }while(genome[nurse].size() < 2);
    int first_patient_index = rng.generateRandomInt(0, genome[nurse].size() - 1);
    int second_patient_index;
    do {
        second_patient_index = rng.generateRandomInt(0, genome[nurse].size() - 1);
    } while (second_patient_index == first_patient_index);
    std::swap(genome[nurse][first_patient_index], genome[nurse][second_patient_index]);
    return genome;
}

Genome swapBetweenJourneys(Genome& genome, const function_parameters& parameters) {
    RandomGenerator& rng = RandomGenerator::getInstance();
    int source_nurse;
    do {
        source_nurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (genome[source_nurse].size() == 0);
    int destination_nurse;
    do {
        destination_nurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (destination_nurse == source_nurse || genome[destination_nurse].size() == 0);
    int patient_index_nurse1 = rng.generateRandomInt(0, genome[source_nurse].size() - 1);
    int patient_index_nurse2 = rng.generateRandomInt(0, genome[destination_nurse].size() - 1);
    std::swap(genome[source_nurse][patient_index_nurse1], genome[destination_nurse][patient_index_nurse2]);
    return genome;
}

Genome insertWithinJourney(Genome& genome, const function_parameters& parameters) {
    RandomGenerator& rng = RandomGenerator::getInstance();
    int nurse;
    do {
        nurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (genome[nurse].size() < 2);
    int patient_index = rng.generateRandomInt(0, genome[nurse].size() - 1);
    int patient = genome[nurse][patient_index];
    int insertion_point = rng.generateRandomInt(0, genome[nurse].size());
    // insert patient at insertion point
    genome[nurse].erase(genome[nurse].begin() + patient_index);
    genome[nurse].insert(genome[nurse].begin() + insertion_point, patient);
    return genome;
}

Genome twoOpt(Genome& genome, const function_parameters& parameters) {
    const Problem_Instance instance = std::get<Problem_Instance>(parameters.at("problem_instance"));
    RandomGenerator& rng = RandomGenerator::getInstance();
    int nurse;
    vector<int> nursesWithMoreThanFourPatients;
    for (int i = 0; i < genome.size(); i++) {
        if (genome[i].size() > 4) {
            nursesWithMoreThanFourPatients.push_back(i);
        }
    }
    if (nursesWithMoreThanFourPatients.size() == 0) {
        std::cout << "No nurse with more than 4 patients found --> no 2-opt possible --> returning original genome" << std::endl;
        return genome;
    }

    nurse = nursesWithMoreThanFourPatients[rng.generateRandomInt(0, nursesWithMoreThanFourPatients.size() - 1)];
    bool found_improvement;
    do{
        std::cout << "Starting 2-opt" << std::endl;
        found_improvement = false;
        
        for (int i = -1; i <= static_cast<int>(genome[nurse].size() - 2); i++){
            for (int j = i+1; j < genome[nurse].size(); j++){
                Journey journey = genome[nurse];
                double old_cost; 
                double new_cost;
                if (i == -1) {
                    if (j == genome[nurse].size() - 1) {
                        old_cost = instance.travel_time[0][journey[i+1]] + instance.travel_time[journey[j]][0];
                        new_cost = instance.travel_time[0][journey[j]] + instance.travel_time[journey[i+1]][0];
                    } else {
                        old_cost = instance.travel_time[0][journey[i+1]] + instance.travel_time[journey[j]][journey[j+1]];
                        new_cost = instance.travel_time[0][journey[j]] + instance.travel_time[journey[i+1]][journey[j+1]];
                    }
                } else if (j == genome[nurse].size() - 1) {
                    old_cost = instance.travel_time[journey[i]][journey[i+1]] + instance.travel_time[journey[j]][0];
                    new_cost = instance.travel_time[journey[i]][journey[j]] + instance.travel_time[journey[i+1]][0];
                } else {
                    old_cost = instance.travel_time[journey[i]][journey[i+1]] + instance.travel_time[journey[j]][journey[j+1]];
                    new_cost = instance.travel_time[journey[i]][journey[j]] + instance.travel_time[journey[i+1]][journey[j+1]];
                }     
                if (new_cost < old_cost) {
                    std::cout << "Found improvement" << std::endl;
                    found_improvement = true;
                    std::reverse(journey.begin() + i + 1, journey.begin() + j + 1);
                    genome[nurse] = journey;
                }           
            }
        }
    }while(found_improvement);
    return genome;
}