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

auto inverseJourney(Genome &genome, const FunctionParameters &parameters) -> Genome{
    auto logger = spdlog::get("main_logger");
    logger->trace("Starting inverseJourney mutation");
    RandomGenerator& rng = RandomGenerator::getInstance();
    int nurse;
    do{
        nurse = rng.generateRandomInt(0, genome.size() - 1);
    }while(genome[nurse].size() < 2);
    int start = rng.generateRandomInt(0, genome[nurse].size() - 2);
    int end;
    do{
        end = rng.generateRandomInt(start + 1, genome[nurse].size() - 1);
    }while(end == start);
    std::reverse(genome[nurse].begin() + start, genome[nurse].begin() + end + 1);
    return genome;
}

auto splitJourney(Genome &genome, const FunctionParameters &parameters) -> Genome{
    auto logger = spdlog::get("main_logger");
    logger->trace("Starting splitJourney mutation");
    RandomGenerator& rng = RandomGenerator::getInstance();
    int destinationNurse = -1;
    for (int i = 0; i < genome.size(); i++){
        if (genome[i].size() == 0){
            destinationNurse = i;
            break;
        }
    }
    if (destinationNurse == -1){
        logger->info("No nurse with 0 patients found --> returning original genome");
        return genome;
    }
    int sourceNurse;
    do{
        sourceNurse = rng.generateRandomInt(0, genome.size() - 1);
    }while(genome[sourceNurse].size() < 2);
    // split at the longest travel time
    double longestTravelTime = -1; 
    int splitIndex = -1;
    ProblemInstance problemInstance = std::get<ProblemInstance>(parameters.at("problem_instance"));
    for (int i = 0; i < genome[sourceNurse].size() - 1; i++){
        double travelTime = problemInstance.travelTime[genome[sourceNurse][i]][genome[sourceNurse][i+1]];
        if (travelTime > longestTravelTime){
            longestTravelTime = travelTime;
            splitIndex = i;
        }
    }
    Journey newJourney;
    newJourney.insert(newJourney.begin(), genome[sourceNurse].begin() + splitIndex + 1, genome[sourceNurse].end());
    genome[destinationNurse] = newJourney;
    genome[sourceNurse].erase(genome[sourceNurse].begin() + splitIndex + 1, genome[sourceNurse].end());
    return genome;
}

auto validateJourneyIfPatientIsInserted(const Journey& journey, const int patientID, const uint insertionPoint, const ProblemInstance &problemInstance) -> bool
{
    if(journey.empty()){
        return true;
    }
    // insert patient index at insertion point
    Journey journeyCopy = journey;
    journeyCopy.insert(journeyCopy.begin() + insertionPoint, patientID);
    return isJourneyValid(journeyCopy, problemInstance);
}

auto insertionHeuristic(Genome& genome, const FunctionParameters &parameters) -> Genome
{

    // get ProblemInstanceFrom parameters
    if (parameters.find("problem_instance") == parameters.end())
    {
        throw std::invalid_argument("insertionHeuristic requires 'problem_instance'");
    }

    ProblemInstance problemInstance = std::get<ProblemInstance>(parameters.at("problem_instance"));

    std::vector<int> flattendGenome = flattenGenome(genome);

    // init empty Genome
    Genome newGenome = std::vector<std::vector<int>>(problemInstance.numberOfNurses);

    for(int patientID : flattendGenome){
        uint minDetour = UINT_MAX;
        uint minDetourIndex = UINT_MAX;
        uint nurseID = UINT_MAX; 
        // calculate minimum detour for each nurse 
        for(int i = 0; i < genome.size(); i++){
            Journey journey = newGenome[i];
            if(journey.empty()){
                minDetour = problemInstance.travelTime[0][patientID] + problemInstance.travelTime[patientID][0];
                minDetourIndex = 0;
                nurseID = i; 
            }else{
                int detour; 
                // calculate detour if patient is inserted between first patient and depot
                detour = problemInstance.travelTime[0][patientID] + problemInstance.travelTime[patientID][journey[0]] - problemInstance.travelTime[0][journey[0]];
                if(detour < minDetour && validateJourneyIfPatientIsInserted(journey, patientID, 0, problemInstance)){
                    minDetour = detour;
                    minDetourIndex = 0;
                }
                // calculate detour between patients the trip back to the depot is not considered
                for (int j = 0; j < journey.size() - 1; j++){
                    int detour = problemInstance.travelTime[journey[j]][patientID] + problemInstance.travelTime[patientID][journey[j+1]] - problemInstance.travelTime[journey[j]][journey[j+1]];
                    if(detour < minDetour && validateJourneyIfPatientIsInserted(journey, patientID, j+1, problemInstance)){
                        minDetour = detour;
                        minDetourIndex = j + 1;
                        nurseID = i; 
                    }
                }
                // calculate detour if patient is inserted between last patient and depot
                detour = problemInstance.travelTime[journey[journey.size() - 1]][patientID] + problemInstance.travelTime[patientID][0] - problemInstance.travelTime[journey[journey.size() - 1]][0];
                if(detour < minDetour && validateJourneyIfPatientIsInserted(journey, patientID, journey.size(), problemInstance)){
                    minDetour = detour;
                    minDetourIndex = journey.size();
                    nurseID = i; 
                }
            }
        }
        if(minDetour == UINT_MAX || minDetourIndex == UINT_MAX || nurseID == UINT_MAX){
            auto main_logger = spdlog::get("main_logger");
            // TODO: better logging
            main_logger->warn("No valid insert found returning original genome");
            return genome;
        }
        // insert patient at selected position
        newGenome[nurseID].insert(newGenome[nurseID].begin() + minDetourIndex, patientID);
    }
    return newGenome;
}
