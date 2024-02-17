#include "parentSelection.h"

#include <algorithm>
#include "RandomGenerator.h"
#include "utils.h"


Population roulette_wheel_selection(const Population& population, const function_parameters& parameters) {
    Population parents;
    std::vector<double> fitnessValues;
    for (int i = 0; i < population.size(); i++) {
        fitnessValues.push_back(population[i].fitness);
    }
    double minFitness = *std::min_element(fitnessValues.begin(), fitnessValues.end());

    // Shift the fitness values so that they are all positive
    std::vector<double> shiftedFitnessValues(fitnessValues.size());
    std::transform(fitnessValues.begin(), fitnessValues.end(), shiftedFitnessValues.begin(),
                   [minFitness](double fitness) { return fitness - minFitness; });

    // Calculate the total fitness
    double totalFitness = std::accumulate(shiftedFitnessValues.begin(), shiftedFitnessValues.end(), 0.0);

    // Calculate the selection probabilities
    std::vector<double> probabilities;
    for (double fitness : shiftedFitnessValues) {
        probabilities.push_back(fitness / totalFitness);
    }

    
    RandomGenerator& rng = RandomGenerator::getInstance();
    // Perform roulette wheel selection
    for (int i = 0; i < population.size(); i++) {
        double r = rng.generateRandomDouble(0, 1);
        double cumulativeProbability = 0;
        for (int j = 0; j < population.size(); j++) {
            cumulativeProbability += probabilities[j];
            if (r <= cumulativeProbability) {
                parents.push_back(population[j]);
                break;
            }
        }
    }
    return parents;
}

Population tournament_selection(const Population& population, const function_parameters& parameters) {
    int tournamentSize = std::get<int>(parameters.at("tournament_size"));
    Population parents;
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int i = 0; i < population.size(); i++) {
        std::vector<Individual> tournament;
        for (int j = 0; j < tournamentSize; j++) {
            int index = rng.generateRandomInt(0, population.size() - 1);
            tournament.push_back(population[index]);
        }
        parents.push_back(*std::max_element(tournament.begin(), tournament.end(), compareByFitness));
    }
    return parents;
}