/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 2 - Warmup, Maze, Search Engine, and Beyond Algorithmic Analysis
Instructor: Christopher Gregg

List the websites that present each of the strings we give as input to the system
*/


#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// Function cleans input of words into alpha and numeric type data
string cleanToken(string s) {
    string result;
    string sLowerCase = toLowerCase(s);
    for (int i = 0; i < sLowerCase.length(); i++) {
        if (isalpha(sLowerCase[i]) || isdigit(sLowerCase[i])) {
            result += sLowerCase[i];
        }
    }
    return result;
}

// We break the text into a vector, then iterate on it
// For every item, we clean it, then add into the new tokenSet

Set<string> gatherTokens(string text) {
    Set<string> tokens;

    Vector<string> tokenVector = stringSplit(text, " ");
    Set<string> tokenSet;

    for (string word : tokenVector) {
        string wordCleaned = cleanToken(word);
        if (wordCleaned.length()>0) {
            tokenSet.add(wordCleaned);
        }
    }
    return tokenSet;
}

// Read the file and fill in the set tokens with the URL and add the count
int buildIndex(string dbfile, Map<string, Set<string>>& index) {
    ifstream infile(dbfile);

    int count = 0;
    string url;
    string content;

    while (getline(infile, url)) {
        getline(infile, content);
        Set<string> tokens = gatherTokens(content);
        for (const string& token : tokens) {
            index[token].add(url);
        }
        count++;
    }
    return count;
}

// Give the output of links according to the search query looked for.
// Considering the + & - operations
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) { //Suggested
    Vector<string> queryTerms = stringSplit(query, " ");
    string term = cleanToken(queryTerms[0]);
    Set<string> result = index.get(term);


    // Idea is to build on results consecutively iterating on the new term if compound terms
    for (int i = 1; i < queryTerms.size(); i++) {
        term = cleanToken(queryTerms[i]);

        char modifier = queryTerms[i][0];
        Set<string> termMatches = index.get(term);
        if (modifier == '+') {
            result = result.intersect(termMatches);
        } else if (modifier == '-') {
            result = result - termMatches;
        } else {
            result = result.unionWith(termMatches);
        }
    }
    return result;
}


// Here, we read our file and prompt the user to ask for the word to be queried in the DB, and we return the URLs list

void searchEngine(string dbfile) { //Suggested
    Map<string, Set<string>> index;
    int pageCount = buildIndex(dbfile, index);
    cout << "Indexed " << pageCount << " pages containing " << index.size() << " unique terms." << endl;

    while (true) {
        string query = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        if (query.empty()) {
            break;
        }
        Set<string> matches = findQueryMatches(index, query);
        cout << "Found " << matches.size() << " matching pages" << endl;
        cout << matches << endl;
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings of letters and digits") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS106B"), "cs106b");
}

PROVIDED_TEST("cleanToken on strings containing punctuation") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
    EXPECT_EQUAL(cleanToken("they're"), "theyre");
}

PROVIDED_TEST("cleanToken on string of only punctuation") {
    EXPECT_EQUAL(cleanToken("#$^@@.;"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 6 unique tokens, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 6);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 12 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 12);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}

STUDENT_TEST("Test of additional untested cases for cleanToken") {
    EXPECT_EQUAL(cleanToken("Henrique Neffa"), "henriqueneffa");
    EXPECT_EQUAL(cleanToken("6CS798 !@`"), "6cs798");
    EXPECT_EQUAL(cleanToken("árabe"), "rabe");
}

STUDENT_TEST("Test of additional untested cases for gatherTokens") {
    Set<string> expected = {"bull", "lets", "go"};
    EXPECT_EQUAL(gatherTokens("Bull, Let's     go bull"), expected);
}

STUDENT_TEST("buildIndex from tiny.txt, check for index") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT(index.containsKey("red"));
}

STUDENT_TEST("findQueryMatches from tiny.txt, compound queries but with non-ASCII strings") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesTestA = findQueryMatches(index, "Fish RED!");
    EXPECT_EQUAL(matchesTestA.size(), 4);
    Set<string> matchesTestB = findQueryMatches(index, "red bread blue");
    EXPECT_EQUAL(matchesTestB.size(), 3);
}
