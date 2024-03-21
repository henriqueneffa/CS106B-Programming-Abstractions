/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 6 - Linked Lists
Instructor: Christopher Gregg
*/

#include "Labyrinth.h"
using namespace std;

// Function that tells whether the path is legal and all 3 items have been picked up
bool isPathToFreedom(MazeCell* start, string moves) {
    // Declare current pointer = to starting point
    MazeCell* current = start;
    // Define whether we picked up Spellbook / Potion / Wand
    bool pickedupSpellbook = false;
    bool pickedupPotion= false;
    bool pickedupWand  = false;

    // Iterate on all characters
    for (char move : moves) {
        // Checks if any of the items are in the current pointer location
        if (current->whatsHere == Item::SPELLBOOK) {
            pickedupSpellbook = true;
        } else if (current->whatsHere == Item::POTION) {
            pickedupPotion =true;
        } else if (current->whatsHere == Item::WAND) {
            pickedupWand = true;
        }

        // Check if null pointer (if yes, return false);
        if (move == 'N') {
            if (current->north != nullptr) {
                // Assign a new pointer that is the location the north object is pointing to. [same for the other coordinates]
                current = current->north;
            } else {
                return false;
            }
        } else if (move == 'S') {
            if (current->south != nullptr) {
                current = current->south;
            } else {
                return false;
            }
        } else if (move == 'E') {
            if (current->east != nullptr) {
                current = current->east;
            } else {
                return false;
            }
        } else if (move == 'W') {
            if (current->west != nullptr) {
                current = current->west;
            } else {
                return false;
            }
        }
    }
    // Last check to see if last location also contains items
    if (current->whatsHere == Item::SPELLBOOK) {
        pickedupSpellbook = true;
    } else if (current->whatsHere == Item::POTION) {
        pickedupPotion = true;
    } else if (current->whatsHere == Item::WAND) {
        pickedupWand = true;
    }
    // Return whether we picked up items
    return pickedupSpellbook&&pickedupPotion&&pickedupWand;
}


/* * * * * * Test Cases Below This Point * * * * * */
#include "GUI/SimpleTest.h"
#include "Demos/MazeGenerator.h"

/* Optional: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

/* Utility function to free all memory allocated for a maze. */
void deleteMaze(const Grid<MazeCell*>& maze) {
    for (auto* elem: maze) {
        delete elem;
    }
    /* Good question to ponder: why don't we write 'delete maze;'
     * rather than what's shown above?
     */
}

PROVIDED_TEST("Checks paths in the sample maze.") {
    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths are the ones in the handout. They all work. */
    EXPECT(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWN"));
    EXPECT(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES"));
    EXPECT(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES"));

    /* These paths don't work, since they don't pick up all items. */
    EXPECT(!isPathToFreedom(maze[2][2], "ESNW"));
    EXPECT(!isPathToFreedom(maze[2][2], "SWWN"));
    EXPECT(!isPathToFreedom(maze[2][2], "WNNE"));

    /* These paths don't work, since they aren't legal paths. */
    EXPECT(!isPathToFreedom(maze[2][2], "WW"));
    EXPECT(!isPathToFreedom(maze[2][2], "NN"));
    EXPECT(!isPathToFreedom(maze[2][2], "EE"));
    EXPECT(!isPathToFreedom(maze[2][2], "SS"));

    deleteMaze(maze);
}

PROVIDED_TEST("Can't walk through walls.") {
    auto maze = toMaze({"* S *",
                        "     ",
                        "W * P",
                        "     ",
                        "* * *"});

    EXPECT(!isPathToFreedom(maze[1][1], "WNEES"));
    EXPECT(!isPathToFreedom(maze[1][1], "NWSEE"));
    EXPECT(!isPathToFreedom(maze[1][1], "ENWWS"));
    EXPECT(!isPathToFreedom(maze[1][1], "SWNNEES"));

    deleteMaze(maze);
}

PROVIDED_TEST("Works when starting on an item.") {
    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "EE"));
    EXPECT(isPathToFreedom(maze[0][1], "WEE"));
    EXPECT(isPathToFreedom(maze[0][2], "WW"));

    deleteMaze(maze);
}

/* Printer for items. */
ostream& operator<< (ostream& out, Item item) {
    if (item == Item::NOTHING) {
        return out << "Item::NOTHING";
    } else if (item == Item::WAND) {
        return out << "Item::WAND";
    } else if (item == Item::POTION) {
        return out << "Item::POTION";
    } else if (item == Item::SPELLBOOK) {
        return out << "Item::SPELLBOOK";
    } else {
        return out << "<unknown item type>";
    }
}
