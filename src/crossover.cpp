#include "crossover.h"

#include <algorithm>
#include "RandomGenerator.h"
#include "utils.h"
#include <cassert>
#include <set>
#include <climits>

auto order1Crossover(const Genome &parent1, const Genome &parent2) -> std::pair<Genome, std::optional<Genome>>
{ 

    std::vector<int> parent1_flat = flatten_genome(parent1);
    std::vector<int> parent2_flat = flatten_genome(parent2);
    // assert that the genomes have the same length and are longer than 1
    assert(parent1_flat.size() == parent2_flat.size());
    assert(parent1_flat.size() > 1);

    RandomGenerator& rng = RandomGenerator::getInstance();
    int start = rng.generateRandomInt(0, parent1_flat.size() - 1);
    int end = rng.generateRandomInt(0, parent1_flat.size() - 1);
    if (start > end) {
        std::swap(start, end);
    }
    std::vector<int> child1_flat = std::vector<int>(parent1_flat.size());
    std::vector<int> child2_flat = std::vector<int>(parent2_flat.size());

    // copy the selected part from parent1 to child1 and the selected part from parent2 to child2
    for (int i = start; i <= end; i++) {
        child1_flat[i] = parent1_flat[i];
        child2_flat[i] = parent2_flat[i];
    }

    // fill the rest of the child with the remaining genes from the other parent
    for (int i = 1; i < parent1_flat.size() - (end - start); i++) {
        int source_index = (end + i) % parent1_flat.size();
        int target_index = source_index; 
        while (std::find(child1_flat.begin(), child1_flat.end(), parent2_flat[target_index]) != child1_flat.end()) {
            target_index = (target_index + 1) % parent1_flat.size();
        }
        child1_flat[source_index] = parent2_flat[target_index];
    }
    
    for (int i = 1; i < parent2_flat.size() - (end - start); i++) {
        int source_index = (end + i) % parent2_flat.size();
        int target_index = source_index; 
        while (std::find(child2_flat.begin(), child2_flat.end(), parent1_flat[target_index]) != child2_flat.end()) {
            target_index = (target_index + 1) % parent2_flat.size();
        }
        child2_flat[source_index] = parent1_flat[target_index];
    }
    Genome child1 = unflatten_genome(child1_flat, parent1);
    Genome child2 = unflatten_genome(child2_flat, parent2);
    return std::make_pair(child1, child2);
}

auto partiallyMappedCrossover(const Genome &parent1, const Genome &parent2) -> std::pair<Genome, std::optional<Genome>>
{
    std::vector<int> parent1_flat = flatten_genome(parent1);
    std::vector<int> parent2_flat = flatten_genome(parent2);

    RandomGenerator& rng = RandomGenerator::getInstance();
    std::size_t start = rng.generateRandomInt(0, parent1_flat.size() - 1);
    std::size_t end = rng.generateRandomInt(0, parent1_flat.size() - 1);
    if (start > end) {
        std::swap(start, end);
    }
    std::vector<int> child1_flat = std::vector<int>(parent1_flat.size(), -1);
    std::vector<int> child2_flat = std::vector<int>(parent2_flat.size(), -1);

    // copy the selected part from parent1 to child1 and the selected part from parent2 to child2
    std::vector<int> previous_indices;
    for (int i = start; i <= end; i++) {
        child1_flat[i] = parent1_flat[i];
        child2_flat[i] = parent2_flat[i];
    }

    for (int i = start; i <= end; i++) {
        int index = i; 
        previous_indices.clear();
        // check if the value is already in the selected part
        if (std::find(child1_flat.begin(), child1_flat.end(), parent2_flat[i]) != child1_flat.end()) {          
            // the value is already in the selected part
            continue;
        }
        do {
            previous_indices.push_back(index);
            auto iterator = std::find(parent2_flat.begin(), parent2_flat.end(), parent1_flat[index]);
            index = iterator - parent2_flat.begin();
        } while ((start <= index && index <= end && std::find(previous_indices.begin(), previous_indices.end(), index) == previous_indices.end()) || child1_flat[index] != -1);
        
        child1_flat[index] = parent2_flat[i];
    }

    for (int i = start; i <= end; i++) {
        
        int index = i;
        previous_indices.clear();
        if (std::find(child2_flat.begin(), child2_flat.end(), parent1_flat[i]) != child2_flat.end()) {
            continue;
        }
        do {
            previous_indices.push_back(index);
            auto iterator = std::find(parent1_flat.begin(), parent1_flat.end(), parent2_flat[index]);
            index = iterator - parent1_flat.begin();
        } while ((start <= index && index <= end && std::find(previous_indices.begin(), previous_indices.end(), index) == previous_indices.end()) || child2_flat[index] != -1);
        child2_flat[index] = parent1_flat[i];
        
    }

    // fill the rest of the child with the remaining genes from the other parent
    for (int i = 1; i < parent1_flat.size(); i++) {
        int index = (i + end) % parent1_flat.size();
        // check if the value is undefined
        if (child1_flat[index] == -1) {
            int target_index = index; 
            while (std::find(child1_flat.begin(), child1_flat.end(), parent2_flat[target_index]) != child1_flat.end()) {
                target_index = (target_index + 1) % parent1_flat.size();
            }
            child1_flat[index] = parent2_flat[target_index];
        }

        if (child2_flat[index] == -1) {
            int target_index = index; 
            while (std::find(child2_flat.begin(), child2_flat.end(), parent1_flat[target_index]) != child2_flat.end()) {
                target_index = (target_index + 1) % parent2_flat.size();
            }
            child2_flat[index] = parent1_flat[target_index];
        }
    }

    Genome child1 = unflatten_genome(child1_flat, parent1); 
    Genome child2 = unflatten_genome(child2_flat, parent2);
    return std::make_pair(child1, child2);
}

auto edgeRecombination(const Genome &parent1, const Genome &parent2) -> std::pair<Genome, std::optional<Genome>>
{
    std::vector<int> parent1_flat = flatten_genome(parent1);
    std::vector<int> parent2_flat = flatten_genome(parent2);
    // assert that the genomes have the same length
    assert(parent1_flat.size() == parent2_flat.size());

    std::map<int, std::vector<int>> adjacency_list;
    for (int patientID : parent1_flat)
    {
        adjacency_list[patientID] = std::vector<int>();
    }
    for (int i = 0; i < parent1_flat.size(); i++) {
        int left = (i - 1 + parent1_flat.size()) % parent1_flat.size();
        int right = (i + 1) % parent1_flat.size();
        adjacency_list[parent1_flat[i]].push_back(parent1_flat[left]);
        adjacency_list[parent1_flat[i]].push_back(parent1_flat[right]);
        adjacency_list[parent2_flat[i]].push_back(parent2_flat[left]);
        adjacency_list[parent2_flat[i]].push_back(parent2_flat[right]);
    }

    RandomGenerator& rng = RandomGenerator::getInstance();
    std::vector<int> child;
    int current = parent1_flat[rng.generateRandomInt(0, parent1_flat.size() - 1)];
    for (int i = 0; i < parent1_flat.size(); i++) {
        child.push_back(current);
        for (auto& [key, value] : adjacency_list) {
            value.erase(std::remove(value.begin(), value.end(), current), value.end());
        }
        
        //Examine list for current element:
            // – If there is a common edge, pick that to be next element
            // – Otherwise pick the entry in the list which itself has the shortest list
            // – Ties are split at random
        int new_current = INT_MAX;
        std::set<int> seen = std::set<int>();
        for (int value : adjacency_list[current]) {
            if (seen.contains(value)) {
                new_current = value;
                break;
            }
            seen.insert(value);
        }

        // choice of new current is not random if there are two list of equal length. 
        if (new_current == INT_MAX) {
            int min_size = INT_MAX;
            for (int key : adjacency_list[current]) {
                std::set<int> value_set = std::set<int>(adjacency_list[key].begin(), adjacency_list[key].end());
                if (value_set.size() <= min_size) {
                    min_size = value_set.size();
                    new_current = key;
                }
            }
        }
        adjacency_list.erase(current);
        if (adjacency_list.empty()) {
            break;
        }
        if (new_current == INT_MAX) {
            do{
                new_current = parent1_flat[rng.generateRandomInt(0, parent1_flat.size() - 1)];
            } while (adjacency_list.find(new_current) == adjacency_list.end());
        }
        current = new_current;
    }
    return std::make_pair(unflatten_genome(child, parent1), std::nullopt);
}
