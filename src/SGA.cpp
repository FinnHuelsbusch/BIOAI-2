#include "SGA.h"
#include "structures.h"
#include "utils.h"
#include "RandomGenerator.h"
#include <iostream>
#include <climits>
#include <spdlog/spdlog.h>



auto evaluateIndividual(Individual *individual, const ProblemInstance &problemInstance) -> void
{
    double combinedTripTime = 0;
    double missingCareTimePenality = 0;
    double capacityPenality = 0;
    double toLateToDepotPenality = 0;

    const auto &travelTime = problemInstance.travelTime;

    for (const Journey &nurseJourney : individual->genome)
    {
        double nurseTripTime = 0;
        double nurseTravelTime = 0;
        int nurseUsedCapacity = 0;

        for (std::size_t j = 0; j < nurseJourney.size(); j++)
        {

            // Accumulate trip time. From Depot to first patient and after that from patient to patient. (Applies when patientID == index in travel matrix)
            int patientId = nurseJourney[j];
            if (j == 0)
            {
                nurseTripTime += travelTime[0][patientId];
                nurseTravelTime += travelTime[0][patientId];
            }
            else
            {
                nurseTripTime += travelTime[nurseJourney[j - 1]][patientId];
                nurseTravelTime += travelTime[nurseJourney[j - 1]][patientId];
            }

            // If the triptime to the patient is lower than his time window, wait to the start ot the timewindow
            nurseTripTime = std::max(nurseTripTime, static_cast<double>(problemInstance.patients.at(patientId).startTime));

            // Nurse is caring for the patient
            nurseTripTime += problemInstance.patients.at(patientId).careTime;

            // If the nurse is arriving to late we may not have enough time to care for the patient in the time window.
            if (nurseTripTime > problemInstance.patients.at(patientId).endTime)
            {
                // Use the missed caretime as a penality
                missingCareTimePenality = nurseTripTime - problemInstance.patients.at(patientId).endTime;
            }

            // We have now cared for the patient. Add the demand to our used capacity.
            // We dont care if we are outside of time windows. The important part ist the capacity of a trip overall
            nurseUsedCapacity += problemInstance.patients.at(patientId).demand;

            // Capacity penality
            if (nurseUsedCapacity > problemInstance.nurseCapacity)
            {
                capacityPenality = (nurseUsedCapacity - problemInstance.nurseCapacity);
            }
        }

        // add the driving time from the last patient to the depot if there is at least one patient
        if (!nurseJourney.empty())
        {
            nurseTripTime += travelTime[nurseJourney.back()][0];
            nurseTravelTime += travelTime[nurseJourney.back()][0];
        }

        // add penality if we are too late to the depot
        toLateToDepotPenality = std::max(0.0, nurseTripTime - problemInstance.depot.returnTime);
        combinedTripTime += nurseTravelTime;
    }

    double fitness = -combinedTripTime - capacityPenality * 100000 - missingCareTimePenality * 10000 - toLateToDepotPenality * 10000;

    individual->fitness = fitness;
    individual->capacityPenality = capacityPenality;
    individual->missingCareTimePenality = missingCareTimePenality;
    individual->toLateToDepotPenality = toLateToDepotPenality;
}

auto initializeRandomPopulation(const ProblemInstance &problemInstance, const Config &config) -> Population
{
    Population pop = std::vector<Individual>();
    pop.reserve(config.populationSize);
    // Seed the random number generator
    RandomGenerator &rng = RandomGenerator::getInstance();
    for (int i = 0; i < config.populationSize; i++)
    {
        // Generate patient
        std::vector<int> PatientIDs = std::vector<int>();
        PatientIDs.reserve(problemInstance.patients.size());

        for (std::pair<int, Patient> PatientID_Patient : problemInstance.patients)
        {
            PatientIDs.push_back(PatientID_Patient.first);
        }
        rng.shuffle(PatientIDs);

        // Distribute the patients over the nurses

        Genome genome = std::vector<std::vector<int>>(problemInstance.numberOfNurses);

        // pop from PatientIDs until empty
        for (int i = 0; i < problemInstance.patients.size(); i++)
        {
            int patientID = PatientIDs[i];
            if (config.initialPopulationDistirbutePatientsEqually)
            {
                int nurseId = i % problemInstance.numberOfNurses;
                genome[nurseId].push_back(patientID);
            }
            else
            {
                int nurseId = rng.generateRandomInt(0, problemInstance.numberOfNurses - 1);
                genome[nurseId].push_back(patientID);
            }
        }
        // Create the Individual
        Individual individual = {genome};
        evaluateIndividual(&individual, problemInstance);
        pop.push_back(individual);
    }
    return pop;
}

auto initializeFeasiblePopulation(const ProblemInstance &problemInstance, const Config &config) -> Population
{
    Population pop = std::vector<Individual>();
    pop.reserve(config.populationSize);
    // Seed the random number generator
    RandomGenerator &rng = RandomGenerator::getInstance();

    std::map<int, std::vector<const Patient *>> PatientsByEndTime;
    for (const auto &[id, patient] : problemInstance.patients)
    {
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

    for (int i = 0; i < config.populationSize; i++)
    {
        // copy the  PatientsByEndTime
        std::map<int, std::vector<const Patient *>> PatientsByEndTimeCopy(PatientsByEndTime.begin(), PatientsByEndTime.end());
        // init genome with number of nurses x empty vector
        Genome genome = std::vector<std::vector<int>>(problemInstance.numberOfNurses);
        int currentStartTimeIndex = 0;
        int currentStartTime = startTimes[currentStartTimeIndex];
        int index;

        for (int j = 0; j < problemInstance.patients.size(); j++)
        {

            index = rng.generateRandomInt(0, PatientsByEndTimeCopy[currentStartTime].size() - 1);
            const Patient *patient = PatientsByEndTimeCopy[currentStartTime][index];
            PatientsByEndTimeCopy[currentStartTime].erase(PatientsByEndTimeCopy[currentStartTime].begin() + index);
            // insert patient in genome
            int minDetour = INT_MAX;
            int minDetourIndex = -1;
            for (int k = 0; k < genome.size(); k++)
            {
                if (genome[k].empty())
                {
                    minDetourIndex = k;
                    minDetour = problemInstance.travelTime[k][patient->id] + problemInstance.travelTime[patient->id][0];
                }
                else
                {
                    genome[k].push_back(patient->id);
                    if (isJourneyValid(genome[k], problemInstance))
                    {
                        int detour = problemInstance.travelTime[k - 1][patient->id] + problemInstance.travelTime[patient->id][0] - problemInstance.travelTime[k - 1][0];
                        if (detour < minDetour)
                        {
                            minDetour = detour;
                            minDetourIndex = k;
                        }
                    }
                    genome[k].pop_back();
                }
            }
            if (minDetourIndex != -1)
            {
                genome[minDetourIndex].push_back(patient->id);
            }
            else
            {
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
        if (isSolutionValid(genome, problemInstance))
        {
            Individual individual = {genome};
            evaluateIndividual(&individual, problemInstance);
            pop.push_back(individual);
        }
        else
        {
            i--;
        }
    }
    return pop;
}

auto applyCrossover(Population &parents, CrossoverConfiguration &crossover, ProblemInstance &problemInstance) -> Population
{
    Population children = std::vector<Individual>();
    children.reserve(parents.size());
    for (const Individual &parent : parents)
    {
        children.push_back(parent);
    }
    RandomGenerator &rng = RandomGenerator::getInstance();

    for (std::pair<CrossoverFunction, double> crossoverPair : crossover)
    {
        CrossoverFunction CrossoverFunction = crossoverPair.first;
        int numberOfCrossovers = std::ceil(parents.size() * crossoverPair.second);

        for (int i = 0; i < numberOfCrossovers; i++)
        {
            int individualIndex1 = rng.generateRandomInt(0, parents.size() - 1);
            int individualIndex2 = rng.generateRandomInt(0, parents.size() - 1);
            while (individualIndex1 == individualIndex2)
            {
                individualIndex2 = rng.generateRandomInt(0, parents.size() - 1);
            }
            Individual individual1 = parents[individualIndex1];
            Individual individual2 = parents[individualIndex2];
            std::pair<Genome, std::optional<Genome>> childrenGenomes = CrossoverFunction(individual1.genome, individual2.genome);
            Individual child1 = {childrenGenomes.first};
            evaluateIndividual(&child1, problemInstance);
            children[individualIndex1] = child1;
            if (childrenGenomes.second.has_value())
            {
                Individual child2 = {childrenGenomes.second.value()};
                evaluateIndividual(&child2, problemInstance);
                children[individualIndex2] = child2;
            }
        }
    }
    return children;
}

auto applyMutation(Population &population, MuationConfiguration &mutation, ProblemInstance &problemInstance) -> Population
{
    auto main_logger = spdlog::get("main_logger");
    RandomGenerator &rng = RandomGenerator::getInstance();
    for (std::tuple<MutationFunction, FunctionParameters, double> mutationTuple : mutation)
    {
        MutationFunction MutationFunction = std::get<0>(mutationTuple);
        FunctionParameters parameters = std::get<1>(mutationTuple);
        double mutationRate = std::get<2>(mutationTuple);
        int numberOfMutations = std::ceil(population.size() * mutationRate);
        for (int i = 0; i < numberOfMutations; i++)
        {
            int individualIndex = rng.generateRandomInt(0, population.size() - 1);
            Individual individual = population[individualIndex];
            main_logger->trace("Applying mutation to individual {}", individualIndex);
            main_logger->trace("Genome before mutation: {}", fmt::join(flattenGenome(individual.genome), ", "));
            main_logger->trace("Is genome valid: {}", isSolutionValid(individual.genome, problemInstance));
            Genome mutatedGenome = MutationFunction(individual.genome, parameters);
            Individual mutatedIndividual = {mutatedGenome};
            main_logger->trace("Genome after mutation: {}", fmt::join(flattenGenome(individual.genome), ", "));
            main_logger->trace("Is mutated genome valid: {}", isSolutionValid(mutatedIndividual.genome, problemInstance));
            evaluateIndividual(&mutatedIndividual, problemInstance);
            population[individualIndex] = mutatedIndividual;
        }
    }
    return population;
}

Individual SGA(ProblemInstance problemInstance, Config config)
{
    auto main_logger = spdlog::get("main_logger");
    main_logger->info("Starting the SGA");
    auto statistics_logger = spdlog::get("statistics_logger");
    const  int populationSize = config.populationSize;

    Population pop = initializeFeasiblePopulation(problemInstance, config);
    //Population pop = initializeRandomPopulation(problemInstance, config);
    main_logger->info("Population initialized");
    sortPopulationByFitness(pop, false);
    logGenome(pop[0].genome, "Best", 0);
    //  check if population only contains valid solutions
    bool valid = std::all_of(pop.begin(), pop.end(), [&](const Individual &individual)
                             { return isSolutionValid(individual.genome, problemInstance); });
    if (valid)
    {
        main_logger->info("The initial population only contains valid solutions");
        std::cout << "The initial population only contains valid solutions" << '\n';
    }
    else
    {
        main_logger->info("The initial population contains invalid solutions");
        std::cout << "The initial population contains invalid solutions" << '\n';
    }
    for (int currentGeneration = 0; currentGeneration < config.numberOfGenerations; currentGeneration++)
    {
        main_logger->info("Generation: {}", currentGeneration);
        statistics_logger->info("Generation: {}", currentGeneration);
        std::cout << "Generation: " << currentGeneration << '\n';

        // Parent selection
        std::cout << "SEL|";
        Population parents = config.parentSelection.first(pop, config.parentSelection.second, populationSize);

        // Crossover
        std::cout << "CROSS|";
        Population children = applyCrossover(parents, config.crossover, problemInstance);
        // Mutation
        std::cout << "MUT|";
        children = applyMutation(children, config.mutation, problemInstance);

        // Survivor selection
        std::cout << "SURV_SEL" << '\n';
        pop = config.survivorSelection.first(pop, children, config.survivorSelection.second, populationSize);

        // calculate percentage of valid solutions
        int validSolutions = std::count_if(pop.begin(), pop.end(), [&](const Individual &individual)
                                          { return isSolutionValid(individual.genome, problemInstance); });
        double percentageValid = (validSolutions / static_cast<double>(pop.size())) * 100;
        main_logger->info("Percentage of valid solutions: {}", percentageValid);
        statistics_logger->info("Percentage of valid solutions: {}", percentageValid);

        // Average fitness
        sortPopulationByTravelTime(pop, false, problemInstance);
        double averageTravelTime = std::accumulate(pop.begin(), pop.end(), 0.0, [problemInstance](double sum, const Individual &individual)
                                                   { return sum + getTotalTravelTime(individual.genome, problemInstance); }) /
                                   pop.size();
        main_logger->info("Travel Time Best: {} Avg: {} Worst: {}", getTotalTravelTime(pop[0].genome, problemInstance), averageTravelTime, getTotalTravelTime(pop[pop.size() - 1].genome, problemInstance));
        statistics_logger->info("Travel Time Best: {} Avg: {} Worst: {}", getTotalTravelTime(pop[0].genome, problemInstance), averageTravelTime, getTotalTravelTime(pop[pop.size() - 1].genome, problemInstance));
        std::cout << "Travel Time Best: " << getTotalTravelTime(pop[0].genome, problemInstance) << " Avg: " << averageTravelTime << " Worst: " << getTotalTravelTime(pop[pop.size() - 1].genome, problemInstance) << " Percentage of valid solutions: " << percentageValid << '\n';

        // Genome logging: 
        // Log the Genome of the fastest individual
        logGenome(pop[0].genome, "Fastest", currentGeneration);
        // Log the Genome of the fittest individual
        sortPopulationByFitness(pop, false);
        logGenome(pop[0].genome, "Fittest", currentGeneration);

        // Log the fitness of the best, average and worst individual
        double averageFitness = std::accumulate(pop.begin(), pop.end(), 0.0, [](double sum, const Individual &individual)
                                                { return sum + individual.fitness; }) /
                                pop.size();
        main_logger->info("Fitness Best: {} Avg: {} Worst: {}", pop[0].fitness, averageFitness, pop[pop.size() - 1].fitness);
        statistics_logger->info("Fitness Best: {} Avg: {} Worst: {}", pop[0].fitness, averageFitness, pop[pop.size() - 1].fitness);
        
        main_logger->flush();
    }
    sortPopulationByFitness(pop, false);
    valid = isSolutionValid(pop[0].genome, problemInstance);

    double totalTravelTime = getTotalTravelTime(pop[0].genome, problemInstance);
    if(valid)
    {
        main_logger->info("The solution is valid and fullfills {}% of the benchmark", (problemInstance.benchmark / totalTravelTime) * 100);
        std::cout << "The solution is valid and fullfills " << ((problemInstance.benchmark / totalTravelTime) * 100) << "% of the benchmark" << '\n';
    }
    else
    {
        main_logger->info("The solution is invalid and fullfills {}% of the benchmark", (problemInstance.benchmark / totalTravelTime) * 100);
        std::cout << "The solution is invalid and fullfills " << ((problemInstance.benchmark / totalTravelTime) * 100) << "% of the benchmark" << '\n';
    }
    

    return pop[0];
}