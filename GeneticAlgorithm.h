#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <vector>
#include <random>
#include "AtspGraph.h"

// Struktura reprezentująca wynik działania algorytmu
struct GeneticResult {
    long long bestCost;
    double averageLastPopulationCost;
    double timeToFindBest; // <--- NOWOŚĆ: Czas znalezienia najlepszego osobnika
};

struct Individual {
    std::vector<int> path;
    long long cost;
};

class GeneticAlgorithm {
private:
    const AtspGraph& graph;

    double timeLimitSeconds;
    int popSize;
    double mutationRate;
    double crossoverRate;
    int selectionMethod; // 1: Turniej, 2: Ruletka
    int crossoverMethod; // 1: OX, 2: PMX
    int mutationMethod;  // 1: Swap, 2: Inwersja

    mutable std::mt19937 gen;

    long long calculateCost(const std::vector<int>& path) const;
    std::vector<Individual> generateInitialPopulation();

    Individual tournamentSelection(const std::vector<Individual>& population);
    Individual rouletteSelection(const std::vector<Individual>& population);

    std::vector<int> crossoverOX(const std::vector<int>& parent1, const std::vector<int>& parent2);
    std::vector<int> crossoverPMX(const std::vector<int>& parent1, const std::vector<int>& parent2);

    void mutateSwap(std::vector<int>& path);
    void mutateInversion(std::vector<int>& path);

public:
    GeneticAlgorithm(const AtspGraph& g);

    void setParameters(double time, int popSize, double mutRate, double crossRate,
                       int selMethod, int crossMethod, int mutMethod);

    GeneticResult solve();
};

#endif // GENETICALGORITHM_H