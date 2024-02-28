#include "structures.h"
#include <algorithm>
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream> 
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h> 



using json = nlohmann::json;


auto getTotalTravelTime(const Genome &genome, const ProblemInstance &problemInstance) -> double
{
    double totalTravelTime = 0;
    for (Journey nurseJourney : genome)
    {
        for (int i = 0; i < nurseJourney.size(); i++)
        {
            int patientId = nurseJourney[i];
            int previousPatientId = nurseJourney[i - 1];
            if (i == 0)
            {
                totalTravelTime += problemInstance.travelTime[0][patientId];
            }
            else
            {
                totalTravelTime += problemInstance.travelTime[previousPatientId][patientId];
            }
        }
        // add the driving time from the last patient to the depot if there is at least one patient
        if (!nurseJourney.empty())
        {
            totalTravelTime += problemInstance.travelTime[nurseJourney[nurseJourney.size() - 1]][0];
        }
    }
    return totalTravelTime;
}

auto sortPopulationByFitness(Population& population, bool ascending) -> void
{
    if (ascending) {
        std::sort(population.begin(), population.end(), [](const Individual& individualA, const Individual& individualB) {
            return individualA.fitness < individualB.fitness;
        });
    }
    else {
        std::sort(population.begin(), population.end(), [](const Individual& individualA, const Individual& individualB) {
            return individualA.fitness > individualB.fitness;
        });
    }
}

auto sortPopulationByTravelTime(Population& population, bool ascending, const ProblemInstance &problemInstance) -> void
{
    if (ascending) {
        std::sort(population.begin(), population.end(), [&problemInstance](const Individual& individualA, const Individual& individualB) {
            return getTotalTravelTime(individualA.genome, problemInstance) > getTotalTravelTime(individualB.genome, problemInstance);
        });
    }
    else {
        std::sort(population.begin(), population.end(), [&problemInstance](const Individual& individualA, const Individual& individualB) {
            return getTotalTravelTime(individualA.genome, problemInstance) < getTotalTravelTime(individualB.genome, problemInstance);
        });
    }
}

auto logGenome(const Genome& genome, const std::string& IndividualName, const int generation) -> void {
    auto logger = spdlog::get("statistics_logger");
    std::string genomeString = "Genome: Name: " + IndividualName + " Generation: " + std::to_string(generation) + " Genome: [";
    for (int i = 0; i < genome.size(); i++) {
        genomeString += "[";
        for (int patientID : genome[i])
        {
            genomeString += std::to_string(patientID) + ", ";
        }
        genomeString += "], ";
    }
    genomeString += "]";
    logger->debug(genomeString);

}

auto flattenGenome(const Genome &genome) -> std::vector<int>
{
    std::vector<int> flatGenome;
    for (const Journey &journey : genome)
    {
        for (int patientID : journey)
        {
            flatGenome.push_back(patientID);
        }
    }
    return flatGenome;
}

auto unflattenGenome(std::vector<int> flatGenome, const Genome &parent) -> Genome
{
    Genome unflattenedGenome;
    unflattenedGenome.reserve(parent.size());
    int index = 0;
    for (const auto &parentJourney : parent)
    {
        Journey journey;
        journey.reserve(parentJourney.size());
        for (int j = 0; j < parentJourney.size(); j++)
        {
            journey.push_back(flatGenome[index]);
            index++;
        }
        unflattenedGenome.push_back(journey);
    }
    return unflattenedGenome;
}


auto exportIndividual(const Individual& individual, const std::string& path) -> void
{
    json individualJson;
    individualJson["fitness"] = individual.fitness;
    individualJson["genome"] = individual.genome;
    // write to file
    std::ofstream outputFileStream(path); // Create an instance of std::ofstream
    outputFileStream << individualJson.dump(4);
    outputFileStream.close();
}

auto isJourneyValid(const Journey &nurseJourney, const ProblemInstance &problemInstance) -> bool
{
    auto logger = spdlog::get("main_logger");
    if (nurseJourney.empty())
    {
        return true;
    }
    double totalTimeSpent = 0.0;
    int totalDemand = 0;
    for (int j = 0; j < nurseJourney.size(); j++)
    {
        int patientId = nurseJourney[j];
        int previousPatientId = nurseJourney[j - 1];
        if (j == 0)
        {
            totalTimeSpent += problemInstance.travelTime[0][patientId];
        }
        else
        {
            totalTimeSpent += problemInstance.travelTime[previousPatientId][patientId];
        }
        if (totalTimeSpent < problemInstance.patients.at(patientId).startTime)
        {
            totalTimeSpent = problemInstance.patients.at(patientId).startTime;
        }
        totalTimeSpent += problemInstance.patients.at(patientId).careTime;
        if (totalTimeSpent > problemInstance.patients.at(patientId).endTime)
        {
            logger->trace("Patient {} treatment finishes too late", patientId);
            return false;
        }
        totalDemand += problemInstance.patients.at(patientId).demand;
        if (totalDemand > problemInstance.nurseCapacity)
        {
            logger->trace("Nurse exceeds the capacity");
            return false;
        }
    }
    // add the driving time from the last patient to the depot if there is at least one patient
    if (nurseJourney.empty())
    {
        totalTimeSpent += problemInstance.travelTime[nurseJourney[nurseJourney.size() - 1]][0];
    }
    if (totalTimeSpent > problemInstance.depot.returnTime)
    {
        logger->trace("Nurse exceeds the return time");
        return false;
    }
    return true;
}

auto isSolutionValid(const Genome &genome, const ProblemInstance &problemInstance) -> bool
{
    auto logger = spdlog::get("main_logger");
    // validate that every patient is visited exactly once
    std::map<int, bool> visitedPatients;
    for (const Journey &nurseJourney : genome)
    {
        for (int patientId : nurseJourney)
        {
            if (visitedPatients[patientId])
            {
                logger->debug("Patient {} is visited more than once", patientId);
                return false;
            }
            visitedPatients[patientId] = true;
        }
    }
    // validate that every patient is visited
    for (const auto &[id, patient] : problemInstance.patients)
    {
        if (!visitedPatients[id])
        {
            logger->debug("Patient {} is not visited", id);
            return false;
        }
    }

    for (int nurseId = 0; nurseId < genome.size(); nurseId++)
    {
        const Journey &nurseJourney = genome[nurseId];
        if (!isJourneyValid(nurseJourney, problemInstance))
        {
            logger->trace("Journey of nurse {} is invalid", nurseId);
            return false;
        }
    }
    return true;
}

auto initLogger() -> void
{
        std::string main_log_file_path = "logfile.txt";
    std::string statistics_log_file_path = "statistics.txt";

    // Check if log files exist, and if so, remove them
    if (std::filesystem::exists(main_log_file_path)) {
        std::filesystem::remove(main_log_file_path);
    }

    if (std::filesystem::exists(statistics_log_file_path)) {
        std::filesystem::remove(statistics_log_file_path);
    }

    // Create a basic file sink
    auto main_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(main_log_file_path);
    auto statistics_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(statistics_log_file_path);


    // Create a logger with the basic file sink
    auto logger = std::make_shared<spdlog::logger>("main_logger", main_file_sink);
    auto statistics_logger = std::make_shared<spdlog::logger>("statistics_logger", statistics_file_sink);

    // Set the logging pattern with the thread id
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");
    statistics_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");


    #if defined(PRODUCTION)
        logger->set_level(spdlog::level::debug);
        statistics_logger->set_level(spdlog::level::debug);
    #else
        logger->set_level(spdlog::level::trace);
        statistics_logger->set_level(spdlog::level::debug);
    #endif

    // Register the logger
    spdlog::register_logger(logger);
    spdlog::register_logger(statistics_logger);
}