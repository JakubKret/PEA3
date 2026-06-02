#include "AtspGraph.h"
#include <fstream>
#include <iostream>
#include <sstream>

AtspGraph::AtspGraph() : dimension(0) {}

bool AtspGraph::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Blad: Nie mozna otworzyc pliku " << filepath << std::endl;
        return false;
    }

    std::string line;
    bool reading_nodes = false;
    std::vector<int> matrix_data;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("DIMENSION") == 0) {
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                dimension = std::stoi(line.substr(colon_pos + 1));
            }
        } 
        else if (line.find("EDGE_WEIGHT_SECTION") == 0) {
            reading_nodes = true;
        } 
        else if (line.find("EOF") == 0) {
            break;
        } 
        else if (reading_nodes) {
            std::stringstream ss(line);
            int weight;
            while (ss >> weight) {
                matrix_data.push_back(weight);
            }
        }
    }
    file.close();

    if (dimension == 0 || matrix_data.size() != static_cast<size_t>(dimension * dimension)) {
        std::cerr << "Blad: Nieprawidlowy format danych." << std::endl;
        return false;
    }

    costMatrix.assign(dimension, std::vector<int>(dimension, 0));
    int index = 0;
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            costMatrix[i][j] = matrix_data[index++];
        }
    }
    return true;
}

int AtspGraph::getDimension() const { return dimension; }
int AtspGraph::getCost(int from, int to) const { return costMatrix[from][to]; }
bool AtspGraph::isLoaded() const { return dimension > 0; }