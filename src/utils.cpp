#include "structures.h"
#include <algorithm>
#include <iostream>

// custom comparator for sorting the population
auto compareByFitness(const Individual &individualA, const Individual &individualB) -> bool
{
    // sort in descending order (highest fitness first)
    return individualA.fitness < individualB.fitness;
}

auto sort_population(Population population, bool ascending) -> Population
{
    if (ascending) {
        std::sort(population.begin(), population.end(), compareByFitness);
    }
    else {
        std::sort(population.begin(), population.end(), [](const Individual& individualA, const Individual& individualB) {
            return individualA.fitness > individualB.fitness;
        });
    }
    return population;
}

void print_genome(Genome genome){
    for (int i = 0; i < genome.size(); i++) {
        std::cout << "Nurse " << i << " has patients: ";
        for (int patientID : genome[i])
        {
            std::cout << patientID << " ";
        }
        std::cout << '\n';
    }
}

auto flatten_genome(const Genome &genome) -> std::vector<int>
{
    std::vector<int> flat_genome;
    for (const Journey &journey : genome)
    {
        for (int patientID : journey)
        {
            flat_genome.push_back(patientID);
        }
    }
    return flat_genome;
}

auto unflatten_genome(std::vector<int> flat_genome, const Genome &parent) -> Genome
{
    Genome unflattened_genome;
    unflattened_genome.reserve(parent.size());
    int index = 0;
    for (const auto &parentJourney : parent)
    {
        Journey journey;
        journey.reserve(parentJourney.size());
        for (int j = 0; j < parentJourney.size(); j++)
        {
            journey.push_back(flat_genome[index]);
            index++;
        }
        unflattened_genome.push_back(journey);
    }
    return unflattened_genome;
}