    /*
 * File: perfect.cpp
 * Author: Henrique Neffa
 * Date: April 15, 2023
 *
 * This file contains an exercise on how to improve algorithms and how to completely rethink them, using perfect numbers as an example
 *
 * What I learned: How much you can improve an algorithm just by thinking smartly about it,
 * and how totally rethinking an algorithm is even more beneficisl
 *
 * About me: I love sports (basketball, tennis, soccer) and nature (hiking, going to the beach)
 */
#include "console.h"
#include <iostream>
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/* The divisorSum function takes one argument `n` and calculates the
 * sum of proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division using the modulus operator %
 *
 * Note: the C++ long type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* The isPerfect function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* The findPerfects function takes one argument `stop` and performs
 * an exhaustive search for perfect numbers over the range 1 to `stop`.
 * Each perfect number found is printed to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/*
same as divisorSum, but...
added the sqrt bellow and adjusted to include the other divisors (complementaries)
 */
long smarterSum(long n) {
    long total = 1; // 1 is already a divisor
    for (long divisor = 2; divisor <= sqrt(n); divisor++) {
        if (n % divisor == 0) {
            total += divisor;
            if (divisor != n / divisor) {
                total += n / divisor;
            }
        }
    }
    return total;
}

/*
 * Similar to what was done before - just changed names
 */
bool isPerfectSmarter(long n) {
    return (n != 0) && (n == smarterSum(n));
}

/*
 * Similar to what was done before - just changed names
 */
void findPerfectsSmarter(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/*
 * We calculate m = 2^k -1.
 * Then check if it's prime, if yes we calculate the first Euclidian perfect number.
 * We iterate over this loop until we calculate the nth of that number
 */

long findNthPerfectEuclid(long n) {
    long perfectCount = 0;
    long k=1;
    while (perfectCount<n){
        long m = pow(2,k)-1;
        if (divisorSum(m)==1) {
            long perfect = pow(2,k-1) * m;
            perfectCount++;
            if (perfectCount == n) {
                return perfect;
            }
        }
        k++;
    }
    return 0;
}


/* * * * * * Test Cases * * * * * */

/* Note: Do not add or remove any of the PROVIDED_TEST tests.
 * You should add your own STUDENT_TEST tests below the
 * provided tests.
 */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

PROVIDED_TEST("Time trial of findPerfects on input size 1000") {
    TIME_OPERATION(1000, findPerfects(1000));
}


// Long test -- commented

/*
STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {

    int smallest = 35000, largest = 300000;

    for (int size = smallest; size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfects(size));
    }
}
*/

STUDENT_TEST("isPerfect works for negative"){
    EXPECT(!isPerfect(-5));
    EXPECT(!isPerfect(-6));
    EXPECT(!isPerfect(-28));
}

STUDENT_TEST("Check is smarterSum is working for positives"){
    EXPECT(isPerfectSmarter(6));
    EXPECT(isPerfectSmarter(28));
}

STUDENT_TEST("Check is smarterSum is working for negatives"){
    EXPECT(!isPerfectSmarter(12));
    EXPECT(!isPerfectSmarter(98765));
}

STUDENT_TEST("Check if is working for same numbers"){
    EXPECT(isPerfectSmarter(200)==isPerfect(200));
}

STUDENT_TEST("Multiple time trials of SmarterfindPerfects on increasing input sizes") {

    int smallest = 35000, largest = 300000;

    for (int size = smallest; size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfectsSmarter(size));
    }
}

STUDENT_TEST("findNthPerfectEuclid returns correct result for #1") {
    EXPECT_EQUAL(findNthPerfectEuclid(1), 6);
}

STUDENT_TEST("findNthPerfectEuclid returns correct result for #2") {
    EXPECT_EQUAL(findNthPerfectEuclid(2), 28);
}

STUDENT_TEST("findNthPerfectEuclid returns correct result for #3") {
    EXPECT_EQUAL(findNthPerfectEuclid(3), 496);
}

STUDENT_TEST("findNthPerfectEuclid returns correct result for #4") {
    EXPECT_EQUAL(findNthPerfectEuclid(4), 8128);
}

/*
 * Below is a suggestion of how to use a loop to set the input sizes
 * for a sequence of time trials.
 *
 *
STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {

    int smallest = 1000, largest = 8000;

    for (int size = smallest; size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfects(size));
    }
}

*/
