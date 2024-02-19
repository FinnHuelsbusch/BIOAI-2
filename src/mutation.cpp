#include "mutation.h"
#include "RandomGenerator.h"

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