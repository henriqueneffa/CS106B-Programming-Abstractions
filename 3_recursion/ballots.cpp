/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 3 - Recursion Etudes
Instructor: Christopher Gregg
*/

#include "recursion.h"
#include "SimpleTest.h"
using namespace std;

/*
 * Count all orderings using a base case and recursive function
 * by creating a tree with all ramifications
 */
int countAllOrderings(int a, int b) {
    // Base case: If all remaining votes are for A or B, there
    // is only 1 ordering
    if (b == 0 || a == 0) {
        return 1;
    }
    // Recursive function: We are creating a tree with all the
    // possible combinations of A and B. When we arrive at the b or a equal 0
    // The tree go back up counting this total value
    return countAllOrderings(a - 1, b) + countAllOrderings(a, b - 1);
}

// Calculate number of good orderings using a helper integer called lead
int countGoodOrderingsHelper(int a, int b, int lead) {
    // Base case on 3 ifs below:
    // If lead reached 0, it means this sequence is not good
    if (lead == 0) {
        return 0;
    }
    // If a reached 0, the sequence is good if lead > b and bad if lead <= b:
    if (a == 0 && lead > b) {
        return 1;
    }
    if (a == 0 && lead <= b) {
        return 0;
    }
    if (b == 0) {
        return 1;
    }
    // Recursive function
    return countGoodOrderingsHelper(a - 1, b, lead + 1) + countGoodOrderingsHelper(a, b - 1, lead - 1);
}

// Provides the output with only 2 inputs
int countGoodOrderings(int a, int b) {
    if (a <= 0) {
        return 0;
    }
    // Let's start removing one a and counting 1 lead (1 a)
    return countGoodOrderingsHelper(a - 1, b, 1);
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("countAllOrderings, two A one B") {
   EXPECT_EQUAL(countAllOrderings(2, 1), 3);
}

PROVIDED_TEST("countGoodOrderings, two A one B") {
   EXPECT_EQUAL(countGoodOrderings(2, 1), 1);
}

STUDENT_TEST("countAllOrderings, two A two B") {
   EXPECT_EQUAL(countAllOrderings(2, 2), 6);
}

STUDENT_TEST("countGoodOrderings, two A two B") {
   EXPECT_EQUAL(countGoodOrderings(2, 2), 0);
}

STUDENT_TEST("countAllOrderings, three A one B") {
   EXPECT_EQUAL(countAllOrderings(3, 1), 4);
}

STUDENT_TEST("countGoodOrderings, three A one B") {
   EXPECT_EQUAL(countGoodOrderings(3, 1), 2);
}

STUDENT_TEST("countGoodOrderings, one A three B") {
   EXPECT_EQUAL(countGoodOrderings(1, 3), 0);
}

STUDENT_TEST("countGoodOrderings, Three A two B") {
   EXPECT_EQUAL(countGoodOrderings(3, 2), 2);
}

STUDENT_TEST("Test both formula's with Bertrand's Theorem") {
   for (int i = 0; i < 10; i++) {
        int randA = randomInteger(7, 14);
        int randB = randomInteger(0, 6);

        EXPECT_EQUAL(countGoodOrderings(randA, randB) / countAllOrderings(randA, randB), (randA - randB) / (randA + randB));
   }
}
