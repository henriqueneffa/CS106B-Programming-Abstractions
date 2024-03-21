/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 4 - Backtracking
Instructor: Christopher Gregg
*/

// Code intends to return from a vector with # of votes per block the % of critical votes/power for each block
// Eg: from an input (50,49,1) return (60,20,20)

#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "voting.h"
#include "SimpleTest.h"
using namespace std;

// Helper function to count critical votes for a target block recursively
int recursionCritVotes(int targetBlockIndex, int index, int sumVotes, int accumulatedVotes, const Vector<int>& blocks) {
    // Base Case -- last block:
    if (index == blocks.size()) {
        // It's critical if the accumulated votes by themselves are not majority, but including this block it is
        if (accumulatedVotes < sumVotes / 2 + 1 && accumulatedVotes + blocks[targetBlockIndex] >= sumVotes / 2 + 1) {
            return 1;
        } else {
            return 0;
        }
    }

    // Deal with the case against the block itself
    if (index == targetBlockIndex) {
        return recursionCritVotes(targetBlockIndex, index + 1, sumVotes, accumulatedVotes, blocks);
    }

    // If without the block, it's >50%, doesn't work
    if (accumulatedVotes >= sumVotes / 2 + 1) {
        return 0;
    }
    // Calls the recursive funtion to create a tree that will explore all possible variations of the tree

    int criticalVotes = recursionCritVotes(targetBlockIndex, index + 1, sumVotes, accumulatedVotes, blocks) +
                        recursionCritVotes(targetBlockIndex, index + 1, sumVotes, accumulatedVotes + blocks[index], blocks);

    return criticalVotes;
}

// Function that returns the % of voting power given a vector of number of votes for each block
Vector<int> computePowerIndexes(Vector<int>& blocks) {

    // Vector that holds final result
    Vector<int> result(blocks.size());

    // Count Total Votes of the block
    int sumVotes = 0;
    for (int blockVotes : blocks) {
        sumVotes += blockVotes;
    }

    // Store the # of critical votes for each block and the total number of critical votes
    int sumCriticalVotes = 0;
    for (int i = 0; i < blocks.size(); i++) {
        // Call recursive function that returns # of critical votes
        int criticalVotes = recursionCritVotes(i, 0, sumVotes, 0, blocks);
        result[i] = criticalVotes;
        sumCriticalVotes += criticalVotes;
    }

    // Translate that into power. i.e., percentage
    for (int i = 0; i < blocks.size(); i++) {
        result[i] = (result[i] * 100) / sumCriticalVotes;
    }

    return result;
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Test power index, blocks 50-49-1") {
    Vector<int> blocks = {50, 49, 1};
    Vector<int> expected = {60, 20, 20};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks Hempshead 1-1-3-7-9-9") {
    Vector<int> blocks = {1, 1, 3, 7, 9, 9};
    Vector<int> expected = {0, 0, 0, 33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-NY 55-38-39") {
    Vector<int> blocks = {55, 38, 29};
    Vector<int> expected = {33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-GA 55-38-16") {
    Vector<int> blocks = {55, 38, 16};
    Vector<int> expected = {100, 0, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks EU post-Nice") {
    // Estonia is one of those 4s!!
    Vector<int> blocks = {29,29,29,29,27,27,14,13,12,12,12,12,12,10,10,10,7,7,7,7,7,4,4,4,4,4,3};
    Vector<int> expected = {8, 8, 8, 8, 7, 7, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Time power index operation") {
    Vector<int> blocks;
    for (int i = 0; i < 15; i++) {
        blocks.add(randomInteger(1, 10));
    }
    TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
}

STUDENT_TEST("Test power index, blocks 50-49") {
    Vector<int> blocks = {51, 49};
    Vector<int> expected = {100, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

STUDENT_TEST("Time power of each index operation") {
    Vector<int> blocks;
    for (int i = 0; i < 26; i++) {
        blocks.add(randomInteger(1, 10));
        TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
    }
}
