#include "parentSelection.h"

#include <algorithm>
#include "RandomGenerator.h"
#include "utils.h"
#include <iostream>
#include <stdexcept>
#include <spdlog/spdlog.h>


auto rouletteWheelSelection(const Population &population, const FunctionParameters &parameters, const int populationSize) -> Population
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
    for (int i = 0; i < populationSize; i++)
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

auto tournamentSelection(const Population &population, const FunctionParameters &parameters, const int populationSize) -> Population
{
    auto mainLogger = spdlog::get("main_logger");
    // check that the parameters are present
    if (parameters.find("tournamentSize") == parameters.end() || parameters.find("tournamentProbability") == parameters.end())
    {
        throw std::invalid_argument("Tournament selection requires the parameters 'tournamentSize' and 'tournamentProbability'");
    }

    int tournamentSize = std::get<int>(parameters.at("tournamentSize"));
    double tournamentProbability = std::get<double>(parameters.at("tournamentProbability"));
    mainLogger->trace("TournamentSize {}", tournamentSize);
    mainLogger->trace("TournamentProbability {}", tournamentProbability);
    Population parents;
    RandomGenerator &rng = RandomGenerator::getInstance();
    for (int i = 0; i < populationSize; i++)
    {
        std::vector<int> tournament;
        tournament.reserve(tournamentSize);
        for (int j = 0; j < tournamentSize; j++)
        {
            int index = rng.generateRandomInt(0, population.size() - 1);
            tournament.push_back(index);
        }

        // get index of best individual according to fitness 
        auto it = std::max_element(tournament.begin(), tournament.end(), [&population](int a, int b) {
            return population[a].fitness < population[b].fitness;
        });

        int bestIndex = std::distance(tournament.begin(), it);

        double randNumber = rng.generateRandomDouble(0, 1);
        if (randNumber <= tournamentProbability)
        {
            parents.push_back(population[tournament[bestIndex]]);
        }
        else
        {
            // remove the best individual from the tournament
            tournament.erase(tournament.begin() + bestIndex);
            // select a random individual from the remaining ones
            int randomIndex = rng.generateRandomInt(0, tournament.size() - 1);
            parents.push_back(population[tournament[randomIndex]]);
        }
    }
    return parents;
}