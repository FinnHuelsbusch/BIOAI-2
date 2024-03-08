#pragma once
#include <functional>
#include "structures.h"



auto reassignOnePatient(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto swapWithinJourney(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto swapBetweenJourneys(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto insertWithinJourney(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto twoOpt(Genome& genome, const FunctionParameters& parameters) -> Genome;
auto inverseJourney(Genome &genome, const FunctionParameters &parameters) -> Genome;
auto splitJourney(Genome &genome, const FunctionParameters &parameters) -> Genome;
auto insertionHeuristic(Genome& genome, const FunctionParameters &parameters) -> Genome;

// static final map of mutation functions
static const std::map<std::string, MutationFunction> mutationFunctions = {
    {"reassignOnePatient", reassignOnePatient},
    {"swapWithinJourney", swapWithinJourney},
    {"swapBetweenJourneys", swapBetweenJourneys},
    {"insertWithinJourney", insertWithinJourney},
    {"twoOpt", twoOpt},
    {"inverseJourney", inverseJourney},
    {"splitJourney", splitJourney},
    {"insertionHeuristic", insertionHeuristic}
};