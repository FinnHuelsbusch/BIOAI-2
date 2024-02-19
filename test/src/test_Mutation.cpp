#include <gtest/gtest.h>
#include <gmock/gmock.h> 
#include "RandomGenerator.h"
#include "MockRandomGenerator.h"
#include "mutation.h"
#include "structures.h"



namespace {
class MutationTestFixture : public ::testing::Test {
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

TEST_F (MutationTestFixture, reassignOnePatient_standardCase) {

    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};
    // first call source nurse is 0
    // first call target nurse is 1
    // first call source patient index is 4
    // first call target patient index is 0
    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(1))
        .WillOnce(testing::Return(4))
        .WillOnce(testing::Return(0));

    
    Genome expectedGenome = {{1, 2, 3, 4},
                             {5, 6, 7, 8, 9, 10}};

    Genome result = reassignOnePatient(genome, {});
    EXPECT_EQ(result, expectedGenome);
} 

TEST_F (MutationTestFixture, reassignOnePatient_toEmptySource) {
    Genome genome = {{},
                     {1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};
    // first call source nurse is 0 
    // recursive call source nurse is 1
    // recursive call target nurse is 2 
    // recursive call source patient index is 4
    // recursive call target patient index is 0
    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(1))
        .WillOnce(testing::Return(2))
        .WillOnce(testing::Return(4))
        .WillOnce(testing::Return(0));

    
    Genome expectedGenome = {{},
                             {1, 2, 3, 4},
                             {5, 6, 7, 8, 9, 10}};

    Genome result = reassignOnePatient(genome, {});
    EXPECT_EQ(result, expectedGenome);
} 

TEST_F(MutationTestFixture, reassignOnePatient_toEmptyTarget) {
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10},
                     {}};
    // first call source nurse is 0 
    // first call target nurse is 2
    // first call source patient index is 4
    // no target patient index as it is empty
    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(2))
        .WillOnce(testing::Return(4));

    
    Genome expectedGenome = {{1, 2, 3, 4},
                             {6, 7, 8, 9, 10},
                             {5}};

    Genome result = reassignOnePatient(genome, {});
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, reassignOnePatient_toSameNurse) {
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};
    // first call source nurse is 0
    // first call target nurse is 0
    // second call target nurse is 1
    // first call source patient index is 4
    // first call target patient index is 0
    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(1))
        .WillOnce(testing::Return(4))
        .WillOnce(testing::Return(0));

    
    Genome expectedGenome = {{1, 2, 3, 4},
                             {5, 6, 7, 8, 9, 10}};

    Genome result = reassignOnePatient(genome, {});
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, swapWithinJourney_standardCase) {
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};
    // first call nurse is 0 chosen 
    // first call patient index is 4
    // first call second patient index is 3
    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(4))
        .WillOnce(testing::Return(3));

    Genome expectedGenome = {{1, 2, 3, 5, 4},
                             {6, 7, 8, 9, 10}};
    Genome result = swapWithinJourney(genome, {});
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, swapWithinJourney_JourneyIsToShort){
    Genome genome = {{1, 2, 3, 4},
                     {5}};
    // first call nurse is 1 chosen 
    // second call nurse is 0 chosen
    // first call patient index is 3
    // first call second patient index is 0
    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(1))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(3))
        .WillOnce(testing::Return(0));

    Genome expectedGenome = {{4, 2, 3, 1},
                             {5}};
    Genome result = swapWithinJourney(genome, {});
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, swapWithinJourney_SameIndexIsChosenTwice){
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};
    // first call nurse is 0 chosen 
    // first call patient index is 4
    // second call patient index is 4
    // third call patient index is 3
    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))
        .WillOnce(testing::Return(4))
        .WillOnce(testing::Return(4))
        .WillOnce(testing::Return(3));

    Genome expectedGenome = {{1, 2, 3, 5, 4},
                             {6, 7, 8, 9, 10}};
    Genome result = swapWithinJourney(genome, {});
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, swapBetweenJourneys_standardCase) {
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};

    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))  // source_nurse
        .WillOnce(testing::Return(1))  // destination_nurse
        .WillOnce(testing::Return(4))  // patient_index_nurse1
        .WillOnce(testing::Return(2)); // patient_index_nurse2
    Genome result = swapBetweenJourneys(genome, {});

    // Assert
    Genome expectedGenome = {{1, 2, 3, 4, 8},
                             {6, 7, 5, 9, 10}};
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, swapBetweenJourneys_emtpySource) {
    Genome genome = {{},
                     {1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};

    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))  // source_nurse
        .WillOnce(testing::Return(1))  // source_nurse resampled
        .WillOnce(testing::Return(2))  // destination_nurse
        .WillOnce(testing::Return(4))  // patient_index_nurse1
        .WillOnce(testing::Return(2)); // patient_index_nurse2
    Genome result = swapBetweenJourneys(genome, {});

    // Assert
    Genome expectedGenome = {{},
                             {1, 2, 3, 4, 8},
                             {6, 7, 5, 9, 10}};
    EXPECT_EQ(result, expectedGenome);

}

TEST_F(MutationTestFixture, swapBetweenJourneys_emtpyDestination) {
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10},
                     {}};

    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))  // source_nurse
        .WillOnce(testing::Return(2))  // destination_nurse
        .WillOnce(testing::Return(1))  // destination_nurse resampled
        .WillOnce(testing::Return(4))  // patient_index_nurse1
        .WillOnce(testing::Return(2)); // patient_index_nurse2
    Genome result = swapBetweenJourneys(genome, {});

    // Assert
    Genome expectedGenome = {{1, 2, 3, 4, 8},
                            {6, 7, 5, 9, 10},
                            {}};
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, swapBetweenJourneys_identicalSourceAndDestination){
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};

    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))  // source_nurse
        .WillOnce(testing::Return(0))  // destination_nurse
        .WillOnce(testing::Return(1))  // destination_nurse resampled
        .WillOnce(testing::Return(4))  // patient_index_nurse1
        .WillOnce(testing::Return(2)); // patient_index_nurse2
    Genome result = swapBetweenJourneys(genome, {});

    // Assert
    Genome expectedGenome = {{1, 2, 3, 4, 8},
                             {6, 7, 5, 9, 10}};
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, insertWithinJourney_standardCase) {
    Genome genome = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10}};

    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(0))  // nurse
        .WillOnce(testing::Return(4))  // patient index
        .WillOnce(testing::Return(3)); // insertion point

    Genome expectedGenome = {{1, 2, 3, 5, 4},
                             {6, 7, 8, 9, 10}};
    Genome result = insertWithinJourney(genome, {});
    EXPECT_EQ(result, expectedGenome);
}

TEST_F(MutationTestFixture, insertWithinJourney_JourneyIsToShort){
    Genome genome = {{1, 2, 3, 4, 5},
                     {6}};

    EXPECT_CALL(mockRng, generateRandomInt(testing::_, testing::_))
        .WillOnce(testing::Return(1))  // nurse
        .WillOnce(testing::Return(0))  // nurse resampled
        .WillOnce(testing::Return(4))  // patient index
        .WillOnce(testing::Return(0)); // insertion point

    Genome expectedGenome = {{5, 1, 2, 3, 4},
                             {6}};
    Genome result = insertWithinJourney(genome, {});
    EXPECT_EQ(result, expectedGenome);
}
} // namespace