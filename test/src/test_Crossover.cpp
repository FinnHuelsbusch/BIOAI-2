#define TESTING_MODE
#include <gtest/gtest.h>
#include <gmock/gmock.h> 
#include "RandomGenerator.h"
#include "MockRandomGenerator.h"
#include "crossover.h"
#include "structures.h"



namespace {
class CrossoverTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up mock
        EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_)).WillRepeatedly(testing::ReturnRoundRobin({3, 6}));


        RandomGenerator::setInstance(&mockRng);
    }

    void TearDown() override {
        // Clean up
        RandomGenerator::resetInstance();
    }

    MockRandomGenerator mockRng;
    Genome parent1 = {{1, 2, 3, 4, 5, 6, 7, 8, 9}};
    Genome parent2 = {{9, 3, 7, 8, 2, 6 ,5, 1, 4}};
};

TEST_F (CrossoverTestFixture, TestCrossover) {
    
    Genome expectedChild1 = {{3, 8, 2, 4, 5, 6, 7, 1, 9}};
    Genome expectedChild2 = {{3, 4, 7, 8, 2, 6, 5, 9, 1}};

    std::pair<Genome, Genome> children = order1Crossover(parent1, parent2);
    Genome child1 = children.first;
    Genome child2 = children.second;

    EXPECT_EQ(child1, expectedChild1);
    EXPECT_EQ(child2, expectedChild2);

} 

TEST_F (CrossoverTestFixture, partiallyMappedCrossover) {
    Genome expectedChild1 = {{9, 3, 2, 4, 5, 6, 7, 1, 8}};
    Genome expectedChild2 = {{1, 7, 3, 8, 2, 6, 5, 4, 9}};

    std::pair<Genome, Genome> children = partiallyMappedCrossover(parent1, parent2);
    Genome child1 = children.first;
    Genome child2 = children.second;

    EXPECT_EQ(child1, expectedChild1);
    EXPECT_EQ(child2, expectedChild2);
}
} // namespace