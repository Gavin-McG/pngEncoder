#ifndef DYNAMICTREE_H
#define DYNAMICTREE_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <math.h>

using namespace std;

struct DynamicCode {
    uint8_t length;
    uint16_t val;

    DynamicCode(uint8_t length, uint16_t val) : length(length), val(val) {}
};

bool pairCompare(const pair<int, int>& a, const pair<int, int>& b);

//take a set of frequencies and return a vector of huffman codes O(n*maxL + nlogn) time & space
//alphabet is {0...n-1}
//all code lengths are no greater than maxL
vector<DynamicCode> getPrefixCodes(vector<size_t> &frequencies, size_t maxL);




struct node {
    bool proper; //whether it is a proper node. Is leaf node if not
    size_t val; //frequency value of node
    size_t index; //index for leaf nodes, pointer for proper nodes

    node(bool proper, size_t val, size_t index) : proper(proper), val(val), index(index) {}
};

//search a tree as applied in get CodeLengths to alter lengths vector
void exploreTree(vector<vector<node>> &table, size_t row, size_t tree, vector<size_t> &lengths);

//takes a sorted set of frequencies and returns corresponding code lengths O(n*maxL)
vector<size_t> getCodeLengths(vector<size_t> &frequencies, size_t maxL, size_t count);





vector<DynamicCode> lengths2Codes(vector<size_t> &lengths, size_t maxL);


#endif
