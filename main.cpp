#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "structures.h"
#include <string>

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
    std::vector<Patient> patients(number_of_patients); 
    for (const auto& entry : data["patients"].items()) {
        const auto& patient_data = entry.value();
        patients.push_back({
            std::stoi(entry.key()),
            patient_data["x_coord"],
            patient_data["y_coord"],
            patient_data["demand"],
            patient_data["start_time"],
            patient_data["end_time"],
            patient_data["care_time"]
        });
    }
    // load the travel time matrix
    const int number_of_nurses = data["nbr_nurses"];
    const int nurse_capacity = data["capacity_nurse"];
    const float benchmark = data["benchmark"];
    std::vector<std::vector<double>> travel_time_matrix(number_of_patients+1, std::vector<double>(number_of_patients+1));
    for(const auto& row : data["travel_times"]){
        std::vector<double> helper(number_of_patients+1);
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
    return problem_instance;
}

int main()
{
    
    Problem_Instance problem_instance = load_instance("train_0.json");

    

    return 0;
}
