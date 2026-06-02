#ifndef ATSP_GRAPH_H
#define ATSP_GRAPH_H

#include <vector>
#include <string>

class AtspGraph {
private:
    int dimension;
    std::vector<std::vector<int>> costMatrix;

public:
    AtspGraph();
    bool loadFromFile(const std::string& filepath);
    int getDimension() const;
    int getCost(int from, int to) const;
    bool isLoaded() const;
};

#endif // ATSP_GRAPH_H