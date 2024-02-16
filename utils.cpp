#pragma once
#include "structures.h"
#include <iostream>
#include <algorithm>
#include <vector>

Population sort_population(Population population, bool ascending) {
    if (ascending) {
        std::sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
            return a.fitness < b.fitness;
        });
    }
    else {
        std::sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
            return a.fitness > b.fitness;
        });
    }
    return population;
}

void print_genome(Genome genome){
    for (int i = 0; i < genome.size(); i++) {
        std::cout << "Nurse " << i << " has patients: ";
        for (int j = 0; j < genome[i].size(); j++) {
            std::cout << genome[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<int> flatten_genome(Genome genome) {
    std::vector<int> flat_genome;
    for (int i = 0; i < genome.size(); i++) {
        for (int j = 0; j < genome[i].size(); j++) {
            flat_genome.push_back(genome[i][j]);
        }
    }
    return flat_genome;
}

Genome unflatten_genome(std::vector<int> flat_genome, Genome parent) {
    Genome unflattened_genome;
    unflattened_genome.reserve(parent.size());
    int index = 0;
    for (int i = 0; i < parent.size(); i++) {
        Journey journey;
        journey.reserve(parent[i].size());
        for (int j = 0; j < parent[i].size(); j++) {
            journey.push_back(flat_genome[index]);
            index++;
        }
        unflattened_genome.push_back(journey);
    }
    return unflattened_genome;
}