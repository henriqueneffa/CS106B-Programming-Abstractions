/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 3 - Recursion Etudes
Instructor: Christopher Gregg
*/

#include <iostream>    // for cout, endl
#include "recursion.h"
#include "SimpleTest.h"
using namespace std;

// This program draw Sierpinski Triangles in black according to the order provided

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}

// This function recursively draws Sierpinski Triangles
// It recursively calls itself until reaching order 0, where it draws the first traingle
// It reaches the order 0 multiple times by creating many ramifications of the recursion since it calls itself 3 times

int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    if (order == 0) {
        fillBlackTriangle(window, one, two, three);
        return 1;
    } else {
        // Define mid points where the triangle should be draw in each increasing order
        GPoint midOneTwo = {(one.x + two.x) / 2, (one.y + two.y) / 2};
        GPoint midTwoThree = {(two.x + three.x) / 2, (two.y + three.y) / 2};
        GPoint midThreeOne = {(three.x + one.x) / 2, (three.y + one.y) / 2};

        // Generates all other scenarios for each order recursively
        int count1 = drawSierpinskiTriangle(window, one, midOneTwo, midThreeOne, order - 1);
        int count2 = drawSierpinskiTriangle(window, midOneTwo, two, midTwoThree, order - 1);
        int count3 = drawSierpinskiTriangle(window, midThreeOne, midTwoThree, three, order - 1);

        return count1 + count2 + count3;
    }
}

// Test whether count is correct

STUDENT_TEST("Test returning count is correct") {
    GWindow window;
    GPoint one = {1,1};
    GPoint two = {2,0};
    GPoint three = {0,0};
    EXPECT_EQUAL(drawSierpinskiTriangle(window, one, two, three, 2),9);
}

/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all your testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {
    runInteractiveDemo();
}



