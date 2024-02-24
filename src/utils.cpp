#include "structures.h"
#include <algorithm>
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream> // Include the <fstream> header file



using json = nlohmann::json;


// custom comparator for sorting the population
auto compareByFitness(const Individual &individualA, const Individual &individualB) -> bool
{
    // sort in descending order (highest fitness first)
    return individualA.fitness < individualB.fitness;
}

auto sortPopulation(Population population, bool ascending) -> Population
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

void printGenome(const Genome& genome){
    for (int i = 0; i < genome.size(); i++) {
        std::cout << "Nurse " << i << " has patients: ";
        for (int patientID : genome[i])
        {
            std::cout << patientID << " ";
        }
        std::cout << '\n';
    }
}

auto flattenGenome(const Genome &genome) -> std::vector<int>
{
    std::vector<int> flatGenome;
    for (const Journey &journey : genome)
    {
        for (int patientID : journey)
        {
            flatGenome.push_back(patientID);
        }
    }
    return flatGenome;
}

auto unflattenGenome(std::vector<int> flatGenome, const Genome &parent) -> Genome
{
    Genome unflattenedGenome;
    unflattenedGenome.reserve(parent.size());
    int index = 0;
    for (const auto &parentJourney : parent)
    {
        Journey journey;
        journey.reserve(parentJourney.size());
        for (int j = 0; j < parentJourney.size(); j++)
        {
            journey.push_back(flatGenome[index]);
            index++;
        }
        unflattenedGenome.push_back(journey);
    }
    return unflattenedGenome;
}


auto exportIndividual(const Individual& individual, const std::string& path) -> void
{
    json individualJson;
    individualJson["fitness"] = individual.fitness;
    individualJson["genome"] = individual.genome;
    // write to file
    std::ofstream outputFileStream(path); // Create an instance of std::ofstream
    outputFileStream << individualJson.dump(4);
    outputFileStream.close();
}