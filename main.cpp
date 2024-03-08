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

auto loadFunctionParameters(json functionJson) -> FunctionParameters
{
    FunctionParameters params;
    for (const auto &entry : functionJson.items())
    {
        // ignore when key is 'name' or 'probability'
        if (entry.key() == "name" || entry.key() == "probability")
        {
            continue;
        }
        const auto &value = entry.value();
        if (value.is_number_integer())
        {
            params.insert({entry.key(), value.get<int>()});
        }
        else if (value.is_number_float())
        {
            params.insert({entry.key(), value.get<double>()});
        }
        else if (value.is_string())
        {
            params.insert({entry.key(), value.get<std::string>()});
        }
        else if (value.is_boolean())
        {
            params.insert({entry.key(), value.get<bool>()});
        }
    }
    return params;
}

auto loadConfig(Config* configptr) -> void
{
    auto mainLogger = spdlog::get("main_logger");
    mainLogger->info("Loading config from ./../config.json");
    std::ifstream inputFileStream("./../config.json");
    json data = json::parse(inputFileStream);

    
    int populationSize = data["populationSize"];
    int numberOfGenerations = data["numberOfGenerations"];
    FunctionParameters parentSelectionParameters = loadFunctionParameters(data["parentSelection"]);
    ParentSelectionFunction parentSelection = parentSelectionFunctions.at(data["parentSelection"]["name"]);
    ParentSelectionConfiguration parentSelectionConfiguration = {parentSelection, parentSelectionParameters};

    MutationConfiguration mutations = {};
    for (const auto &entry : data["mutations"])
    {
        const auto &mutation = entry;
        MutationFunction function = mutationFunctions.at(mutation["name"]);
        FunctionParameters params = loadFunctionParameters(mutation);
        double probability = mutation["probability"];
        mutations.push_back({function, params, probability});
    }

    CrossoverConfiguration crossoverConfigurations = {};
    for (const auto &entry : data["crossovers"])
    {
        const auto &crossover = entry;
        CrossoverFunction function = crossoverFunctions.at(crossover["name"]);
        FunctionParameters params = loadFunctionParameters(crossover);
        double probability = crossover["probability"];
        crossoverConfigurations.push_back({function, params, probability});
    }
    



    FunctionParameters survivorSelectionParameters = loadFunctionParameters(data["survivorSelection"]);
    SurvivorSelectionFunction survivorSelection = survivorSelectionFunctions.at(data["survivorSelection"]["name"]);
    SurvivorSelectionConfiguration survivorSelectionConfiguration = {survivorSelection, survivorSelectionParameters};
    
    configptr->populationSize = populationSize;
    configptr->numberOfGenerations = numberOfGenerations;
    configptr->parentSelection = parentSelectionConfiguration;
    configptr->mutation = mutations;
    configptr->crossover = crossoverConfigurations;
    configptr->survivorSelection = survivorSelectionConfiguration;



    std::cout << configptr->populationSize << std::endl;
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
    for (int i = 0; i < individuals.size(); i++)
    {
        if (isSolutionValid(individuals[i].genome, instance))
        {
            return individuals[i];
        }
    }
    return individuals[0];
}

auto main() -> int
{
    initLogger();
    class Config {
    public:
        Config() = default; // Add default constructor
        // Rest of the class definition
    };

    auto logger = spdlog::get("main_logger");

    logger->info("Loggers created, starting program");

    ProblemInstance problemInstance = loadInstance("train_9.json");
    Config* configptr = new Config();
    loadConfig(configptr);
    std::cout << configptr->populationSize << std::endl;


    RandomGenerator &rng = RandomGenerator::getInstance();
    rng.setSeed(4711);


    Individual result = runInParallel(problemInstance, *configptr);
    std::cout << "Overall Fittest individual has fitness: " << result.fitness << std::endl;
    if (isSolutionValid(result.genome, problemInstance))
    {
        std::cout << "Overall Fittest individual is valid" << std::endl;
    }
    else
    {
        std::cout << "Overall Fittest individual is invalid" << std::endl;
    }
    std::cout << "Overall Fittest individual uses " << 100 / problemInstance.benchmark * getTotalTravelTime(result.genome, problemInstance) << "% of the benchmarks time" << std::endl;
    logger->info("Best individual: {}", result.fitness);
    logger->info("Exporting best individual to json");
    exportIndividual(result, "./../solution.json");
    logger->info("Best individual exported to solution.json");
    delete configptr;
    return 0;
}
