#pragma once
#include <utility>
#include <vector>
#include <variant>
#include <map>
#include <string>
#include <unordered_map>
#include <optional>

struct Patient
{
    int id;
    int demand;
    int startTime;
    int endTime;
    int careTime;
    int xCoord;
    int yCoord;
};

#include <iostream>

struct Depot
{
    int xCoord;
    int yCoord;
    int returnTime;
};

struct ProblemInstance
{
    std::string instanceName;
    int numberOfNurses;
    int nurseCapacity;
    float benchmark;
    Depot depot;
    // hashmap with patient id as key and patient as value
    std::unordered_map<int, Patient> patients;
    // travel time matrix
    std::vector<std::vector<double>> travelTime;

    // Constructor
    ProblemInstance(std::string instanceName, int numberOfNurses, int nurseCapacity, float benchmark, Depot depot, std::unordered_map<int, Patient> patients, std::vector<std::vector<double>> travelTime) : instanceName(std::move(instanceName)), numberOfNurses(numberOfNurses), nurseCapacity(nurseCapacity), benchmark(benchmark), depot(depot), patients(std::move(patients)), travelTime(std::move(travelTime)) {}
};

using Journey = std::vector<int>;

using Genome = std::vector<Journey>;

struct Individual
{
    Genome genome;
    double fitness = 0.0;
    double missingCareTimePenality = 0.0;
    double capacityPenality = 0.0;
    double toLateToDepotPenality = 0.0;
};

using Population = std::vector<Individual>;

using FunctionParameters = std::map<std::string, std::variant<int, double, std::string, bool, ProblemInstance>>;
using CrossoverFunction = std::pair<Genome, std::optional<Genome>> (*)(const Genome &, const Genome &);
using MutationFunction = Genome (*)(Genome &, const FunctionParameters &parameters);
using ParentSelectionFunction = Population (*)(const Population &population, const FunctionParameters &parameters, const int populationSize);
using SurvivorSelectionFunction = Population (*)(const Population &parents, const Population &children, const FunctionParameters &parameters, const int populationSize);

using CrossoverConfiguration = std::vector<std::tuple<CrossoverFunction, FunctionParameters &, double>>;
using MutationConfiguration = std::vector<std::tuple<MutationFunction, FunctionParameters &, double>>;
using ParentSelectionConfiguration = std::pair<ParentSelectionFunction, FunctionParameters &>;
using SurvivorSelectionConfiguration = std::pair<SurvivorSelectionFunction, FunctionParameters &>;

struct Config
{
    int populationSize;
    int numberOfGenerations;
    ParentSelectionConfiguration parentSelection;
    CrossoverConfiguration crossover;
    MutationConfiguration mutation;
    SurvivorSelectionConfiguration survivorSelection;

    // Constructor
    // Constructor
    Config(int popSize, int numGenerations, const ParentSelectionConfiguration &parentSel,
           const CrossoverConfiguration &cross, const MutationConfiguration &mut,
           const SurvivorSelectionConfiguration &survivorSel)
        : populationSize(popSize),
          numberOfGenerations(numGenerations),
          parentSelection(parentSel),
          crossover(cross),
          mutation(mut),
          survivorSelection(survivorSel)
    {
    }

    // Copy Constructor
    Config(const Config &other)
        : populationSize(other.populationSize),
          numberOfGenerations(other.numberOfGenerations),
          parentSelection(other.parentSelection),
          crossover(other.crossover),
          mutation(other.mutation),
          survivorSelection(other.survivorSelection)
    {
    }

};
