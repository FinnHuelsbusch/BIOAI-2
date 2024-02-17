#define TESTING_MODE
#include <gtest/gtest.h>
#include <gmock/gmock.h> 
#include "RandomGenerator.h"
#include "MockRandomGenerator.h"



namespace {
class RandomGeneratorTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up mock
        ON_CALL(mockRng, generateRandomInt(testing::_, testing::_)).WillByDefault(testing::ReturnRoundRobin({1, 13, 4711}));

        ON_CALL(mockRng, generateRandomDouble(testing::_, testing::_)).WillByDefault(testing::ReturnRoundRobin({0.1, 0.2, 0.3}));

        RandomGenerator::setInstance(&mockRng);
    }

    void TearDown() override {
        // Clean up
        RandomGenerator::resetInstance();
    }

    MockRandomGenerator mockRng;
};

TEST(RandomGeneratorTest, TestSetAndResetInstance) {
    RandomGenerator& rng = RandomGenerator::getInstance();

    MockRandomGenerator mockRng;
    
    EXPECT_NE(&rng, &mockRng);
    RandomGenerator::setInstance(&mockRng);
    EXPECT_EQ(&RandomGenerator::getInstance(), &mockRng);
    RandomGenerator::resetInstance();
    EXPECT_NE(&RandomGenerator::getInstance(), &mockRng);

}

TEST_F(RandomGeneratorTestFixture, TestGenerateRandomInt) {
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (int i : {1, 13, 4711}) {
        int result = rng.generateRandomInt(1, 100);
        EXPECT_EQ(i, result);
    }
}

TEST_F(RandomGeneratorTestFixture, TestGenerateRandomDouble) {
    RandomGenerator& rng = RandomGenerator::getInstance();
    for (double i : {0.1, 0.2, 0.3}) {
        double result = rng.generateRandomDouble(0.0, 1.0);
        EXPECT_EQ(i, result);
    }
}


}  // namespace
