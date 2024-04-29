#include "../include/dynamicTree.h"


bool pairCompare(const pair<int, int>& a, const pair<int, int>& b) {
    if (a.second == 0 && b.second != 0) {
        return false; // a (zero) should go after b
    } else if (a.second != 0 && b.second == 0) {
        return true; // a should go before b (b is zero)
    } else {
        return a.second < b.second; // normal comparison
    }
}

vector<DynamicCode> getPrefixCodes(vector<size_t> &frequencies, size_t maxL) {
    //sort index and frequency pairs
    vector<pair<size_t,size_t>> frequencyIndex;
    frequencyIndex.reserve(frequencies.size());
    for (size_t i=0;i<frequencies.size();++i) {
        frequencyIndex.emplace_back(i,frequencies[i]);
    }
    sort(frequencyIndex.begin(),frequencyIndex.end(),pairCompare);

    //seperate pairs into 2 lists
    vector<size_t> indices;
    indices.reserve(frequencies.size());
    vector<size_t> sortedFreq;
    sortedFreq.reserve(frequencies.size());
    for (size_t i=0; i<frequencies.size(); ++i) {
        indices.push_back(frequencyIndex[i].first);
        sortedFreq.push_back(frequencyIndex[i].second);
    }
    frequencyIndex = vector<pair<size_t,size_t>>();

    //count non-zero values
    size_t values = 0;
    for (size_t i=0;i<sortedFreq.size(); ++i, ++values) {
        if (sortedFreq[i]==0) {
            break;
        }
    }

    //no values edge case
    if (values==0) {
        return vector<DynamicCode>(frequencies.size(),DynamicCode(0,0));
    }

    //get code lengths from sorted frequencies
    vector<size_t> lengths = getCodeLengths(sortedFreq, maxL, values);

    //add zero-length codes
    for (size_t i=0;i<sortedFreq.size()-values;++i) {
        lengths.push_back(0);
    }

    //unsort lengths
    vector<size_t> unsortedLengths;
    unsortedLengths.resize(frequencies.size());
    for (size_t i=0;i<indices.size();++i) {
        unsortedLengths[indices[i]] = lengths[i];
    }

    //turn code lengths into codes
    return lengths2Codes(unsortedLengths, maxL);
}



void exploreTree(vector<vector<node>> &table, size_t row, size_t tree, vector<size_t> &lengths) {
    if (table[row][tree].proper) {
        exploreTree(table, row-1, table[row][tree].index,lengths);
        exploreTree(table, row-1, table[row][tree].index+1,lengths);
    }else{
        ++lengths[table[row][tree].index];
    }
}

vector<size_t> getCodeLengths(vector<size_t> &frequencies, size_t maxL, size_t count) {
    vector<vector<node>> table;
    table.resize(maxL);

    //create leaf and roots 
    vector<node> leaves;
    for (size_t i=0;i<count;++i) {
        leaves.emplace_back(false,frequencies[i],i);
    }
    vector<node> roots;

    //fill table with nodes
    for (size_t i=0;i<maxL; ++i) {
        //merge roots and leaves into table row in sorted order
        size_t ind1=0, ind2=0;
        while (ind1<leaves.size() || ind2<roots.size()) {
            if (ind1==leaves.size()) {
                table[i].push_back(roots[ind2++]);
            }else if (ind2==roots.size()) {
                table[i].push_back(leaves[ind1++]);
            }else if (leaves[ind1].val<roots[ind2].val) {
                table[i].push_back(leaves[ind1++]);
            }else{
                table[i].push_back(roots[ind2++]);
            }
        }

        //generate new roots
        roots = vector<node>();
        for (size_t j=0; j<table[i].size()/2; ++j) {
            roots.emplace_back(true,table[i][j*2].val+table[i][j*2+1].val,j*2);
        }
    }
    leaves = vector<node>();
    roots = vector<node>();

    //explore first 2n-2 trees to get code lengths
    vector<size_t> lengths(count,0);
    for (size_t i=0;i<max(1ul,count*2-2);++i) {
        exploreTree(table,maxL-1,i,lengths);
    }
    return lengths;
}




vector<DynamicCode> lengths2Codes(vector<size_t> &lengths, size_t maxL) {
    //count number of codes for each code length
    vector<size_t> bl_count(maxL+1);
    for (size_t i=0;i<lengths.size();++i) {
        ++bl_count[lengths[i]];
    }

    //find the numerically smallest code for each code length
    uint32_t code = 0;
    bl_count[0] = 0;
    vector<uint32_t> next_code(maxL+1);
    for (size_t bits = 1; bits <= maxL; bits++) {
        code = (code + bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }

    //assign numerical values for all codes
    vector<DynamicCode> codes;
    for (size_t i=0; i<lengths.size();++i) {
        if (lengths[i] != 0) {
            codes.emplace_back(lengths[i],next_code[lengths[i]]++);
        }else{
            codes.emplace_back(0,0);
        }
    }

    return codes;
}