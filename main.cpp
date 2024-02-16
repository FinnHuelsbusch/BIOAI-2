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
    // instantiate the random number generator
    RandomGenerator& rng = RandomGenerator::getInstance();    
    rng.seedGenerator(4711); 
    // load the problem instance
    Problem_Instance problem_instance = load_instance("train_0.json");
    Config config = {
        100, 
        100, 
        0.1, 
        0.9, 
        true, 
        roulette_wheel_selection, 
        order1Crossover, 
        reassignOnePatient, 
        full_replacement
    };
    SGA(problem_instance,config);
    return 0;
}
