#pragma once
#include <iostream>
#include <vector>
 
using namespace std;
 
struct Patient {
    int id;
    int demand; 
    int start_time;
    int end_time;
    int care_time;
    int x_coord;
    int y_coord;
};
 
struct Depot {
    int x_coord;
    int y_coord;
    int return_time;
};

struct Problem_Instance {
    string instance_name;
    int number_of_nurses;
    int nurse_capacity;
    float benchmark;
    Depot depot;
    // hashmap with patient id as key and patient as value
    std::unordered_map<int, Patient> patients;
    // travel time matrix
    std::vector<std::vector<double>> travel_time;
}; 

typedef std::vector<std::vector<int>> Genome;

struct Individual {
    Genome genome; 
    double fitness;
}; 

typedef std::vector<Individual> population;
