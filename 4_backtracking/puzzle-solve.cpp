/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 4 - Backtracking
Instructor: Christopher Gregg
*/

#include "puzzle-solve.h"
#include "Puzzle.h"
#include "PuzzleGUI.h"
#include "SimpleTest.h"

using namespace std;

// Code that runs the game and solves the puzzles

void tileMatch() {
    Puzzle puzzle;
    Vector<Tile> tiles;
    Action action;

    loadPuzzleConfig("puzzles/turtles/turtles.txt", puzzle, tiles);
    updateDisplay(puzzle, tiles);

    do {
        action = playInteractive(puzzle, tiles);
        if (action == LOAD_NEW) {
            string configFile = chooseFileDialog();
            loadPuzzleConfig(configFile, puzzle, tiles);
            updateDisplay(puzzle, tiles);
        } else if (action == RUN_SOLVE) {
            bool success = solve(puzzle, tiles);
            cout << "Found solution to puzzle? " << boolalpha << success << endl;
            updateDisplay(puzzle, tiles);
        }
    } while (action != QUIT);
}

// Solve the puzzle by placing Tiles
bool solve(Puzzle& puzzle, Vector<Tile>& tileVec) {
    // Base case: Stop when full
    if (puzzle.isFull()) {
        return true;
    }

    // Iterate on all tiles and rotations
    for (int i = 0; i < tileVec.size(); i++) {
        Tile currentTile = tileVec[i];
        for (int j = 0; j < NUM_SIDES; j++) {
            if (puzzle.canAdd(currentTile)) {
                puzzle.add(currentTile);
                tileVec.remove(i);

                // Recursively try to solve with other tiles
                if (solve(puzzle, tileVec)) {
                    return true;
                }

                // Backtrack
                puzzle.remove();
                tileVec.insert(i, currentTile);

                updateDisplay(puzzle, tileVec, 500);
            }

            // Rotate tile and try again
            currentTile.rotate();
        }
    }
    return false;
}
