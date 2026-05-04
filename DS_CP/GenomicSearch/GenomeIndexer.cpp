#include "GenomeIndexer.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>

GenomeIndexer::GenomeIndexer() {}

bool GenomeIndexer::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Failed to open " << filename << std::endl;
        return false;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    genome = buffer.str();
    
    // Clean sequence by removing all whitespace/newlines
    genome.erase(std::remove_if(genome.begin(), genome.end(), ::isspace), genome.end());
    return true;
}

void GenomeIndexer::loadFromString(const std::string& sequence) {
    genome = sequence;
    genome.erase(std::remove_if(genome.begin(), genome.end(), ::isspace), genome.end());
}

void GenomeIndexer::build() {
    if (genome.empty()) return;
    buildSuffixArray();
}

// Prefix doubling suffix struct
struct Suffix {
    int index;
    int rank[2];
};

int cmp(struct Suffix a, struct Suffix b) {
    return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? 1 : 0) : (a.rank[0] < b.rank[0] ? 1 : 0);
}

void GenomeIndexer::buildSuffixArray() {
    int n = genome.size();
    if (n == 0) return;
    
    suffixArray.resize(n);
    std::vector<Suffix> suffixes(n);

    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = genome[i];
        suffixes[i].rank[1] = ((i + 1) < n) ? (genome[i + 1]) : -1;
    }

    std::sort(suffixes.begin(), suffixes.end(), cmp);

    std::vector<int> ind(n);
    for (int k = 4; k < 2 * n; k = k * 2) {
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;

        for (int i = 1; i < n; i++) {
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }

        for (int i = 0; i < n; i++) {
            int nextindex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (nextindex < n) ? suffixes[ind[nextindex]].rank[0] : -1;
        }

        std::sort(suffixes.begin(), suffixes.end(), cmp);
    }

    for (int i = 0; i < n; i++)
        suffixArray[i] = suffixes[i].index;
}


std::vector<int> GenomeIndexer::search(const std::string& pattern) const {
    std::vector<int> results;
    if (pattern.empty() || suffixArray.empty()) return results;
    
    int n = genome.size();
    int m = pattern.size();
    
    // Find lower bound
    int l = 0, r = n - 1;
    int first = -1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        int sa_mid = suffixArray[mid];
        
        int len = std::min(n - sa_mid, m);
        int cmpResult = genome.compare(sa_mid, len, pattern);
        if (cmpResult == 0 && (n - sa_mid) < m) cmpResult = -1;
        
        if (cmpResult < 0) {
            l = mid + 1;
        } else if (cmpResult > 0) {
            r = mid - 1;
        } else {
            first = mid;
            r = mid - 1; // force finding the first occurrence
        }
    }
    
    if (first == -1) return results;
    
    // Find upper bound
    l = first; 
    r = n - 1;
    int last = first;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        int sa_mid = suffixArray[mid];
        
        int len = std::min(n - sa_mid, m);
        int cmpResult = genome.compare(sa_mid, len, pattern);
        if (cmpResult == 0 && (n - sa_mid) < m) cmpResult = -1;
        
        if (cmpResult < 0) {
            l = mid + 1;
        } else if (cmpResult > 0) {
            r = mid - 1;
        } else {
            last = mid;
            l = mid + 1; // force finding the last occurrence
        }
    }
    
    for (int i = first; i <= last; ++i) {
        results.push_back(suffixArray[i]);
    }
    // Standardize order of outputs by index
    std::sort(results.begin(), results.end());
    return results;
}


