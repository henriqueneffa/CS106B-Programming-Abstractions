/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 3 - Recursion Etudes
Instructor: Christopher Gregg
*/

#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "SimpleTest.h"

using namespace std;

// Program identifies when operators such as (), {}, [] are correctly used in a programming writing

// Function eliminates all chars of a string with the exception of operators {}[]()
string operatorsFrom(string str) {
    // Base case: if it's first index, return blank
    if (str.length() == 0) {
        return "";
    }
    // Recursive function: If it's a operator, include itself + previous recursive iteration
    char curStr = str[str.length() - 1];
    string newStr = str.substr(0, str.length() - 1); // Create a new string without the last character
    if (curStr == '{' || curStr == '}' || curStr == '(' || curStr == ')' || curStr == '[' || curStr == ']') {
        return operatorsFrom(newStr) + curStr;
    // Recursive function: If it's not a operator, include only previous recursive iteration
    } else {
        return operatorsFrom(newStr);
    }
}

// Check if operators have a match (i.e., if { is linked to a })
bool operatorsAreMatched(string ops) {
    // Base case: String is empty
    if (ops.length() == 0) {
        return true;
    }

    // Recursive Case:
    char opsFirst = ops[0];
    // Should start with an opener operator
    if (opsFirst == ']' || opsFirst == '}' || opsFirst == ')') {
        return false;
    }
    // Define complementary operators
    char opsFirstOpposite;
    if (opsFirst == '[') {
        opsFirstOpposite = ']';
    }
    if (opsFirst == '(') {
        opsFirstOpposite = ')';
    }
    if (opsFirst == '{') {
        opsFirstOpposite = '}';
    }

    // Check on loop whether the first operator has a complementary operator
    bool found = false;
    char opsNext;
    int nextIndex;
    for (int i = 1; i < ops.length(); i++) {
        if (opsFirstOpposite == ops[i]) {
            opsNext = ops[i];
            nextIndex = i;
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }
    // Recursively checks for this operation until the string is empty
    string stringPostRecursion1 = ops.erase(nextIndex, 1);
    string stringPostRecursion2 = stringPostRecursion1.erase(0,1);
    return operatorsAreMatched(stringPostRecursion2);
}

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}

STUDENT_TEST("operatorsFrom case A") {
    EXPECT_EQUAL(operatorsFrom("v}{ec[[]()3]"), "}{[[]()]");
}

STUDENT_TEST("operatorsFrom case B") {
    EXPECT_EQUAL(operatorsFrom("vector"), "");
}

STUDENT_TEST("operatorsFrom case C") {
    EXPECT_EQUAL(operatorsFrom("{][{"), "{][{");
}

STUDENT_TEST("operatorsAreMatched on example A") {
    EXPECT(!operatorsAreMatched("][{}"));
}

STUDENT_TEST("operatorsAreMatched on example B") {
    EXPECT(!operatorsAreMatched("((()"));
}

STUDENT_TEST("operatorsAreMatched on example C") {
    EXPECT(operatorsAreMatched("()[]{}{}"));
}

STUDENT_TEST("operatorsAreMatched on example D") {
    EXPECT(operatorsAreMatched("{[()]}"));
}

STUDENT_TEST("operatorsFrom on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT_EQUAL(operatorsFrom(example), "(){([])(())}");
}

STUDENT_TEST("operatorsAreMatched on example from writeup") {
    EXPECT(operatorsAreMatched("(){([])(())}"));
}
