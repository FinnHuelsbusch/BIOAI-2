#pragma once
#include <vector>
#include <variant>
#include <map>
#include <string>
#include <unordered_map>
 
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

    // Constructor
    Problem_Instance(string instance_name, int number_of_nurses, int nurse_capacity, float benchmark, Depot depot, std::unordered_map<int, Patient> patients, std::vector<std::vector<double>> travel_time) : instance_name(instance_name), number_of_nurses(number_of_nurses), nurse_capacity(nurse_capacity), benchmark(benchmark), depot(depot), patients(patients), travel_time(travel_time) {}
}; 

typedef std::vector<int> Journey;

typedef std::vector<Journey> Genome;

struct Individual {
    Genome genome; 
    double fitness;
}; 

typedef std::vector<Individual> Population;

typedef std::map<string, std::variant<int, double, string, bool, Problem_Instance>> function_parameters;
typedef std::pair<Genome, optional<Genome>> (*crossover_function)(const Genome&, const Genome&);
typedef Genome (*mutation_function)(Genome&, const function_parameters& parameters);
typedef Population (*parent_selection_function)(const Population& population, const function_parameters& parameters);
typedef Population (*survivor_selection_function)(const Population& parents, const Population& children, const function_parameters& parameters);


typedef std::vector<std::pair<crossover_function, double>> crossover_configuration;
typedef std::vector<std::tuple<mutation_function, function_parameters&, double>> mutation_configuration;
typedef std::pair<parent_selection_function, function_parameters&> parent_selection_configuration; 
typedef std::pair<survivor_selection_function, function_parameters&> survivor_selection_configuration;

struct Config {
    int population_size;
    int number_of_generations;  
    bool initial_population_distirbute_patients_equally; 
    parent_selection_configuration parent_selection;
    crossover_configuration crossover;
    mutation_configuration mutation;
    survivor_selection_configuration survivor_selection;

    // Constructor
    Config(int population_size, int number_of_generations, bool initial_population_distirbute_patients_equally, parent_selection_configuration parent_selection, crossover_configuration crossover, mutation_configuration mutation, survivor_selection_configuration survivor_selection) : population_size(population_size), number_of_generations(number_of_generations), initial_population_distirbute_patients_equally(initial_population_distirbute_patients_equally), parent_selection(parent_selection), crossover(crossover), mutation(mutation), survivor_selection(survivor_selection) {}
};






