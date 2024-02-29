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

#include <spdlog/sinks/basic_file_sink.h> 
#include <spdlog/spdlog.h>               

using json = nlohmann::json;

auto loadInstance(const std::string &filename) -> ProblemInstance
{
    auto logger = spdlog::get("main_logger");
    std::ifstream inputFileStream("./../train/" + filename);
    json data = json::parse(inputFileStream);
    const std::string instanceName = data["instance_name"];
    spdlog::get("main_logger")->info("Loading instance: {}", instanceName);
    // load the depot
    Depot depot = {
        data["depot"]["x_coord"],
        data["depot"]["y_coord"],
        data["depot"]["return_time"]};
    // load the patients
    const int numberOfPatients = data["patients"].size();
    logger->info("Number of patients: {}", numberOfPatients);
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
    logger->info("Number of nurses: {}", numberOfNurses);
    logger->info("Nurse capacity: {}", nurseCapacity);
    logger->info("Benchmark: {}", benchmark);
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
    logger->info("Instance loaded");
    return problemInstance;
}

auto runInParallel(ProblemInstance instance, Config config) -> Individual
{
    // Define an array to hold the threads
    // Create an array of individuals
    std::vector<Individual> individuals;

    // Create threads to process each individual
    std::vector<std::thread> threads;
    for (int i = 0; i < 16; ++i)
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
    for (int i = 0; i < individuals.size(); i++)
    {
        if(isSolutionValid(individuals[i].genome, instance)){
            return individuals[i];
        }
    }
    return individuals[0];
}

auto main() -> int
{
    initLogger();
    auto logger = spdlog::get("main_logger");

    logger->info("Loggers created, starting program");

    ProblemInstance problemInstance = loadInstance("train_9.json");
    RandomGenerator &rng = RandomGenerator::getInstance();
    rng.setSeed(4711);

    const int populationSize = 5000;

    FunctionParameters emptyParams;
    // parent selection
    FunctionParameters tournamentSelectionConfigurationParams = {{"tournamentSize", 5}};
    ParentSelectionConfiguration tournamentSelectionConfiguration = {tournamentSelection, tournamentSelectionConfigurationParams};
    ParentSelectionConfiguration rouletteWheelSelectionConfiguration = {rouletteWheelSelection, emptyParams};
    // crossover
    CrossoverConfiguration order1CrossoverConfiguration = {{order1Crossover, 0.3}};
    CrossoverConfiguration partiallyMappedCrossoverConfiguration = {{partiallyMappedCrossover, 0.2}};
    CrossoverConfiguration edgeRecombinationConfiguration = {{edgeRecombination, 0.8}};
    CrossoverConfiguration partiallyMappedCrossoverAndEdgeRecombinationConfiguration = {{partiallyMappedCrossover, 0.2}, {edgeRecombination, 0.2}};
    // mutation
    FunctionParameters twoOptParams = {{"problem_instance", problemInstance}};
    MuationConfiguration reassignOnePatientConfiguration = {{reassignOnePatient, emptyParams, 0.01}};
    MuationConfiguration everyMutationConfiguration = {{reassignOnePatient, emptyParams, 0.01},
                                                       {insertWithinJourney, emptyParams, 0.01},
                                                       {swapBetweenJourneys, emptyParams, 0.01},
                                                       {swapWithinJourney, emptyParams, 0.01},
                                                       {twoOpt, twoOptParams, 0.01}};
    MuationConfiguration insertWithinJourneyConfiguration = {{insertWithinJourney, emptyParams, 0.1}};
    // survivor selection
    SurvivorSelectionConfiguration fullReplacementConfiguration = {fullReplacement, emptyParams};
    SurvivorSelectionConfiguration rouletteWheelSurvivorSelectionConfiguration = {rouletteWheelReplacement, emptyParams};
    FunctionParameters elitismWithFillParams = {{"elitism_percentage", 0.1}, {"fillFunction", "rouletteWheel"}};
    SurvivorSelectionConfiguration elitismWithFillConfiguration = {elitismWithFill, elitismWithFillParams};

    Config config = Config(populationSize, 5000, false,
                           tournamentSelectionConfiguration,
                           partiallyMappedCrossoverAndEdgeRecombinationConfiguration,
                           everyMutationConfiguration,
                           elitismWithFillConfiguration);

    Individual result = runInParallel(problemInstance, config);
    std::cout << "Overall Fittest individual has fitness: " << result.fitness << std::endl;
    if(isSolutionValid(result.genome, problemInstance)){
        std::cout << "Overall Fittest individual is valid" << std::endl;
    } else {
        std::cout << "Overall Fittest individual is invalid" << std::endl;
    }
    std::cout << "Overall Fittest individual uses " <<  100 / problemInstance.benchmark * getTotalTravelTime(result.genome, problemInstance)   << "% of the benchmarks time" << std::endl;
    logger->info("Best individual: {}", result.fitness);
    logger->info("Exporting best individual to json");
    exportIndividual(result, "./../solution.json");
    logger->info("Best individual exported to solution.json");

    return 0;
}
