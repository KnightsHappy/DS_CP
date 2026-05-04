#pragma once
#include <string>
#include <vector>

class GenomeIndexer {
public:
    GenomeIndexer();
    ~GenomeIndexer() = default;

    // Load genome from file
    bool loadFromFile(const std::string& filename);
    
    // Load genome from string (for synthetic testing)
    void loadFromString(const std::string& sequence);

    // Build internal structures Suffix Array (O(n log^2 n))
    void build();

    // Instant Search using binary search on Suffix Array, returns vector of starting indices
    // Runs in O(m log n) time where m is pattern length and n is genome length.
    std::vector<int> search(const std::string& pattern) const;

    // Get the standard genome string (for validation testing)
    const std::string& getSequence() const { return genome; }

private:
    std::string genome;
    std::vector<int> suffixArray;

    // O(n log^2 n) Suffix Array construction using Prefix Doubling
    void buildSuffixArray();
    
    void buildSuffixArray();
};
