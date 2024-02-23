#pragma once
#include <utility>
#include <vector>
#include <variant>
#include <map>
#include <string>
#include <unordered_map>
 
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
    std::string instance_name;
    int number_of_nurses;
    int nurse_capacity;
    float benchmark;
    Depot depot;
    // hashmap with patient id as key and patient as value
    std::unordered_map<int, Patient> patients;
    // travel time matrix
    std::vector<std::vector<double>> travel_time;

    // Constructor
    Problem_Instance(std::string instance_name, int number_of_nurses, int nurse_capacity, float benchmark, Depot depot, std::unordered_map<int, Patient> patients, std::vector<std::vector<double>> travel_time) : instance_name(std::move(instance_name)), number_of_nurses(number_of_nurses), nurse_capacity(nurse_capacity), benchmark(benchmark), depot(depot), patients(std::move(patients)), travel_time(std::move(travel_time)) {}
};

using Journey = std::vector<int>;

using Genome = std::vector<Journey>;

struct Individual {
    Genome genome; 
    double fitness;
};

using Population = std::vector<Individual>;

using function_parameters = std::map<std::string, std::variant<int, double, std::string, bool, Problem_Instance>>;
using crossover_function = std::pair<Genome, std::optional<Genome>> (*)(const Genome &, const Genome &);
using mutation_function = Genome (*)(Genome&, const function_parameters& parameters);
using parent_selection_function = Population (*)(const Population& population, const function_parameters& parameters);
using survivor_selection_function = Population (*)(const Population& parents, const Population& children, const function_parameters& parameters);

using crossover_configuration = std::vector<std::pair<crossover_function, double>>;
using mutation_configuration = std::vector<std::tuple<mutation_function, function_parameters &, double>>;
using parent_selection_configuration = std::pair<parent_selection_function, function_parameters &>;
using survivor_selection_configuration = std::pair<survivor_selection_function, function_parameters &>;

struct Config {
    int population_size;
    int number_of_generations;  
    bool initial_population_distirbute_patients_equally; 
    parent_selection_configuration parent_selection;
    crossover_configuration crossover;
    mutation_configuration mutation;
    survivor_selection_configuration survivor_selection;

    // Constructor
    Config(int population_size, int number_of_generations, bool initial_population_distirbute_patients_equally, parent_selection_configuration parent_selection, crossover_configuration crossover, mutation_configuration mutation, survivor_selection_configuration survivor_selection) : population_size(population_size), number_of_generations(number_of_generations), initial_population_distirbute_patients_equally(initial_population_distirbute_patients_equally), parent_selection(std::move(parent_selection)), crossover(std::move(crossover)), mutation(std::move(mutation)), survivor_selection(std::move(survivor_selection)) {}
};






