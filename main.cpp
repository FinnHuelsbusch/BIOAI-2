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



using json = nlohmann::json;

auto loadInstance(const std::string& filename) -> ProblemInstance {
    std::ifstream inputFileStream("./../train/" + filename);
    json data = json::parse(inputFileStream);
    const std::string instanceName = data["instance_name"];
    std::cout << "Loading instance: " << instanceName << '\n';
    // load the depot
    Depot depot = {
        data["depot"]["x_coord"], 
        data["depot"]["y_coord"], 
        data["depot"]["return_time"]
    };
    // load the patients
    const int numberOfPatients = data["patients"].size();
    std::cout << "Number of patients: " << numberOfPatients << '\n';
    std::unordered_map<int, Patient> patients;
    patients.reserve(numberOfPatients);
    for (const auto& entry : data["patients"].items()) {
        const auto& patientData = entry.value();
        patients.insert({std::stoi(entry.key()), {
            std::stoi(entry.key()),
            patientData["demand"],
            patientData["start_time"],
            patientData["end_time"],
            patientData["care_time"],
            patientData["x_coord"],
            patientData["y_coord"]
        }});
    }
    // load the travel time matrix
    const int numberOfNurses = data["nbr_nurses"];
    const int nurseCapacity = data["capacity_nurse"];
    const float benchmark = data["benchmark"];
    std::vector<std::vector<double>> travelTimeMatrix; 
    travelTimeMatrix.reserve(numberOfPatients+1);
    for(const auto& row : data["travel_times"]){
        std::vector<double> helper;
        helper.reserve(numberOfPatients+1);
        for(const auto& travelTime: row){
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
        travelTimeMatrix
    };
    std::cout << "Done loading instance: " << instanceName << '\n';
    return problemInstance;
}

auto main() -> int
{   
    ProblemInstance problemInstance = loadInstance("train_0.json");
    Genome genome = {
        {1, 2, 3, 4},
        {8, 5, 6, 7},
        {10,  11, 9,  12},
        {13, 15, 16, 14},
        {20, 17, 18, 19},
        {24, 21, 23, 22},
        {25, 27, 28, 26},
        {32, 29, 30, 31},
        {33, 34, 35, 36},
        {40, 38, 39, 37},
        {43, 42, 41, 44, 45, 47},
        {46, 48, 49},
        {50, 51, 52},
        {55, 54, 53, 56},
        {57, 58, 60, 59},
        {63, 62, 61, 64},
        {67, 65, 68, 66},
        {72, 71, 70, 69},
        {75, 76, 74, 73},
        {78, 77, 79, 80},
        {81, 82, 83, 84},
        {87, 86, 85, 88},
        {90, 92, 91, 89},
        {93, 94, 95, 96},
        {97, 100, 99, 98}
    };
    std::cout << "Handcrafted genome is valid: " << isSolutionValid(genome, problemInstance) << '\n';
    std::cout << "Handcrafted genome fitness: " << evaluateGenome(genome, problemInstance) << '\n';
    std::cout << "Handcrafted genome total travel time: " << getTotalTravelTime(genome, problemInstance) << '\n';

    RandomGenerator& rng = RandomGenerator::getInstance();
    rng.setSeed(42);

    // parent selection
    FunctionParameters rouletteWheelSelectionConfigurationParams = {};
    FunctionParameters tournamentSelectionConfigurationParams = {{"tournamentSize", 5}};
    ParentSelectionConfiguration tournamentSelectionConfiguration = {tournamentSelection, tournamentSelectionConfigurationParams};
    ParentSelectionConfiguration rouletteWheelSelectionConfiguration = {rouletteWheelSelection, tournamentSelectionConfigurationParams};
    // crossover
    CrossoverConfiguration order1CrossoverConfiguration = {{order1Crossover, 0.8}};
    CrossoverConfiguration partiallyMappedCrossoverConfiguration = {{partiallyMappedCrossover, 0.8}};
    CrossoverConfiguration edgeRecombinationConfiguration = {{edgeRecombination, 0.8}};
    // mutation
    FunctionParameters emptyParams;
    MuationConfiguration reassignOnePatientConfiguration = {{reassignOnePatient, emptyParams, 2.0}};
    MuationConfiguration everyMutationConfiguration = {{reassignOnePatient, emptyParams, 0.2}, 
                                                        {insertWithinJourney, emptyParams, 0.2},
                                                        {swapBetweenJourneys, emptyParams, 0.2},
                                                        {swapWithinJourney, emptyParams, 0.2}};
    // survivor selection
    SurvivorSelectionConfiguration fullReplacementConfiguration = {fullReplacement, emptyParams};
    SurvivorSelectionConfiguration rouletteWheelSurvivorSelectionConfiguration = {rouletteWheelReplacement, emptyParams};

    
    Config config = Config(100, 100, true, 
        tournamentSelectionConfiguration, 
        partiallyMappedCrossoverConfiguration, 
        reassignOnePatientConfiguration, 
        rouletteWheelSurvivorSelectionConfiguration
    );

    SGA(problemInstance, config);



    return 0;
}
