/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <cctype>
#include <fstream>
#include <string>
#include "console.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/* This function is intended to return a string which
 * includes only the letter characters from the original
 * (all non-letter characters are excluded)
 */

// Adjusted the code to start from i=0 and define result as null

string lettersOnly(string s) {
    string result;
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}

// Make Upper case, then encode according to table provided, then return result
string encode(string s) {
    string sU = toUpperCase(s);
    string result;
    for (int i=0; i<s.length(); i++) {
        if (sU[i]=='A' or sU[i]=='E' or sU[i]=='I' or sU[i]=='O' or sU[i]=='U' or sU[i]=='H' or sU[i]=='W' or sU[i]=='Y') {
            result += '0';
        }
        if (sU[i]=='B' or sU[i]=='F' or sU[i]=='P' or sU[i]=='V') {
            result += '1';
        }
        if (sU[i]=='C' or sU[i]=='G' or sU[i]=='J' or sU[i]=='K' or sU[i]=='Q' or sU[i]=='S' or sU[i]=='X' or sU[i]=='Z') {
            result += '2';
        }
        if (sU[i]=='D' or sU[i]=='T') {
            result += '3';
        }
        if (sU[i]=='L') {
            result += '4';
        }
        if (sU[i]=='M' or sU[i]=='N') {
            result += '5';
        }
        if (sU[i]=='R') {
            result += '6';
        }
    }
    return result;
}

// Coalesce adjacent duplicate digits from the code (e.g. 222025 becomes 2025)
string coalesce(string s) {
    string result;
    if (s.empty()) {
        return s;
    }
    result += s[0];
    for (int i=1; i<s.length(); i++){
        if (s[i] != s[i-1]) {
            result += s[i];
        }
    }
    return result;
}

// Replace the first digit of the code with the first letter of the original name
string firstLetterOriginal (string s, string originalAlpha) {
    if (s.empty()) {
        return s;
    }
    string result=s;
    result[0]=toUpperCase(originalAlpha[0]);
    return result;
}

// Discard zeros
string discardZeros (string s) {
    string result;
    if (s.empty()) {
        return s;
    }
    for (int i=0; i<s.length(); i++){
        if (s[i] != '0') {
            result += s[i];
        }
    }
    return result;
}

string makeFourDigits (string s) {
    string result;
    if (s.length()==4) {
        result = s;
    } else if (s.length()>4) {
        result = s.substr(0,4);
    } else {
        result = s;
        for (int i=0; i < 4-s.length();i++){
            result += '0';
        }
    }
    return result;
}

// This soundex function compounds all the previously explained functions below (input = surname; output = soundex)

string soundex(string s) {
    string result = makeFourDigits(discardZeros(firstLetterOriginal(coalesce(encode(lettersOnly(s))), s)));
    return result;
}


/* This function intends to return other surnames that have the same soundex as the one inputted, based on a list previously provided
 */

void soundexSearch(string filepath) {
    // This provided code opens the specified file
    // and reads the lines into a vector of strings
    ifstream in;
    Vector<string> allNames;

    if (openFile(in, filepath)) {
        readEntireFile(in, allNames);
    }
    cout << "Read file " << filepath << ", "
         << allNames.size() << " names found." << endl << endl;

    // The names read from file are now stored in Vector allNames

    string inputSurname = getLine("Enter a surname (RETURN to quit): ");
    string result = soundex(inputSurname);
    cout << "Soundex code is " << result << endl;

    // Create and populate vector with the matching names
    vector<string> matchingNames; 
    for (string& name : allNames) {
        string nameSoundex = soundex(name);
        if (nameSoundex == result) {
            matchingNames.push_back(name);
        }
    }

    // Sort the matching names
    sort(matchingNames.begin(), matchingNames.end());

    // Print the matching names
    cout << "Matches from database: {" ;
    int i=0;
    for (const std::string& name : matchingNames) {
        cout << name;
        if (i<matchingNames.size()-1) {
            cout << ", ";
        }
        i++;
    }
    cout << "}" << endl;
    std::cout << endl << "All done!" << std::endl;
}


/* * * * * * Test Cases * * * * * */


PROVIDED_TEST("Test exclude of punctuation, digits, and spaces") {
    string s = "O'Hara";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "tldr");
}

PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}

// TODO: add your test cases here


STUDENT_TEST("First char Edge") {
    string s = "`Ronald";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "Ronald");
}

STUDENT_TEST("Empty string") {
    string s = "Mc Donald";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "McDonald");
}

STUDENT_TEST("Test string to encode"){
    string s = "McDonald";
    string result = encode(s);
    EXPECT_EQUAL(result, "52305043");
}

STUDENT_TEST("Test coalesce"){
    string s = "2555034";
    string result = coalesce(s);
    EXPECT_EQUAL(result, "25034");
    string s2 = "22255503444";
    string result2 = coalesce(s2);
    EXPECT_EQUAL(result2, "25034");
}

STUDENT_TEST("Test first letter change") {
    string s="5010";
    string originalAlpha = "Neffa";
    string result = firstLetterOriginal(s, originalAlpha);
    EXPECT_EQUAL(result, "N010");
}

STUDENT_TEST("Test if discarding zeros correctly") {
    string s="C0600";
    string result = discardZeros(s);
    EXPECT_EQUAL(result,"C6");
}

STUDENT_TEST("Test if having 4 digits") {
    string s="C6";
    string result = makeFourDigits(s);
    EXPECT_EQUAL(result,"C600");
    string s2="C345326";
    string result2 = makeFourDigits(s2);
    EXPECT_EQUAL(result2,"C345");
}

STUDENT_TEST("Neffa soundex test") {
    EXPECT_EQUAL(soundex("Neffa"), "N100");
}

STUDENT_TEST("Curie soundex test") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
}
