#include "GenomeIndexer.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include <string>

void generateSyntheticDNA(const std::string& filename, int length) {
    std::ofstream out(filename);
    const char bases[] = {'A', 'C', 'G', 'T'};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);
    
    for (int i = 0; i < length; ++i) {
        out << bases[dis(gen)];
    }
}

int main(int argc, char** argv) {
    bool jsonMode = false;   //enable JSAON mode and store pattern 
    std::string searchPattern = "";
    if (argc >= 3 && std::string(argv[1]) == "--json") {
        jsonMode = true;
        searchPattern = argv[2];
    }

    std::string testFilename = "synthetic_dna.txt";
    int seqLength = 100000;
    
    std::ifstream checkFile(testFilename);
    if (!checkFile.good()) {
        if (!jsonMode) std::cout << "Creating synthetic DNA file of length " << seqLength << "..." << std::endl;
        generateSyntheticDNA(testFilename, seqLength);
    }
    checkFile.close();
    
    GenomeIndexer indexer; //loading to memory 
    if (!jsonMode) std::cout << "Loading genome from " << testFilename << "..." << std::endl;
    if (!indexer.loadFromFile(testFilename)) {
        return 1;
    }
    
    //..
    auto t1 = std::chrono::high_resolution_clock::now();
    indexer.build();
    auto t2 = std::chrono::high_resolution_clock::now();
    if (!jsonMode) std::cout << "Built Suffix Array in " << buildTime.count() << " ms." << std::endl;
    
    const std::string& genome = indexer.getSequence();

    if (jsonMode) {
        std::vector<int> saResults = indexer.search(searchPattern);

        std::cout << "{\n";
        std::cout << "  \"pattern\": \"" << searchPattern << "\",\n";
        std::cout << "  \"matches\": " << saResults.size() << ",\n";
        std::cout << "  \"indices\": [";
        for (size_t i = 0; i < saResults.size(); ++i) {
            std::cout << saResults[i];
            if (i < saResults.size() - 1) std::cout << ", ";
            if (i > 50) { // Limit to 50 for JSON output payload size
                std::cout << "\"TRUNCATED\"";
                break;
            }
        }
        std::cout << "]\n";
        std::cout << "}\n";
        return 0;
    }
              
    std::cout << "\n--- Genomic Pattern Search Engine CLI ---" << std::endl;
    std::cout << "Enter a pattern to search (or 'quit' to exit):" << std::endl;
    
    std::string pattern;
    while (true) {
        std::cout << "> ";
        if (!(std::cin >> pattern)) break;
        if (pattern == "quit") break;
        
        std::vector<int> saResults = indexer.search(pattern);
        
        std::cout << "\nMatches found: " << saResults.size() << std::endl;
        
        std::cout << "Indices: ";
        for (int i = 0; i < std::min((int)saResults.size(), 10); ++i) {
            std::cout << saResults[i] << " ";
        }
        if (saResults.size() > 10) std::cout << "...";
        std::cout << "\n\n";
    }
    
    return 0;
}
