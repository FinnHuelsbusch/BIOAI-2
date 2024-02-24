#include "parentSelection.h"

#include <algorithm>
#include "RandomGenerator.h"
#include "utils.h"
#include <iostream>

auto rouletteWheelSelection(const Population &population, const FunctionParameters &parameters) -> Population
{
    Population parents;
    std::vector<double> fitnessValues;
    for (const Individual &individual : population)
    {
        fitnessValues.push_back(individual.fitness);
    }
    double minFitness = *std::min_element(fitnessValues.begin(), fitnessValues.end());

    // Shift the fitness values so that they are all positive
    std::vector<double> shiftedFitnessValues(fitnessValues.size());
    std::transform(fitnessValues.begin(), fitnessValues.end(), shiftedFitnessValues.begin(),
                   [minFitness](double fitness)
                   { return (fitness - minFitness) + 0.000001; });

    // Calculate the total fitness
    double totalFitness = std::accumulate(shiftedFitnessValues.begin(), shiftedFitnessValues.end(), 0.0);

    // Calculate the selection probabilities
    std::vector<double> probabilities;
    probabilities.reserve(shiftedFitnessValues.size());
    for (double fitness : shiftedFitnessValues)
    {

        probabilities.push_back(fitness / totalFitness);
    }

    RandomGenerator &rng = RandomGenerator::getInstance();
    // Perform roulette wheel selection
    int popSize = std::get<int>(parameters.at("populationSize"));
    for (int i = 0; i < popSize; i++)
    {
        double randomValue = rng.generateRandomDouble(0, 1);
        double cumulativeProbability = 0;
        for (int j = 0; j < population.size(); j++)
        {
            cumulativeProbability += probabilities[j];
            if (randomValue <= cumulativeProbability)
            {
                parents.push_back(population[j]);
                break;
            }
        }
    }

    return parents;
}

auto tournamentSelection(const Population &population, const FunctionParameters &parameters) -> Population
{
    int tournamentSize = std::get<int>(parameters.at("tournamentSize"));
    Population parents;
    RandomGenerator &rng = RandomGenerator::getInstance();
    for (int i = 0; i < population.size(); i++)
    {
        std::vector<Individual> tournament;
        for (int j = 0; j < tournamentSize; j++)
        {
            int index = rng.generateRandomInt(0, population.size() - 1);
            tournament.push_back(population[index]);
        }
        parents.push_back(*std::max_element(tournament.begin(), tournament.end(), compareByFitness));
    }
    return parents;
}