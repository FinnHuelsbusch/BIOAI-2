#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "structures.h"
#include <string>
#include "SGA.cpp"
#include "crossover.cpp"
#include "mutation.cpp"
#include "survivorSelection.cpp"
#include "parentSelection.cpp"
#include "RandomGenerator.h"


using json = nlohmann::json;

Problem_Instance load_instance(const std::string& filename) {
    std::ifstream i("/workspaces/BIOAI-2/train/train_0.json");
    json data = json::parse(i);
    const string instance_name = data["instance_name"];
    std::cout << "Loading instance: " << instance_name << std::endl;
    // load the depot
    Depot depot = {
        data["depot"]["x_coord"], 
        data["depot"]["y_coord"], 
        data["depot"]["return_time"]
    };
    // load the patients
    const int number_of_patients = data["patients"].size();
    std::cout << "Number of patients: " << number_of_patients << std::endl;
    std::unordered_map<int, Patient> patients;
    patients.reserve(number_of_patients);
    for (const auto& entry : data["patients"].items()) {
        const auto& patient_data = entry.value();
        patients.insert({std::stoi(entry.key()), {
            std::stoi(entry.key()),
            patient_data["demand"],
            patient_data["start_time"],
            patient_data["end_time"],
            patient_data["care_time"],
            patient_data["x_coord"],
            patient_data["y_coord"]
        }});
    }
    // load the travel time matrix
    const int number_of_nurses = data["nbr_nurses"];
    const int nurse_capacity = data["capacity_nurse"];
    const float benchmark = data["benchmark"];
    std::vector<std::vector<double>> travel_time_matrix; 
    travel_time_matrix.reserve(number_of_patients+1);
    for(const auto& row : data["travel_times"]){
        std::vector<double> helper;
        helper.reserve(number_of_patients+1);
        for(const double& travel_time: row){
            helper.push_back(travel_time); 
        }
        travel_time_matrix.push_back(helper);
    }

    Problem_Instance problem_instance = {
        instance_name, 
        number_of_nurses, 
        nurse_capacity, 
        benchmark, 
        depot, 
        patients, 
        travel_time_matrix
    };
    std::cout << "Done loading instance: " << instance_name << std::endl;
    return problem_instance;
}

int main()
{   
    Problem_Instance problem_instance = load_instance("train_0.json");
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
    std::cout << "Handcrafted genome is valid: " << isSolutionValid(genome, problem_instance) << std::endl;
    std::cout << "Handcrafted genome fitness: " << evaluate_genome(genome, problem_instance) << std::endl;
    std::cout << "Handcrafted genome total travel time: " << getTotalTravelTime(genome, problem_instance) << std::endl;
    if (getTotalTravelTime(genome, problem_instance) < problem_instance.benchmark) {
        std::cout << "Handcrafted genome is valid and better than benchmark by " << problem_instance.benchmark - getTotalTravelTime(genome, problem_instance) << std::endl;
    }
    else {
        std::cout << "Handcrafted genome is valid but worse than benchmark by " << getTotalTravelTime(genome, problem_instance) - problem_instance.benchmark << std::endl;
    }

    // instantiate the random number generator
    RandomGenerator& rng = RandomGenerator::getInstance();    
    rng.seedGenerator(4711); 
    // load the problem instance
    Config config = {
        100, 
        1000, 
        0.6, 
        0.8, 
        true, 
        roulette_wheel_selection, 
        order1Crossover, 
        reassignOnePatient, 
        full_replacement
    };
    SGA(problem_instance,config);
    return 0;
}
