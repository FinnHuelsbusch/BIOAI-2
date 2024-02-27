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

#include <spdlog/sinks/rotating_file_sink.h> // Include the necessary header file
#include <spdlog/spdlog.h>                   // Include the necessary header file

using json = nlohmann::json;

auto loadInstance(const std::string &filename) -> ProblemInstance
{
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
    for (int i = 0; i < 1; ++i)
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
    return individuals[0];
}

auto main() -> int
{

    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logfile", 1024 * 1024 * 1024, 5);

    // Create a logger with the rotating file sink
    auto logger = std::make_shared<spdlog::logger>("main_logger", rotating_sink);

    // Set the logging pattern if needed
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // Register the logger
    spdlog::register_logger(logger);
    logger->info("Starting the program");

    ProblemInstance problemInstance = loadInstance("train_0.json");
    RandomGenerator &rng = RandomGenerator::getInstance();
    rng.setSeed(42);

    const int populationSize = 100;

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
