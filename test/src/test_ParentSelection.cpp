#include <gtest/gtest.h>
#include <gmock/gmock.h> 
#include "RandomGenerator.h"
#include "MockRandomGenerator.h"
#include "parentSelection.h"
#include "structures.h"
#include <stdexcept>

namespace {
class PaentSelectionTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        RandomGenerator::setInstance(&mockRng);
    }

    void TearDown() override {
        // Clean up
        RandomGenerator::resetInstance();
    }

    MockRandomGenerator mockRng;
    
};

TEST_F (PaentSelectionTestFixture, tournamentSelection_insufficientParameters) {
    Population population = {}; 
    FunctionParameters parameters1; 
    FunctionParameters parameters2 = {{"tournamentSize", 2}};
    FunctionParameters parameters3 = {{"tournamentProbability", 0.5}};

    ASSERT_THROW(tournamentSelection(population, parameters1, 2), const char*);
    ASSERT_THROW(tournamentSelection(population, parameters2, 2), const char*);
    ASSERT_THROW(tournamentSelection(population, parameters3, 2), const char*);

}

TEST_F(PaentSelectionTestFixture, tournamentSelection_fittestWins) {
    Individual individual1 = {{{1,2,3,4,5}}};
    individual1.fitness = 1.0;
    Individual individual2 = {{{6,7,8,9,10}}};
    individual2.fitness = 0.5;
    Population population = {individual1, individual2};
    FunctionParameters parameters = {{"tournamentSize", 2}, {"tournamentProbability", 1}};
    int populationSize = 1;

    EXPECT_CALL(mockRng, generateRandomInt(0, 1))
        .WillOnce(testing::Return(0))//1 participant
        .WillOnce(testing::Return(1)); //2 participant

    EXPECT_CALL(mockRng, generateRandomDouble(0, 1))
        .WillOnce(testing::Return(0.4)); 

    Population parents = tournamentSelection(population, parameters, populationSize);
    EXPECT_EQ(parents.size(), 1);
    EXPECT_EQ(parents[0].genome, population[0].genome);
}
} // namespace