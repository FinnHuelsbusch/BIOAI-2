#include <gtest/gtest.h>
#include <gmock/gmock.h> 
#include "RandomGenerator.h"
#include "MockRandomGenerator.h"
#include "parentSelection.h"
#include "structures.h"
#include <stdexcept>

namespace {
class ParentSelectionTestFixture : public ::testing::Test {
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

TEST_F (ParentSelectionTestFixture, tournamentSelection_insufficientParameters) {
    Population population = {}; 
    FunctionParameters parameters1; 
    FunctionParameters parameters2 = {{"tournamentSize", 2}};
    FunctionParameters parameters3 = {{"tournamentProbability", 0.5}};

    ASSERT_THROW(tournamentSelection(population, parameters1, 2), std::invalid_argument);
    ASSERT_THROW(tournamentSelection(population, parameters2, 2), std::invalid_argument);
    ASSERT_THROW(tournamentSelection(population, parameters3, 2), std::invalid_argument);

}

TEST_F(ParentSelectionTestFixture, tournamentSelection_fittestWins) {
    Individual individual1 = {{{1,2,3,4,5}}};
    individual1.fitness = 1.0;
    Individual individual2 = {{{6,7,8,9,10}}};
    individual2.fitness = 0.5;
    Individual individual3 = {{{11,12}}};
    individual3.fitness = 0.0; 
    Population population = {individual1, individual2};
    FunctionParameters parameters = {{"tournamentSize", 2}, {"tournamentProbability", 1.0}};
    int populationSize = 1;

    // in order 
    EXPECT_CALL(mockRng, generateRandomInt(0, 1))
        .WillOnce(testing::Return(0))//1 participant
        .WillOnce(testing::Return(1)); //2 participant

    EXPECT_CALL(mockRng, generateRandomDouble(0, 1))
        .WillOnce(testing::Return(0.4)); 

    Population parents = tournamentSelection(population, parameters, populationSize);
    EXPECT_EQ(parents.size(), 1);
    EXPECT_EQ(parents[0].genome, population[0].genome);

    // reverse order
    EXPECT_CALL(mockRng, generateRandomInt(0, 1))
    .WillOnce(testing::Return(1))//1 participant
    .WillOnce(testing::Return(0)); //2 participant

    EXPECT_CALL(mockRng, generateRandomDouble(0, 1))
        .WillOnce(testing::Return(0.4)); 

    parents = tournamentSelection(population, parameters, populationSize);
    EXPECT_EQ(parents.size(), 1);
    Genome parentGenome = parents[0].genome; 
    Genome popGenome = population[0].genome; 
    EXPECT_EQ(parents[0].genome, population[0].genome);

    population = {individual2, individual1};
    EXPECT_CALL(mockRng, generateRandomInt(0, 1))
        .WillOnce(testing::Return(0))//1 participant
        .WillOnce(testing::Return(1)); //2 participant

    EXPECT_CALL(mockRng, generateRandomDouble(0, 1))
        .WillOnce(testing::Return(0.4)); 

    parents = tournamentSelection(population, parameters, populationSize);
    EXPECT_EQ(parents.size(), 1);
    EXPECT_EQ(population[1].genome, parents[0].genome);
}

TEST_F(ParentSelectionTestFixture, tournamentSelection_fittestDoesNotWin){
    Individual individual1 = {{{1,2,3,4,5}}};
    individual1.fitness = 1.0;
    Individual individual2 = {{{6,7,8,9,10}}};
    individual2.fitness = 0.5;
    Individual individual3 = {{{11,12}}};
    individual3.fitness = 0.0; 
    Population population = {individual1, individual2, individual3};
    FunctionParameters parameters = {{"tournamentSize", 3}, {"tournamentProbability", 0.0}};
    int populationSize = 1;

    // in order 
    EXPECT_CALL(mockRng, generateRandomInt(0, 2))
        .WillOnce(testing::Return(0))//1 participant
        .WillOnce(testing::Return(1))
        .WillOnce(testing::Return(2));
    EXPECT_CALL(mockRng, generateRandomInt(0,1))
        .WillOnce(testing::Return(1));

    EXPECT_CALL(mockRng, generateRandomDouble(0, 1))
    .WillOnce(testing::Return(0.4)); 
    Population parents = tournamentSelection(population, parameters, populationSize);
    EXPECT_EQ(parents.size(), 1);
    EXPECT_EQ(population[2].genome, parents[0].genome);
}
} // namespace