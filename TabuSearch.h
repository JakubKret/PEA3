#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include "AtspGraph.h"
#include <vector>

struct Solution {
    std::vector<int> path;
    long long totalCost;
};

class TabuSearch {
private:
    const AtspGraph& graph;
    double timeLimitSeconds;
    int tabuTenure;
    int maxTabuListSize;
    int neighborsToGenerate;
    bool useAspiration;

    long long calculateCost(const std::vector<int>& path) const;
    Solution solveNNFromNode(int startNode) const;

public:
    TabuSearch(const AtspGraph& g);

    void setTimeLimit(double seconds);
    void setTabuTenure(int tenure);
    void setMaxTabuListSize(int size);
    void setNeighborsToGenerate(int neighbors);
    void setAspiration(bool enable);

    // Rozwiazania poczatkowe
    Solution generateInitialSolutionRNN() const;
    Solution generateRandomSolution() const;

    // Glowny algorytm
    Solution solve(const Solution& initialSolution);
};

#endif // TABU_SEARCH_H