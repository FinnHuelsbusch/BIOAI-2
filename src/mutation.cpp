#include "mutation.h"
#include "RandomGenerator.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include "utils.h"

auto reassignOnePatient(Genome &genome, const FunctionParameters &parameters) -> Genome
{

    RandomGenerator& rng = RandomGenerator::getInstance();
    int sourceNurse = rng.generateRandomInt(0, genome.size() - 1);
    // check that the source has at least one patient
    if (genome[sourceNurse].empty())
    {
        return reassignOnePatient(genome, parameters);
    }
    int destinationNurse ;
    do {
        destinationNurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (destinationNurse == sourceNurse);
    int patientIndex = rng.generateRandomInt(0, genome[sourceNurse].size() - 1);
    int patient = genome[sourceNurse][patientIndex];
    genome[sourceNurse].erase(genome[sourceNurse].begin() + patientIndex); 
    //randomly sample insertion point
    int insertionPoint;
    if (genome[destinationNurse].empty())
    {
        insertionPoint = 0;
    }
    else
    {
        insertionPoint = rng.generateRandomInt(0, genome[destinationNurse].size());
    }
    genome[destinationNurse].insert(genome[destinationNurse].begin() + insertionPoint, patient);
    return genome;
}

auto swapWithinJourney(Genome &genome, const FunctionParameters &parameters) -> Genome
{
    RandomGenerator& rng = RandomGenerator::getInstance();
    int nurse;
    do{
        nurse = rng.generateRandomInt(0, genome.size() - 1);
    }while(genome[nurse].size() < 2);
    int firstPatientIndex = rng.generateRandomInt(0, genome[nurse].size() - 1);
    int secondPatientIndex;
    do {
        secondPatientIndex = rng.generateRandomInt(0, genome[nurse].size() - 1);
    } while (secondPatientIndex == firstPatientIndex);
    std::swap(genome[nurse][firstPatientIndex], genome[nurse][secondPatientIndex]);
    return genome;
}

auto swapBetweenJourneys(Genome &genome, const FunctionParameters &parameters) -> Genome
{
    RandomGenerator& rng = RandomGenerator::getInstance();
    int sourceNurse;
    do {
        sourceNurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (genome[sourceNurse].empty());
    int destinationNurse;
    do {
        destinationNurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (destinationNurse == sourceNurse || genome[destinationNurse].empty());
    int patientIndexNurse1 = rng.generateRandomInt(0, genome[sourceNurse].size() - 1);
    int patientIndexNurse2 = rng.generateRandomInt(0, genome[destinationNurse].size() - 1);
    std::swap(genome[sourceNurse][patientIndexNurse1], genome[destinationNurse][patientIndexNurse2]);
    return genome;
}

auto insertWithinJourney(Genome &genome, const FunctionParameters &parameters) -> Genome
{
    auto logger = spdlog::get("main_logger");
    logger->trace("Starting insertWithinJourney mutation");
    RandomGenerator& rng = RandomGenerator::getInstance();
    int nurse;
    do {
        nurse = rng.generateRandomInt(0, genome.size() - 1);
    } while (genome[nurse].size() < 2);
    int patientIndex = rng.generateRandomInt(0, genome[nurse].size() - 1);
    int patient = genome[nurse][patientIndex];
    int insertionPoint;
    do{
        insertionPoint = rng.generateRandomInt(0, genome[nurse].size() - 1);
    }while(insertionPoint == patientIndex);
    logger->trace("Inserting patient {} from index {} at index {} in nurse {}", patient, patientIndex, insertionPoint, nurse);
    logger->trace("Trip before mutation: {}", fmt::join(genome[nurse], ", "));
    // insert patient at insertion point
    genome[nurse].erase(genome[nurse].begin() + patientIndex);
    genome[nurse].insert(genome[nurse].begin() + insertionPoint, patient);
    logger->trace("Trip after mutation: {}", fmt::join(genome[nurse], ", "));
    return genome;
}

auto twoOpt(Genome &genome, const FunctionParameters &parameters) -> Genome
{
    auto logger = spdlog::get("main_logger");
    logger->info("Starting 2-opt mutation");
    const ProblemInstance instance = std::get<ProblemInstance>(parameters.at("problem_instance"));
    RandomGenerator& rng = RandomGenerator::getInstance();
    int nurse;
    std::vector<int> nursesWithMoreThanFourPatients;
    for (int i = 0; i < genome.size(); i++) {
        if (genome[i].size() > 4) {
            nursesWithMoreThanFourPatients.push_back(i);
        }
    }
    if (nursesWithMoreThanFourPatients.empty()) {
        logger->warn("No nurse with more than 4 patients found --> no 2-opt possible --> returning original genome");
        return genome;
    }

    nurse = nursesWithMoreThanFourPatients[rng.generateRandomInt(0, nursesWithMoreThanFourPatients.size() - 1)];
    bool foundImprovement;
    do{
        foundImprovement = false;
        
        for (int i = -1; i <= static_cast<int>(genome[nurse].size() - 2); i++){
            for (int j = i+1; j < genome[nurse].size(); j++){
                Journey journey = genome[nurse];
                double oldCost; 
                double newCost;
                if (i == -1) {
                    if (j == genome[nurse].size() - 1) {
                        oldCost = instance.travelTime[0][journey[i+1]] + instance.travelTime[journey[j]][0];
                        newCost = instance.travelTime[0][journey[j]] + instance.travelTime[journey[i+1]][0];
                    } else {
                        oldCost = instance.travelTime[0][journey[i+1]] + instance.travelTime[journey[j]][journey[j+1]];
                        newCost = instance.travelTime[0][journey[j]] + instance.travelTime[journey[i+1]][journey[j+1]];
                    }
                } else if (j == genome[nurse].size() - 1) {
                    oldCost = instance.travelTime[journey[i]][journey[i+1]] + instance.travelTime[journey[j]][0];
                    newCost = instance.travelTime[journey[i]][journey[j]] + instance.travelTime[journey[i+1]][0];
                } else {
                    oldCost = instance.travelTime[journey[i]][journey[i+1]] + instance.travelTime[journey[j]][journey[j+1]];
                    newCost = instance.travelTime[journey[i]][journey[j]] + instance.travelTime[journey[i+1]][journey[j+1]];
                }     
                if (newCost < oldCost) {
                    foundImprovement = true;
                    std::reverse(journey.begin() + i + 1, journey.begin() + j + 1);
                    genome[nurse] = journey;
                }           
            }
        }
    }while(foundImprovement);
    return genome;
}