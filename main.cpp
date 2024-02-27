#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include "nlohmann/json.hpp"

#include "structures.h"
#include "RandomGenerator.h"
#include "SGA.h"

#include "parentSelection.h"
#include "crossover.h"
#include "mutation.h"
#include "survivorSelection.h"
#include <climits>
#include <future>
#include "utils.h"
#include <functional>

using json = nlohmann::json;

auto loadInstance(const std::string &filename) -> ProblemInstance
{
    std::ifstream inputFileStream("./../train/" + filename);
    json data = json::parse(inputFileStream);
    const std::string instanceName = data["instance_name"];
    std::cout << "Loading instance: " << instanceName << '\n';
    // load the depot
    Depot depot = {
        data["depot"]["x_coord"],
        data["depot"]["y_coord"],
        data["depot"]["return_time"]};
    // load the patients
    const int numberOfPatients = data["patients"].size();
    std::cout << "Number of patients: " << numberOfPatients << '\n';
    std::unordered_map<int, Patient> patients;
    patients.reserve(numberOfPatients);
    for (const auto &entry : data["patients"].items())
    {
        const auto &patientData = entry.value();
        patients.insert({std::stoi(entry.key()), {std::stoi(entry.key()), patientData["demand"], patientData["start_time"], patientData["end_time"], patientData["care_time"], patientData["x_coord"], patientData["y_coord"]}});
    }
    // load the travel time matrix
    const int numberOfNurses = data["nbr_nurses"];
    const int nurseCapacity = data["capacity_nurse"];
    const float benchmark = data["benchmark"];
    std::vector<std::vector<double>> travelTimeMatrix;
    travelTimeMatrix.reserve(numberOfPatients + 1);
    for (const auto &row : data["travel_times"])
    {
        std::vector<double> helper;
        helper.reserve(numberOfPatients + 1);
        for (const auto &travelTime : row)
        {
            helper.push_back(travelTime);
        }
        travelTimeMatrix.push_back(helper);
    }

    ProblemInstance problemInstance = {
        instanceName,
        numberOfNurses,
        nurseCapacity,
        benchmark,
        depot,
        patients,
        travelTimeMatrix};
    std::cout << "Done loading instance: " << instanceName << '\n';
    return problemInstance;
}

auto runInParallel(ProblemInstance instance, Config config) -> Individual
{
    // Define an array to hold the threads
    // Create an array of individuals
    std::vector<Individual> individuals;

    // Create threads to process each individual
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i)
    { // Adjust num_individuals as needed
        threads.emplace_back([&]()
                             { individuals.push_back(SGA(instance, config)); });
    }

    // Join all threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    sortPopulationByFitness(individuals, false);
    return results[0];
}

auto main() -> int
{
    ProblemInstance problemInstance = loadInstance("train_0.json");
    RandomGenerator &rng = RandomGenerator::getInstance();
    rng.setSeed(42);

    const int populationSize = 1000;

    // parent selection
    FunctionParameters rouletteWheelSelectionConfigurationParams = {{"populationSize", populationSize}};
    FunctionParameters tournamentSelectionConfigurationParams = {{"tournamentSize", 5}};
    ParentSelectionConfiguration tournamentSelectionConfiguration = {tournamentSelection, tournamentSelectionConfigurationParams};
    ParentSelectionConfiguration rouletteWheelSelectionConfiguration = {rouletteWheelSelection, rouletteWheelSelectionConfigurationParams};
    // crossover
    CrossoverConfiguration order1CrossoverConfiguration = {{order1Crossover, 0.3}};
    CrossoverConfiguration partiallyMappedCrossoverConfiguration = {{partiallyMappedCrossover, 0.2}};
    CrossoverConfiguration edgeRecombinationConfiguration = {{edgeRecombination, 0.8}};
    // mutation
    FunctionParameters emptyParams;
    MuationConfiguration reassignOnePatientConfiguration = {{reassignOnePatient, emptyParams, 0.01}};
    MuationConfiguration everyMutationConfiguration = {{reassignOnePatient, emptyParams, 0.001},
                                                       {insertWithinJourney, emptyParams, 0.001},
                                                       {swapBetweenJourneys, emptyParams, 0.001},
                                                       {swapWithinJourney, emptyParams, 0.001}};
    // survivor selection
    SurvivorSelectionConfiguration fullReplacementConfiguration = {fullReplacement, emptyParams};
    SurvivorSelectionConfiguration rouletteWheelSurvivorSelectionConfiguration = {rouletteWheelReplacement, rouletteWheelSelectionConfigurationParams};

    Config config = Config(populationSize, 100, false,
                           tournamentSelectionConfiguration,
                           order1CrossoverConfiguration,
                           reassignOnePatientConfiguration,
                           rouletteWheelSurvivorSelectionConfiguration);

    Individual result = runInParallel(problemInstance, config);
    std::cout << result.fitness << std::endl;

    return 0;
}
