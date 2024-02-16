#pragma once

#include "structures.h"
#include <random>
#include <iostream>
#include <algorithm>
#include "utils.cpp"
#include "RandomGenerator.h"

Genome reassignOnePatient(Genome& genome) {
    
    RandomGenerator& rng = RandomGenerator::getInstance();
    int source_nurse = rng.generateRandomInt(0, genome.size() - 1);
    // check that the source has at least one patient
    if (genome[source_nurse].size() == 0) {
        return reassignOnePatient(genome);
    }
    int destination_nurse = rng.generateRandomInt(0, genome.size() - 1);
    int patient_index = rng.generateRandomInt(0, genome[source_nurse].size() - 1);
    int patient = genome[source_nurse][patient_index];
    Journey::iterator it = genome[source_nurse].begin() + patient_index;
    genome[source_nurse].erase(it); 
    //randomly sample insertion point
    int insertion_point = rng.generateRandomInt(0, genome[destination_nurse].size() - 1);
    if (genome[destination_nurse].size() == 0) {
        insertion_point = 0;
    }
    genome[destination_nurse].insert(genome[destination_nurse].begin() + insertion_point, patient);
    return genome;
}