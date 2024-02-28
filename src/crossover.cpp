#include "crossover.h"

#include <algorithm>
#include "RandomGenerator.h"
#include "utils.h"
#include <cassert>
#include <set>
#include <climits>

auto order1Crossover(const Genome &parent1, const Genome &parent2) -> std::pair<Genome, std::optional<Genome>>
{ 

    std::vector<int> parent1Flat = flattenGenome(parent1);
    std::vector<int> parent2Flat = flattenGenome(parent2);
    // assert that the genomes have the same length and are longer than 1
    assert(parent1Flat.size() == parent2Flat.size());
    assert(parent1Flat.size() > 1);

    RandomGenerator& rng = RandomGenerator::getInstance();
    int start = rng.generateRandomInt(0, parent1Flat.size() - 1);
    int end = rng.generateRandomInt(0, parent1Flat.size() - 1);
    if (start > end) {
        std::swap(start, end);
    }
    std::vector<int> child1Flat = std::vector<int>(parent1Flat.size());
    std::vector<int> child2Flat = std::vector<int>(parent2Flat.size());

    // copy the selected part from parent1 to child1 and the selected part from parent2 to child2
    for (int i = start; i <= end; i++) {
        child1Flat[i] = parent1Flat[i];
        child2Flat[i] = parent2Flat[i];
    }

    // fill the rest of the child with the remaining genes from the other parent
    for (int i = 1; i < parent1Flat.size() - (end - start); i++) {
        int sourceIndex = (end + i) % parent1Flat.size();
        int targetIndex = sourceIndex; 
        while (std::find(child1Flat.begin(), child1Flat.end(), parent2Flat[targetIndex]) != child1Flat.end()) {
            targetIndex = (targetIndex + 1) % parent1Flat.size();
        }
        child1Flat[sourceIndex] = parent2Flat[targetIndex];
    }
    
    for (int i = 1; i < parent2Flat.size() - (end - start); i++) {
        int sourceIndex = (end + i) % parent2Flat.size();
        int targetIndex = sourceIndex; 
        while (std::find(child2Flat.begin(), child2Flat.end(), parent1Flat[targetIndex]) != child2Flat.end()) {
            targetIndex = (targetIndex + 1) % parent2Flat.size();
        }
        child2Flat[sourceIndex] = parent1Flat[targetIndex];
    }
    Genome child1 = unflattenGenome(child1Flat, parent1);
    Genome child2 = unflattenGenome(child2Flat, parent2);
    return std::make_pair(child1, child2);
}

auto partiallyMappedCrossover(const Genome &parent1, const Genome &parent2) -> std::pair<Genome, std::optional<Genome>>
{
    std::vector<int> parent1Flat = flattenGenome(parent1);
    std::vector<int> parent2Flat = flattenGenome(parent2);

    RandomGenerator& rng = RandomGenerator::getInstance();
    std::size_t start = rng.generateRandomInt(0, parent1Flat.size() - 1);
    std::size_t end = rng.generateRandomInt(0, parent1Flat.size() - 1);
    if (start > end) {
        std::swap(start, end);
    }
    std::vector<int> child1Flat = std::vector<int>(parent1Flat.size(), -1);
    std::vector<int> child2Flat = std::vector<int>(parent2Flat.size(), -1);

    // copy the selected part from parent1 to child1 and the selected part from parent2 to child2
    std::vector<int> previousIndices;
    for (int i = start; i <= end; i++) {
        child1Flat[i] = parent1Flat[i];
        child2Flat[i] = parent2Flat[i];
    }

    for (int i = start; i <= end; i++) {
        int index = i; 
        previousIndices.clear();
        // check if the value is already in the selected part
        if (std::find(child1Flat.begin(), child1Flat.end(), parent2Flat[i]) != child1Flat.end()) {          
            // the value is already in the selected part
            continue;
        }
        do {
            previousIndices.push_back(index);
            auto iterator = std::find(parent2Flat.begin(), parent2Flat.end(), parent1Flat[index]);
            index = iterator - parent2Flat.begin();
        } while ((start <= index && index <= end && std::find(previousIndices.begin(), previousIndices.end(), index) == previousIndices.end()) || child1Flat[index] != -1);
        
        child1Flat[index] = parent2Flat[i];
    }

    for (int i = start; i <= end; i++) {
        
        int index = i;
        previousIndices.clear();
        if (std::find(child2Flat.begin(), child2Flat.end(), parent1Flat[i]) != child2Flat.end()) {
            continue;
        }
        do {
            previousIndices.push_back(index);
            auto iterator = std::find(parent1Flat.begin(), parent1Flat.end(), parent2Flat[index]);
            index = iterator - parent1Flat.begin();
        } while ((start <= index && index <= end && std::find(previousIndices.begin(), previousIndices.end(), index) == previousIndices.end()) || child2Flat[index] != -1);
        child2Flat[index] = parent1Flat[i];
        
    }

    // fill the rest of the child with the remaining genes from the other parent
    for (int i = 1; i < parent1Flat.size(); i++) {
        int index = (i + end) % parent1Flat.size();
        // check if the value is undefined
        if (child1Flat[index] == -1) {
            int targetIndex = index; 
            while (std::find(child1Flat.begin(), child1Flat.end(), parent2Flat[targetIndex]) != child1Flat.end()) {
                targetIndex = (targetIndex + 1) % parent1Flat.size();
            }
            child1Flat[index] = parent2Flat[targetIndex];
        }

        if (child2Flat[index] == -1) {
            int targetIndex = index; 
            while (std::find(child2Flat.begin(), child2Flat.end(), parent1Flat[targetIndex]) != child2Flat.end()) {
                targetIndex = (targetIndex + 1) % parent2Flat.size();
            }
            child2Flat[index] = parent1Flat[targetIndex];
        }
    }

    Genome child1 = unflattenGenome(child1Flat, parent1); 
    Genome child2 = unflattenGenome(child2Flat, parent2);
    return std::make_pair(child1, child2);
}

auto edgeRecombination(const Genome &parent1, const Genome &parent2) -> std::pair<Genome, std::optional<Genome>>
{
    std::vector<int> parent1Flat = flattenGenome(parent1);
    std::vector<int> parent2Flat = flattenGenome(parent2);
    // assert that the genomes have the same length
    assert(parent1Flat.size() == parent2Flat.size());

    std::map<int, std::vector<int>> adjacencyList;
    for (int patientID : parent1Flat)
    {
        adjacencyList[patientID] = std::vector<int>();
    }
    for (int i = 0; i < parent1Flat.size(); i++) {
        int left = (i - 1 + parent1Flat.size()) % parent1Flat.size();
        int right = (i + 1) % parent1Flat.size();
        adjacencyList[parent1Flat[i]].push_back(parent1Flat[left]);
        adjacencyList[parent1Flat[i]].push_back(parent1Flat[right]);
        adjacencyList[parent2Flat[i]].push_back(parent2Flat[left]);
        adjacencyList[parent2Flat[i]].push_back(parent2Flat[right]);
    }

    RandomGenerator& rng = RandomGenerator::getInstance();
    std::vector<int> child;
    int current = parent1Flat[rng.generateRandomInt(0, parent1Flat.size() - 1)];
    for (int i = 0; i < parent1Flat.size(); i++) {
        child.push_back(current);
        for (auto& [key, value] : adjacencyList) {
            value.erase(std::remove(value.begin(), value.end(), current), value.end());
        }
        
        //Examine list for current element:
            // – If there is a common edge, pick that to be next element
            // – Otherwise pick the entry in the list which itself has the shortest list
            // – Ties are split at random
        int newCurrent = INT_MAX;
        std::set<int> seen = std::set<int>();
        for (int value : adjacencyList[current]) {
            if (seen.contains(value)) {
                newCurrent = value;
                break;
            }
            seen.insert(value);
        }

        // choice of new current is not random if there are two list of equal length. 
        if (newCurrent == INT_MAX) {
            int minSize = INT_MAX;
            for (int key : adjacencyList[current]) {
                std::set<int> valueSet = std::set<int>(adjacencyList[key].begin(), adjacencyList[key].end());
                if (valueSet.size() <= minSize) {
                    minSize = valueSet.size();
                    newCurrent = key;
                }
            }
        }
        adjacencyList.erase(current);
        if (adjacencyList.empty()) {
            break;
        }
        if (newCurrent == INT_MAX) {
            do{
                newCurrent = parent1Flat[rng.generateRandomInt(0, parent1Flat.size() - 1)];
            } while (adjacencyList.find(newCurrent) == adjacencyList.end());
        }
        current = newCurrent;
    }
    return std::make_pair(unflattenGenome(child, parent1), std::nullopt);
}
