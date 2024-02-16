#pragma once

#include "structures.h"
#include <random>
#include <iostream>
#include <algorithm>
#include "utils.cpp"

Genome reassignOnePatient(Genome genome) {
    
    std::random_device rd;
    int  seed = rd();
    
    std::mt19937 g(seed);
    std::uniform_int_distribution<int> nurse_distribution(0, genome.size() - 1);
    int source_nurse = nurse_distribution(g);
    int destination_nurse = nurse_distribution(g);
    std::uniform_int_distribution<int> patient_distribution(0, genome[source_nurse].size() - 1);
    int patient_index = patient_distribution(g);
    int patient = genome[source_nurse][patient_index];
    Journey::iterator it = genome[source_nurse].begin() + patient_index;
    genome[source_nurse].erase(it); 
    //randomly sample insertion point
    std::uniform_int_distribution<int> insertion_distribution(0, genome[destination_nurse].size());
    int insertion_point = insertion_distribution(g);
    genome[destination_nurse].insert(genome[destination_nurse].begin() + insertion_point, patient);
    return genome;
}