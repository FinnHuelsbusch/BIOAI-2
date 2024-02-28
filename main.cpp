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
    std::string main_log_file_path = "logfile.txt";
    std::string statistics_log_file_path = "statistics.txt";

    // Check if log files exist, and if so, remove them
    if (std::filesystem::exists(main_log_file_path)) {
        std::filesystem::remove(main_log_file_path);
    }

    if (std::filesystem::exists(statistics_log_file_path)) {
        std::filesystem::remove(statistics_log_file_path);
    }

    // Create a basic file sink
    auto main_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(main_log_file_path);
    auto statistics_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(statistics_log_file_path);


    // Create a logger with the basic file sink
    auto logger = std::make_shared<spdlog::logger>("main_logger", main_file_sink);
    auto statistics_logger = std::make_shared<spdlog::logger>("statistics_logger", statistics_file_sink);

    // Set the logging pattern with the thread id
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");
    statistics_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");


    #if defined(PRODUCTION)
        logger->set_level(spdlog::level::trace);
        statistics_logger->set_level(spdlog::level::trace);
    #else
        logger->set_level(spdlog::level::trace);
        statistics_logger->set_level(spdlog::level::trace);
    #endif

    // Register the logger
    spdlog::register_logger(logger);
    spdlog::register_logger(statistics_logger);

    logger->info("Loggers created, starting program");

    ProblemInstance problemInstance = loadInstance("train_0.json");
    RandomGenerator &rng = RandomGenerator::getInstance();
    rng.setSeed(42);

    const int populationSize = 5000;

    // parent selection
    FunctionParameters rouletteWheelSelectionConfigurationParams = {{"populationSize", populationSize}};
    FunctionParameters tournamentSelectionConfigurationParams = {{"tournamentSize", 5}};
    ParentSelectionConfiguration tournamentSelectionConfiguration = {tournamentSelection, tournamentSelectionConfigurationParams};
    ParentSelectionConfiguration rouletteWheelSelectionConfiguration = {rouletteWheelSelection, rouletteWheelSelectionConfigurationParams};
    // crossover
    CrossoverConfiguration order1CrossoverConfiguration = {{order1Crossover, 0.3}};
    CrossoverConfiguration partiallyMappedCrossoverConfiguration = {{partiallyMappedCrossover, 0.2}};
    CrossoverConfiguration edgeRecombinationConfiguration = {{edgeRecombination, 0.8}};
    CrossoverConfiguration partiallyMappedCrossoverAndEdgeRecombinationConfiguration = {{partiallyMappedCrossover, 0.2}, {edgeRecombination, 0.2}};
    // mutation
    FunctionParameters emptyParams;
    MuationConfiguration reassignOnePatientConfiguration = {{reassignOnePatient, emptyParams, 0.01}};
    MuationConfiguration everyMutationConfiguration = {{reassignOnePatient, emptyParams, 0.01},
                                                       //{insertWithinJourney, emptyParams, 0.001},
                                                       {swapBetweenJourneys, emptyParams, 0.01},
                                                       {swapWithinJourney, emptyParams, 0.01}};
    // survivor selection
    SurvivorSelectionConfiguration fullReplacementConfiguration = {fullReplacement, emptyParams};
    SurvivorSelectionConfiguration rouletteWheelSurvivorSelectionConfiguration = {rouletteWheelReplacement, rouletteWheelSelectionConfigurationParams};

    Config config = Config(populationSize, 500, false,
                           tournamentSelectionConfiguration,
                           partiallyMappedCrossoverAndEdgeRecombinationConfiguration,
                           everyMutationConfiguration,
                           rouletteWheelSurvivorSelectionConfiguration);

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
