/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 3 - Recursion Etudes
Instructor: Christopher Gregg
*/

#include <iostream>    // for cout, endl
#include "queue.h"
#include "SimpleTest.h"
using namespace std;

// Here we create many functions that help us merge multiple queues in an increasing order
// We do that in a binary way and in a multi-way (using iteration and recursion)

void validateQueue(Queue<int> inputQ) {
    if (inputQ.isEmpty()) {
        return;
    }
    int previous = inputQ.dequeue();

    while (!inputQ.isEmpty()) {
        int current = inputQ.dequeue();
        if (current < previous) {
            error("Input Queue isn't in increasing order");
        }
        previous = current;
    }
}

// Get two Queues as inputs and generate a merged queue also sorted
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    // Validate input queues:
    validateQueue(a);
    validateQueue(b);

    Queue<int> result;

    // On the "Else", I check which is smaller, and input them into the result queue
    // Once one of the queues is empty, we just get data from the other one
    while (!a.isEmpty() || !b.isEmpty()) {
        if (a.isEmpty()) {
            result.enqueue(b.dequeue());
        } else if (b.isEmpty()) {
            result.enqueue(a.dequeue());
        } else {
            int aFront = a.peek();
            int bFront = b.peek();

            if (aFront < bFront) {
                result.enqueue(a.dequeue());
            } else {
                result.enqueue(b.dequeue());
            }
        }
    }

    return result;
}

/*
 * The function naiveMultiMerge assumes you have a correctly working
 * binaryMerge function, which this function calls to iteratively
 * merge a collection of sequences, starting off with an empty sequence
 * and repeatedly merging in an additional sequence until all are merged.
 * The code below is provided to you is implemented correctly and you
 * should not need to modify it.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}

// It does a multiway merge using divide-and-conquer method
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {
    int size = all.size();

    // Base case: when there is only 0 or 1 queue
    if (size == 1) {
        return all[0];
    }
    if (size == 0) {
        return Queue<int>();
    }

    // Divide the vector of queues in 2
    int half = size / 2;
    Vector<Queue<int>> leftHalf = all.subList(0, half);
    Vector<Queue<int>> rightHalf = all.subList(half, size - half);

    // Use the function on each half
    Queue<int> leftMerged = recMultiMerge(leftHalf);
    Queue<int> rightMerged = recMultiMerge(rightHalf);

    // Merge both
    return binaryMerge(leftMerged, rightMerged);
}



/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}

STUDENT_TEST("binaryMerge, test A") {
    Queue<int> a = {};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 3, 3};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, test B") {
    Queue<int> a = {8, 12, 13};
    Queue<int> b = {11, 15, 20};
    Queue<int> expected = {8, 11, 12, 13, 15, 20};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, test C error") {
    Queue<int> a = {13, 12, 8};
    Queue<int> b = {11, 15, 20};
    EXPECT_ERROR(binaryMerge(a, b));
}

STUDENT_TEST("Time binaryMerge operation with loop") {
    int smallest = 1000000;
    for (int size = smallest; size <= 8 * smallest; size *= 2) {
        Queue<int> a = createSequence(size/2);
        Queue<int> b = createSequence(size/2);
        TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
    }
}

STUDENT_TEST("Time naiveMultiMerge in a for loop") {
    int smallest = 1000000;
    for (int size = smallest; size <= 8 * smallest; size *= 2) {
        Queue<int> a = createSequence(size/4);
        Queue<int> b = createSequence(size/4);
        Queue<int> c = createSequence(size/4);
        Queue<int> d = createSequence(size/4);

        Vector<Queue<int>> manyQueues;
        manyQueues.add(a);
        manyQueues.add(b);
        manyQueues.add(c);
        manyQueues.add(d);

        TIME_OPERATION(a.size() + b.size() + c.size() + d.size(), naiveMultiMerge(manyQueues));
    }

}

STUDENT_TEST("Time recMultiMerge in a for loop") {
    int smallest = 1000000;
    for (int size = smallest; size <= 8 * smallest; size *= 2) {
        Queue<int> a = createSequence(size/4);
        Queue<int> b = createSequence(size/4);
        Queue<int> c = createSequence(size/4);
        Queue<int> d = createSequence(size/4);

        Vector<Queue<int>> manyQueues;
        manyQueues.add(a);
        manyQueues.add(b);
        manyQueues.add(c);
        manyQueues.add(d);

        TIME_OPERATION(a.size() + b.size() + c.size() + d.size(), recMultiMerge(manyQueues));
    }

}

/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}
