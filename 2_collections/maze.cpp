/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 2 - Warmup, Maze, Search Engine, and Beyond Algorithmic Analysis
Instructor: Christopher Gregg

Function solves the maze solution using BFS and DFS methods, providing the right paths for
solving the Maze problem
*/


#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// Defines each movement N,S,E,W and check if they are an open corridor for each and add into the set
// Also check if the movement is "InBounds"

Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;

    GridLocation N;
    N.row = cur.row - 1;
    N.col = cur.col;
    if (N.row >= 0 && N.row < maze.numRows() && N.col >= 0 && N.col < maze.numCols() && maze[N]) {
        neighbors.add(N);
    }

    GridLocation S;
    S.row = cur.row + 1;
    S.col = cur.col;
    if (S.row >= 0 && S.row < maze.numRows() && S.col >= 0 && S.col < maze.numCols() && maze[S]) {
        neighbors.add(S);
    }

    GridLocation E;
    E.row = cur.row;
    E.col = cur.col + 1;
    if (E.row >= 0 && E.row < maze.numRows() && E.col >= 0 && E.col < maze.numCols() && maze[E]) {
        neighbors.add(E);
    }

    GridLocation W;
    W.row = cur.row;
    W.col = cur.col - 1;
    if (W.row >= 0 && W.row < maze.numRows() && W.col >= 0 && W.col < maze.numCols() && maze[W]) {
        neighbors.add(W);
    }

    return neighbors;
}


// Checks if a Path is valid: not empty, starts at right point, ends at exit, with valid moves, and no loops
void validatePath(Grid<bool>& maze, Vector<GridLocation>& path) {
    if (path.isEmpty()) {
        error("Path is empty!");
    }

    GridLocation Beginning;
    Beginning.row = 0;
    Beginning.col = 0;
    if (path[0] != Beginning) {
        error("Path does not start at the entry (upper left corner) of the maze");
    }

    GridLocation End;
    End.row = maze.numRows() - 1;
    End.col = maze.numCols() - 1;
    if (path[path.size() - 1] != End) {
        error("Path does not end at the exit (lower right corner) of the maze");
    }

    // Check each location in the path is a valid move from the previous path location
    // and the path must not contain a loop, i.e. the same location cannot appear more than once in the path
    Set<GridLocation> seenLocations;
    for (int i = 0; i < path.size() - 1; i++) {
        GridLocation current = path[i];
        GridLocation next = path[i + 1];

        Set<GridLocation> validMoves = generateValidMoves(maze, current);
        if (!validMoves.contains(next)) {
            error("Invalid move in the path");
        }

        if (seenLocations.contains(current)) {
            error("Path contains a loop - already visited");
        }

        seenLocations.add(current);
    }

    // Check if the last location in the path has been visited before
    if (seenLocations.contains(path[path.size() - 1])) {
        error("Path contains a loop");
    }
}


/*
 * I created a path that is the baseline of this iteration;
 * then, I create a new path that is the baseline + generateValidMoves and register as a visited location
 * After that, you keep adding to that current path up to the point where current location == exit,
 * then we have our result
 */
Vector<GridLocation> solveMazeBFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Queue<Vector<GridLocation>> allPaths;

    GridLocation entry(0, 0);
    GridLocation exit(maze.numRows() - 1, maze.numCols() - 1);

    Vector<GridLocation> initialPath;
    initialPath.add(entry);
    allPaths.enqueue(initialPath);

    Set<GridLocation> visitedLocations;
    visitedLocations.add(entry);

    drawMaze(maze);

    while (!allPaths.isEmpty()) {
        Vector<GridLocation> currentPath = allPaths.dequeue();
        GridLocation currentLocation = currentPath[currentPath.size() - 1];

        if (currentLocation == exit) {
            return currentPath;
        }

        Set<GridLocation> neighbors = generateValidMoves(maze, currentLocation);

        for (GridLocation neighbor : neighbors) {
            if (!visitedLocations.contains(neighbor)) {
                Vector<GridLocation> newPath = currentPath;
                newPath.add(neighbor);
                allPaths.enqueue(newPath);
                visitedLocations.add(neighbor);
                highlightPath(newPath, "blue", 1); //Very low timing to speed processing
            }
        }
    }
    return path;
}

// Changed operations from Queue to Stack and the accordingly operations functions

Vector<GridLocation> solveMazeDFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Stack<Vector<GridLocation>> allPaths;

    GridLocation entry(0, 0);
    GridLocation exit(maze.numRows() - 1, maze.numCols() - 1);

    Vector<GridLocation> initialPath;
    initialPath.add(entry);
    allPaths.push(initialPath);

    Set<GridLocation> visitedLocations;
    visitedLocations.add(entry);

    drawMaze(maze);

    while (!allPaths.isEmpty()) {
        Vector<GridLocation> currentPath = allPaths.pop();
        GridLocation currentLocation = currentPath[currentPath.size() - 1];

        if (currentLocation == exit) {
            return currentPath;
        }

        Set<GridLocation> neighbors = generateValidMoves(maze, currentLocation);

        for (GridLocation neighbor : neighbors) {
            if (!visitedLocations.contains(neighbor)) {
                Vector<GridLocation> newPath = currentPath;
                newPath.add(neighbor);
                allPaths.push(newPath);
                visitedLocations.add(neighbor);
                highlightPath(newPath, "blue", 1); //Very low timing to speed processing
            }
        }
    }
    return path;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Vector<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0, 0}, {0, 2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution, hand-constructed maze") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Vector<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution read from file, medium maze") {
    Grid<bool> maze;
    Vector<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> notBeginAtEntry = { {1, 1}, {2, 1} };
    Vector<GridLocation> notEndAtExit = { {0, 0}, {1, 0}, {2, 0} };
    Vector<GridLocation> moveThroughWall = { {0 ,0}, {0, 1}, {1, 1}, {2, 1} };

    EXPECT_ERROR(validatePath(maze, notBeginAtEntry));
    EXPECT_ERROR(validatePath(maze, notEndAtExit));
    EXPECT_ERROR(validatePath(maze, moveThroughWall));
}

PROVIDED_TEST("solveMazeBFS on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMazeDFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("generateValidMoves on location surrounded by walls") {
    Grid<bool> maze = {{true, false, true},
                       {false, true, false},
                       {true, false, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves on location with diagonal walls") {
    Grid<bool> maze = {{false, true, true},
                       {true, false, false},
                       {true, false, false}};
    GridLocation center = {1, 0};
    Set<GridLocation> expected = {{2, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves on location on the side of 5x5 grid with surrounding walls") {
    Grid<bool> maze = {{false, true, true, true, false},
                       {false, false, false, false, false},
                       {false, false, true, false, false},
                       {false, true, false, false, false},
                       {false, true, true, true, false}};
    GridLocation side = {4, 1};
    Set<GridLocation> expected = {{4,2}, {3,1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

STUDENT_TEST("validatePath on 3X3 hand-constructed maze") {
    Grid<bool> maze = {{true, false, false},
                       {true, true, true},
                       {false, true, true}};
    Vector<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1}, {2,1}, {2,2} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("validatePath on move not valid") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> moveNotValid = { {1, 1}, {2, 2} };

    EXPECT_ERROR(validatePath(maze, moveNotValid));
}

STUDENT_TEST("empty string") {
    Grid<bool> maze = {{true, false},
                       {true, true},};
    Vector<GridLocation> moveNotValid = {};

    EXPECT_ERROR(validatePath(maze, moveNotValid));
}

STUDENT_TEST("solveMazeBFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeBFS on file 2x2") {
    Grid<bool> maze;
    readMazeFile("res/2x2.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeDFS on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);
    EXPECT_NO_ERROR(validatePath(maze, soln));
}
