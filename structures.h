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

typedef std::vector<int> Journey;

typedef std::vector<Journey> Genome;

struct Individual {
    Genome genome; 
    double fitness;
}; 

typedef std::vector<Individual> Population;
typedef Population (*parent_selection_function)(const Population&);
typedef std::pair<Genome, Genome> (*crossover_function)(const Genome&, const Genome&);
typedef Genome (*mutation_function)(Genome&);
typedef Population (*survivor_selection_function)(const Population&, const Population&);

struct Config {
    int population_size;
    int number_of_generations;
    double mutation_rate;
    double crossover_rate;    
    int seed;
    bool initial_population_distirbute_patients_equally; 
    parent_selection_function parent_selection;
    crossover_function crossover;
    mutation_function mutation;
    survivor_selection_function survivor_selection;
};






