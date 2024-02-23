#include "SGA.h"
#include "structures.h"

#include "utils.h"
#include "RandomGenerator.h"    
#include <iostream>
#include <climits>

auto isJourneyValid(const Journey &nurseJourney, const ProblemInstance &problemInstance, bool printErrors) -> bool
{
    if(nurseJourney.empty()){
        return true;
    }
    int totalTimeSpent = 0;
    int totalDemand = 0;
    for (int j = 0; j < nurseJourney.size(); j++) {
        int patientId = nurseJourney[j] ;
        int previousPatientId = nurseJourney[j - 1];
        if (j == 0) {
            totalTimeSpent += problemInstance.travelTime[0][patientId];
        }
        else {
            totalTimeSpent += problemInstance.travelTime[previousPatientId][patientId];
        }
        if (totalTimeSpent < problemInstance.patients.at(patientId).startTime) {
            totalTimeSpent = problemInstance.patients.at(patientId).startTime;
        }
        totalTimeSpent += problemInstance.patients.at(patientId).careTime;
        if (totalTimeSpent > problemInstance.patients.at(patientId).endTime) {
            if (printErrors) {
                std::cout << "Patient " << patientId << " treatment finishes too late" << '\n';
            }
            return false;
        }
        totalDemand += problemInstance.patients.at(patientId).demand;
        if (totalDemand > problemInstance.nurseCapacity) {
            if (printErrors) {
                std::cout << "Nurse exceeds the capacity" << '\n';
            }
            return false;
        }
    }
    // add the driving time from the last patient to the depot if there is at least one patient
    if (nurseJourney.empty()) {
        totalTimeSpent += problemInstance.travelTime[nurseJourney[nurseJourney.size()-1]][0];
    }
    if (totalTimeSpent > problemInstance.depot.returnTime) {
        if (printErrors) {
            std::cout << "Nurse exceeds the return time" << '\n';
        }
        return false;
    }
    return true;
}

auto isSolutionValid(const Genome &genome, const ProblemInstance &problemInstance, bool printErrors) -> bool
{
    // validate that every patient is visited exactly once
    std::map<int, bool> visitedPatients;
    for (const Journey& nurseJourney : genome) {
        for (int patientId : nurseJourney) {
            if (visitedPatients[patientId]) {
                if (printErrors) {
                    std::cout << "Patient " << patientId << " is visited more than once" << '\n';
                }
                return false;
            }
            visitedPatients[patientId] = true;
        }
    }
    // validate that every patient is visited
    for (const auto& [id, patient] : problemInstance.patients) {
        if (!visitedPatients[id]) {
            if (printErrors) {
                std::cout << "Patient " << id << " is not visited" << '\n';
            }
            return false;
        }
    }

    for (int nurseId = 0; nurseId < genome.size(); nurseId++) {
        const Journey &nurseJourney = genome[nurseId];
        if (!isJourneyValid(nurseJourney, problemInstance, printErrors)) {
            std::cout << "Nurse " << nurseId << " journey is invalid" << '\n';
            return false;
        }
    }
    return true;
}

auto evaluateGenome(const Genome &genome, const ProblemInstance &problemInstance) -> double
{
    int combinedPenalty = 0;
    int combinedTripTime = 0;

    const auto& travelTime = problemInstance.travelTime;

    for (const Journey& nurseJourney : genome) {
        int nurseTripTime = 0;
        int nurseUsedCapacity = 0;

        for (std::size_t j = 0; j < nurseJourney.size(); j++) {
            int patientId = nurseJourney[j];
            if (j == 0) {
                nurseTripTime += travelTime[0][patientId];
            }
            else {
                nurseTripTime += travelTime[nurseJourney[j - 1]][patientId];
            }
            nurseTripTime = std::max(nurseTripTime, problemInstance.patients.at(patientId).startTime);
            nurseTripTime += problemInstance.patients.at(patientId).careTime;

            if (nurseTripTime > problemInstance.patients.at(patientId).endTime) {
                combinedPenalty += nurseTripTime - problemInstance.patients.at(patientId).endTime;
            }

            nurseUsedCapacity += problemInstance.patients.at(patientId).demand;
            if (nurseUsedCapacity > problemInstance.nurseCapacity) {
                combinedPenalty += (nurseUsedCapacity - problemInstance.nurseCapacity) * 100;
            }
        }

        // add the driving time from the last patient to the depot if there is at least one patient
        if (!nurseJourney.empty()) {
            nurseTripTime += travelTime[nurseJourney.back()][0];
        }

        combinedPenalty += std::max(0, nurseTripTime - problemInstance.depot.returnTime);
        combinedTripTime += nurseTripTime;
    }

    double fitness = -combinedTripTime - (combinedPenalty * 100000.0);
    return fitness;
}

auto initializeRandomPopulation(const ProblemInstance &problemInstance, const Config &config) -> Population
{
    Population pop = std::vector<Individual>();
    pop.reserve(config.populationSize);
    // Seed the random number generator
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int i = 0; i < config.populationSize; i++) {
       // Generate patient
        std::vector<int> PatientIDs = std::vector<int>();
        PatientIDs.reserve(problemInstance.patients.size());

        for (std::pair<int, Patient> PatientID_Patient : problemInstance.patients){
            PatientIDs.push_back(PatientID_Patient.first);
        }
        rng.shuffle(PatientIDs);

        // Distribute the patients over the nurses

        Genome genome = std::vector<std::vector<int>>(problemInstance.numberOfNurses);
        
        // pop from PatientIDs until empty
        for (int i = 0; i < problemInstance.patients.size(); i++) {
            int patientID = PatientIDs[i];
            if (config.initialPopulationDistirbutePatientsEqually) {
                int nurseId = i % problemInstance.numberOfNurses;
                genome[nurseId].push_back(patientID);
            } else {
                int nurseId = rng.generateRandomInt(0, problemInstance.numberOfNurses - 1);
                genome[nurseId].push_back(patientID);
            }
        }
        // Create the Individual
        Individual individual = {genome, evaluateGenome(genome, problemInstance)}; 
        pop.push_back(individual);

        }
    return pop;
}

auto initializeFeasiblePopulation(const ProblemInstance &problemInstance, const Config &config) -> Population
{
    Population pop = std::vector<Individual>();
    pop.reserve(config.populationSize);
    // Seed the random number generator
    RandomGenerator& rng = RandomGenerator::getInstance();

    std::map<int, std::vector<const Patient*>> PatientsByEndTime;
    for (const auto& [id, patient] : problemInstance.patients) {
        PatientsByEndTime[patient.endTime].push_back(&patient);
    }
    std::vector<int> startTimes;
    startTimes.reserve(PatientsByEndTime.size());
    for (const auto &[startTime, patients] : PatientsByEndTime)
    {
        startTimes.push_back(startTime);
    }
    // sort the start times
    std::sort(startTimes.begin(), startTimes.end());


    for (int i = 0; i < config.populationSize; i++) {
        // copy the  PatientsByEndTime 
        std::map<int, std::vector<const Patient*>> PatientsByEndTimeCopy(PatientsByEndTime.begin(), PatientsByEndTime.end());
        // init genome with number of nurses x empty vector
        Genome genome = std::vector<std::vector<int>>(problemInstance.numberOfNurses);
        int currentStartTimeIndex = 0;
        int currentStartTime = startTimes[currentStartTimeIndex];
        int index;

        for(int j = 0; j < problemInstance.patients.size(); j++){

            index = rng.generateRandomInt(0, PatientsByEndTimeCopy[currentStartTime].size() - 1);
            const Patient* patient = PatientsByEndTimeCopy[currentStartTime][index];
            PatientsByEndTimeCopy[currentStartTime].erase(PatientsByEndTimeCopy[currentStartTime].begin() + index);
            // insert patient in genome
            int minDetour = INT_MAX;
            int minDetourIndex = -1;
            for (int k = 0; k < genome.size(); k++) {
                if(genome[k].empty()){
                    minDetourIndex = k;
                    minDetour = problemInstance.travelTime[k][patient->id] + problemInstance.travelTime[patient->id][0];
                }else{
                    genome[k].push_back(patient->id);
                    if (isJourneyValid(genome[k], problemInstance, false)){
                        int detour = problemInstance.travelTime[k-1][patient->id] + problemInstance.travelTime[patient->id][0] - problemInstance.travelTime[k-1][0];
                        if (detour < minDetour) {
                            minDetour = detour;
                            minDetourIndex = k;
                        }
                    }
                    genome[k].pop_back();
                }
                
            }
            if (minDetourIndex != -1) {
                genome[minDetourIndex].push_back(patient->id);
            } else {
                break;
            }
            
            // check if the current start time is empty
            if (PatientsByEndTimeCopy[currentStartTime].empty())
            {
                // remove the current start time from the map
                PatientsByEndTimeCopy.erase(currentStartTime);
                // check if the map is empty
                if (PatientsByEndTimeCopy.empty())
                {
                    break;
                }
                currentStartTimeIndex++;
                currentStartTime = startTimes[currentStartTimeIndex];
            }
        }
        // Create the Individual
        if(isSolutionValid(genome, problemInstance, true)){
            Individual individual = {genome, evaluateGenome(genome, problemInstance)};
            pop.push_back(individual);
        }
        else{
            i--;
        }
    }
    return pop;
}

auto getTotalTravelTime(const Genome &genome, const ProblemInstance &problemInstance) -> double
{
    double totalTravelTime = 0;
    for (Journey nurseJourney : genome) {
        for (int i = 0; i < nurseJourney.size(); i++) {
            int patientId = nurseJourney[i] ;
            int previousPatientId = nurseJourney[i - 1];
            if (i == 0) {
                totalTravelTime += problemInstance.travelTime[0][patientId];
            }
            else {
                totalTravelTime += problemInstance.travelTime[previousPatientId][patientId];
            }
        }
        // add the driving time from the last patient to the depot if there is at least one patient
        if (!nurseJourney.empty())
        {
            totalTravelTime += problemInstance.travelTime[nurseJourney[nurseJourney.size()-1]][0];
        }
    }
    return totalTravelTime;
}
auto applyCrossover(Population &parents, CrossoverConfiguration &crossover, ProblemInstance &problemInstance) -> Population
{
    Population children = std::vector<Individual>();
    children.reserve(parents.size());
    for (const Individual &parent : parents)
    {
        children.push_back(parent);
    }
    RandomGenerator& rng = RandomGenerator::getInstance();

    for (std::pair<CrossoverFunction, double> crossoverPair : crossover) {
        CrossoverFunction CrossoverFunction = crossoverPair.first;
        int numberOfCrossovers = std::ceil(parents.size() * crossoverPair.second);

        for (int i = 0; i < numberOfCrossovers; i++) {
            int individualIndex1 = rng.generateRandomInt(0, parents.size() - 1);
            int individualIndex2 = rng.generateRandomInt(0, parents.size() - 1);
            while (individualIndex1 == individualIndex2) {
                individualIndex2 = rng.generateRandomInt(0, parents.size() - 1);
            }
            Individual individual1 = parents[individualIndex1];
            Individual individual2 = parents[individualIndex2];
            std::pair<Genome, std::optional<Genome>> childrenGenomes = CrossoverFunction(individual1.genome, individual2.genome);
            Individual child1 = {childrenGenomes.first, evaluateGenome(childrenGenomes.first, problemInstance)};
            children[individualIndex1] = child1;
            if(childrenGenomes.second.has_value()){
                Individual child2 = {childrenGenomes.second.value(), evaluateGenome(childrenGenomes.second.value(), problemInstance)};
                children[individualIndex2] = child2;
            }
        }
    }
    return children;
}

auto applyMutation(Population &population, MuationConfiguration &mutation, ProblemInstance &problemInstance) -> Population
{
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (std::tuple<MutationFunction, FunctionParameters, double> mutationTuple : mutation) {
        MutationFunction MutationFunction = std::get<0>(mutationTuple);
        FunctionParameters parameters = std::get<1>(mutationTuple);
        double mutationRate = std::get<2>(mutationTuple);
        int numberOfMutations = std::ceil(population.size() * mutationRate);
        for (int i = 0; i < numberOfMutations; i++) {
            int individualIndex = rng.generateRandomInt(0, population.size() - 1);
            Individual individual = population[individualIndex];
            Genome mutatedGenome = MutationFunction(individual.genome, parameters);
            Individual mutatedIndividual = {mutatedGenome, evaluateGenome(mutatedGenome, problemInstance)};
            population[individualIndex] = mutatedIndividual;
        }
    }
    return population;
}

void SGA(ProblemInstance& problemInstance, Config& config){

    Population pop = initializeFeasiblePopulation(problemInstance, config);
    // check if population only contains valid solutions
    bool valid = std::all_of(pop.begin(), pop.end(), [&](const Individual &individual)
                             { return isSolutionValid(individual.genome, problemInstance); });
    if (valid) {
        std::cout << "The initial population only contains valid solutions" << '\n';
    }
    else {
        std::cout << "The initial population contains invalid solutions" << '\n';
    }
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int currentGeneration = 0; currentGeneration < config.numberOfGenerations; currentGeneration++) {
        // Average fitness
        double averageFitness = std::accumulate(pop.begin(), pop.end(), 0.0, [](double sum, const Individual &individual)
                                                 { return sum + individual.fitness; }) /
                                 pop.size();
        std::cout << "Generation " << currentGeneration << " average population fitness: " << averageFitness << '\n';

        // Parent selection 
        std::cout << "Parent selection";
        Population parents = config.parentSelection.first(pop, config.parentSelection.second);
        // Average parent fitness
        double averageParentFitness = std::accumulate(parents.begin(), parents.end(), 0.0, [](double sum, const Individual &individual)
                                                        { return sum + individual.fitness; }) /
                                        parents.size();
        std::cout << "Generation " << currentGeneration << " average parent fitness: " << averageParentFitness << '\n';
        // Crossover
        std::cout << "Crossover";
        Population children = applyCrossover(parents, config.crossover, problemInstance);
        // Mutation
        std::cout << "Mutation";
        children = applyMutation(children, config.mutation, problemInstance);
        // log children fitness
        double averageChildrenFitness = std::accumulate(children.begin(), children.end(), 0.0, [](double sum, const Individual &individual) {
            return sum + individual.fitness;
        }) / children.size();
        std::cout << "Generation " << currentGeneration << " average children fitness: " << averageChildrenFitness << '\n';
        // Survivor selection
        std::cout << "Survivor selection";
        pop = config.survivorSelection.first(pop, children, config.survivorSelection.second);
        pop = sortPopulation(pop, false);
        std::cout << "Generation " << currentGeneration << "\nBest fitness: " << pop[0].fitness << "\nWorst fitness: " << pop[pop.size() - 1].fitness << '\n';
    }
    valid = isSolutionValid(pop[0].genome, problemInstance);
    if (valid) {
        std::cout << "The solution is valid" << '\n';
    }
    else {
        std::cout << "The solution is invalid" << '\n';
    }
    double totalTravelTime = getTotalTravelTime(pop[0].genome, problemInstance);
    if (totalTravelTime < problemInstance.benchmark) {
        std::cout << "The solution is " << problemInstance.benchmark - totalTravelTime << " time units better than the benchmark" << '\n';
    }
    else {
        std::cout << "The solution is " << totalTravelTime - problemInstance.benchmark << " time units worse than the benchmark" << '\n';
    }

    
}