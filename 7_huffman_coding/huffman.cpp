#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "SimpleTest.h"  // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/*
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * messageBits queue contains a valid sequence of encoded bits.
 *
 * Your implementation may change the messageBits queue however you like. There
 * are no requirements about what it should look like after this function
 * returns. The encoding tree should be unchanged.
 *
 * Input being 0101100011 -> return string moons (considering our tree/root)
 */
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    string decodedText = "";
    EncodingTreeNode* selectNode = tree;

    while (!messageBits.isEmpty()) {
        // Get each bit
        Bit bit = messageBits.dequeue();
        // Move to next bit according to 0 or 1
        if (bit == 0) selectNode = selectNode->zero;
        else selectNode = selectNode->one;
        // If leaf, extract letter. Reset to top of tree again afterward
        if (selectNode->isLeaf()) {
            decodedText += selectNode->getChar();
            selectNode = tree;
        }
    }
    return decodedText;
}

/*
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>. You can assume that the queues are well-formed and represent a valid encoding tree.
 * Your implementation may change the queue parameters however you like. There are no requirements about what they should look like after this function returns
 *
 * Have tree shape (110100100) and treeLeaves (FlERA)to return a Tree (pointer to its root)
 */
EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    Bit bit = treeShape.dequeue();
    // Base case: Reach leaf node
    if (bit == 0) {
        char ch = treeLeaves.dequeue();
        return new EncodingTreeNode(ch);
    }
    // Recursive case: Interior Node. Perform function on ones to the right and to the left; return itself
    else {
        EncodingTreeNode* zero = unflattenTree(treeShape, treeLeaves);
        EncodingTreeNode* one = unflattenTree(treeShape, treeLeaves);
        return new EncodingTreeNode(zero, one);
    }
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 *Combination of the previous functions using EncodedData struct
 */
string decompress(EncodedData& data) {
    // unflatten
    EncodingTreeNode* tree = unflattenTree(data.treeShape, data.treeLeaves);

    // Decode
    string text = decodeText(tree, data.messageBits);

    // Deallocate
    deallocateTree(tree);

    return text;
}

/*
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 *
 * Use algorithm to build tree
 */
EncodingTreeNode* buildHuffmanTree(string text) {
    if (text.size() < 2) {
        error("Input text must contain at least two distinct characters.");
    }

    // Add chars to the map counting its frequency
    Map<char, int> frequencyMap;
    for (char ch : text) {
        frequencyMap[ch]++;
    }

    // Create priority queue with chars and frequencies
    PriorityQueue<EncodingTreeNode*> pq;
    for (char ch : frequencyMap) {
        pq.enqueue(new EncodingTreeNode(ch), frequencyMap[ch]);
    }

    // Remove the highest priority (smaller count), and group them to reinsert in the Pqueue (as pointers in tree structure) until it's 1 sized
    while (pq.size() > 1) {
        int weight1 = pq.peekPriority();
        EncodingTreeNode * node1 = pq.dequeue();

        int weight2 = pq.peekPriority();
        EncodingTreeNode* node2 = pq.dequeue();
        // Enqueued with new priority and in a tree structure of pointers

        pq.enqueue(new EncodingTreeNode(node1, node2), weight1+weight2);
    }

    return pq.dequeue();
}


// Helper function to encodeText:
void buildEncodeMap(EncodingTreeNode* tree, string bitList, Map<char, Queue<Bit>>& map) {
    // Empty tree
    if (tree == nullptr) {
        return;
    }
    // Base case: Tree root is a leaf
    if (tree->isLeaf()) {
        // Create a queue that will store the bits of each character and then will put into the map char -> bits
        Queue<Bit> bitQueue;
        for (char bit : bitList) {
            bitQueue.enqueue(Bit(bit - '0'));
        }
        map[tree->getChar()] = bitQueue;
    } else { // recursive case
        buildEncodeMap(tree->zero, bitList + "0", map);
        buildEncodeMap(tree->one, bitList + "1", map);
    }
}

// Given a string and an encoding tree, encode the text using the tree and return a Queue<Bit> of the encoded bit sequence.
// You can assume tree is a valid non-empty encoding tree and contains an encoding for every character in the text.
// transform a pointer to a root and a text into a Bit code (encoding)
Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    // Map for each char -> Bit sequence
    Map<char, Queue<Bit>> map;
    buildEncodeMap(tree, "", map);
    // create encoded message
    Queue<Bit> encodedMessage;
    // Dequeue list of bits of each chat on the encodedMessage (queue with all chars translted into bits)
    for (char c:text) {
        Queue<Bit> bitList = map[c];
        while (!bitList.isEmpty()) {
            encodedMessage.enqueue(bitList.dequeue());
        }
    }

    return encodedMessage;
}



/*
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner specified in the assignment writeup.
 * You can assume the input queues are empty on entry to this function. You can assume tree is a valid well-formed encoding tree.
 * Receive as input the root and return the treeshape and treeLeaves
 */
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    // Case of no value in tree
    if (tree == nullptr) {
        return;
    }
    // If is leaf, enqueue the letterand a 0
    if (tree->isLeaf()) {
        treeShape.enqueue(0);
        treeLeaves.enqueue(tree->getChar());
    // If not, enqueue 1, and recurse on left and right children
    } else {
        treeShape.enqueue(1);
        flattenTree(tree->zero, treeShape, treeLeaves);
        flattenTree(tree->one, treeShape, treeLeaves);
    }
}

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and flattened
 * encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 *
 * Combination of all functions thus far
 */
EncodedData compress(string messageText) {
    EncodingTreeNode* tree = buildHuffmanTree(messageText);
    Queue<Bit> messageBits = encodeText(tree, messageText);
    Queue<Bit> treeShape;
    Queue<char> treeLeaves;
    flattenTree(tree, treeShape, treeLeaves);

    deallocateTree(tree);

    // Return in struct established
    return EncodedData {treeShape, treeLeaves, messageBits};
}

/* * * * * * Testing Helper Functions Below This Point * * * * * */

EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */
    // Implementation of pointers pointing to struct of LEAF NODES:
    EncodingTreeNode* t = new EncodingTreeNode('T');
    EncodingTreeNode* r = new EncodingTreeNode('R');
    EncodingTreeNode* s = new EncodingTreeNode('S');
    EncodingTreeNode* e = new EncodingTreeNode('E');
    // Implementation of pointers pointing to INTERNAL NODES:
    EncodingTreeNode* rs = new EncodingTreeNode(r, s);
    EncodingTreeNode* rse = new EncodingTreeNode(rs, e);
    EncodingTreeNode* root = new EncodingTreeNode(t, rse);

    return root;
}

void deallocateTree(EncodingTreeNode* t) {
    // Base case: Null pointer
    if (t == nullptr) {
        return;
    }
    // recursive case: delete left and right children:
    deallocateTree(t->zero);
    deallocateTree(t->one);
    delete t;
}

bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {
    // Base case: return true if both null pointers
    if (a == nullptr && b == nullptr) {
        return true;
    }
    // If only one null pointer, false
    if (a == nullptr || b == nullptr) {
        return false;
    }
    // Check if same char
    if (a->isLeaf() && b->isLeaf()) {
        return a->getChar() == b->getChar();
    }
    // Recursive case (Check whole tree starting from root)
    return areEqual(a->zero, b->zero) && areEqual(a->one, b->one);
}

/* * * * * * Test Cases Below This Point * * * * * */

// Tests for AreEqual (and as a consequence other warmup functions):

STUDENT_TEST("areEqual with empty trees") {
    EXPECT(areEqual(nullptr, nullptr));
}

STUDENT_TEST("areEqual with two trees") {
    EncodingTreeNode* tree1 = new EncodingTreeNode(new EncodingTreeNode('A'), new EncodingTreeNode('B'));
    EncodingTreeNode* tree2 = new EncodingTreeNode(new EncodingTreeNode('A'), new EncodingTreeNode('B'));
    EXPECT(areEqual(tree1, tree2));

    deallocateTree(tree1);
    deallocateTree(tree2);
}

STUDENT_TEST("areEqual with rearranged trees") {
    EncodingTreeNode* tree1 = new EncodingTreeNode(new EncodingTreeNode('A'), new EncodingTreeNode('B'));
    EncodingTreeNode* tree2 = new EncodingTreeNode(new EncodingTreeNode('B'), new EncodingTreeNode('A'));
    EXPECT(!areEqual(tree1, tree2));

    deallocateTree(tree1);
    deallocateTree(tree2);
}

STUDENT_TEST("areEqual with example trees") {
    EncodingTreeNode* exampleTree1 = createExampleTree();
    EncodingTreeNode* exampleTree2 = createExampleTree();
    EXPECT(areEqual(exampleTree1, exampleTree2));

    deallocateTree(exampleTree1);
    deallocateTree(exampleTree2);
}


/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeShape   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeShape, treeShape);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "Nana Nana Nana Nana Nana Nana Nana Nana Batman",
        "Research is formalized curiosity. It is poking and prying with a purpose. – Zora Neale Hurston",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(input, output);
    }
}
