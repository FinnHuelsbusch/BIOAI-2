#pragma once
#include "structures.h"
#include <iostream>
#include <random>
#include "utils.cpp"
#include "RandomGenerator.h"



std::pair<Genome, Genome> order1Crossover(Genome parent1, Genome parent2) {
    
    std::vector<int> parent1_flat = flatten_genome(parent1);
    std::vector<int> parent2_flat = flatten_genome(parent2);

    RandomGenerator& rng = RandomGenerator::getInstance();
    int start = rng.generateRandomInt(0, parent1_flat.size() - 1);
    int end = rng.generateRandomInt(0, parent1_flat.size() - 1);
    if (start > end) {
        std::swap(start, end);
    }
    std::vector<int> child1_flat = std::vector<int>(parent1_flat.size());
    std::vector<int> child2_flat = std::vector<int>(parent2_flat.size());

    // copy the selected part from parent1 to child1 and the selected part from parent2 to child2
    for (int i = start; i <= end; i++) {
        child1_flat[i] = parent1_flat[i];
        child2_flat[i] = parent2_flat[i];
    }

    // fill the rest of the child with the remaining genes from the other parent
    for (int i = 1; i < parent1_flat.size() - (end - start); i++) {
        int index = (end + i) % parent1_flat.size();
        int j = index; 
        while (std::find(child1_flat.begin(), child1_flat.end(), parent2_flat[j]) != child1_flat.end()) {
            j = (j + 1) % parent1_flat.size();
        }
        child1_flat[index] = parent2_flat[j];
    }
    
    for (int i = 1; i < parent2_flat.size() - (end - start); i++) {
        int index = (end + i) % parent2_flat.size();
        int j = index; 
        while (std::find(child2_flat.begin(), child2_flat.end(), parent1_flat[j]) != child2_flat.end()) {
            j = (j + 1) % parent2_flat.size();
        }
        child2_flat[index] = parent1_flat[j];
    }
    Genome child1 = unflatten_genome(child1_flat, parent1);
    Genome child2 = unflatten_genome(child2_flat, parent2);
    return std::make_pair(child1, child2);
}