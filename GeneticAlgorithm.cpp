#include "GeneticAlgorithm.h"
#include "Timer.h" // Twój stary plik Timer.h z Projektu 3
#include <algorithm>
#include <iostream>
#include <numeric>

GeneticAlgorithm::GeneticAlgorithm(const AtspGraph& g) : graph(g) {
    std::random_device rd;
    gen.seed(rd());
}

void GeneticAlgorithm::setParameters(double time, int pSize, double mutRate, double crossRate,
                                     int selMethod, int crossMethod, int mutMethod) {
    timeLimitSeconds = time;
    popSize = pSize;
    mutationRate = mutRate;
    crossoverRate = crossRate;
    selectionMethod = selMethod;
    crossoverMethod = crossMethod;
    mutationMethod = mutMethod;
}

long long GeneticAlgorithm::calculateCost(const std::vector<int>& path) const {
    long long cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cost += graph.getCost(path[i], path[i+1]);
    }
    cost += graph.getCost(path.back(), path[0]); // Powrot do poczatku
    return cost;
}

std::vector<Individual> GeneticAlgorithm::generateInitialPopulation() {
    std::vector<Individual> population;
    int n = graph.getDimension();

    std::vector<int> base(n);
    std::iota(base.begin(), base.end(), 0);

    for (int i = 0; i < popSize; ++i) {
        Individual ind;
        ind.path = base;
        std::shuffle(ind.path.begin() + 1, ind.path.end(), gen);
        ind.cost = calculateCost(ind.path);
        population.push_back(ind);
    }
    return population;
}


Individual GeneticAlgorithm::tournamentSelection(const std::vector<Individual>& population) {
    int k = 5;
    std::uniform_int_distribution<> dis(0, popSize - 1);

    Individual best = population[dis(gen)];
    for (int i = 1; i < k; ++i) {
        Individual competitor = population[dis(gen)];
        if (competitor.cost < best.cost) {
            best = competitor;
        }
    }
    return best;
}

Individual GeneticAlgorithm::rouletteSelection(const std::vector<Individual>& population) {
    double sumFitness = 0.0;
    std::vector<double> fitness(popSize);

    for (int i = 0; i < popSize; ++i) {
        fitness[i] = 1.0 / population[i].cost;
        sumFitness += fitness[i];
    }

    std::uniform_real_distribution<> dis(0.0, sumFitness);
    double randVal = dis(gen);

    double runningSum = 0.0;
    for (int i = 0; i < popSize; ++i) {
        runningSum += fitness[i];
        if (runningSum >= randVal) {
            return population[i];
        }
    }
    return population.back();
}


// OX - Order Crossover (Krzyżowanie porządkowe)
std::vector<int> GeneticAlgorithm::crossoverOX(const std::vector<int>& p1, const std::vector<int>& p2) {
    int n = p1.size();
    std::vector<int> child(n, -1);
    child[0] = p1[0];

    std::uniform_int_distribution<> dis(1, n - 1);
    int a = dis(gen);
    int b = dis(gen);
    if (a > b) std::swap(a, b);

    std::vector<bool> inChild(n, false);
    for (int i = a; i <= b; ++i) {
        child[i] = p1[i];
        inChild[p1[i]] = true;
    }

    int childIdx = (b + 1) % n; if (childIdx == 0) childIdx = 1;
    int p2Idx = (b + 1) % n; if (p2Idx == 0) p2Idx = 1;

    for (int i = 0; i < n - 1; ++i) {
        if (!inChild[p2[p2Idx]]) {
            child[childIdx] = p2[p2Idx];
            childIdx = (childIdx + 1) % n; if (childIdx == 0) childIdx = 1;
        }
        p2Idx = (p2Idx + 1) % n; if (p2Idx == 0) p2Idx = 1;
    }
    return child;
}

// PMX - Partially Mapped Crossover (Krzyżowanie z częściowym mapowaniem)
std::vector<int> GeneticAlgorithm::crossoverPMX(const std::vector<int>& p1, const std::vector<int>& p2) {
    int n = p1.size();
    std::vector<int> child = p2;

    std::uniform_int_distribution<> dis(1, n - 1);
    int a = dis(gen);
    int b = dis(gen);
    if (a > b) std::swap(a, b);

    std::vector<int> indexMap(n, -1);
    for (int i = 0; i < n; ++i) indexMap[child[i]] = i;

    for (int i = a; i <= b; ++i) {
        int val1 = p1[i];
        int val2 = child[i];
        if (val1 != val2) {
            int idxToSwap = indexMap[val1];
            std::swap(child[i], child[idxToSwap]);
            indexMap[child[idxToSwap]] = idxToSwap;
            indexMap[child[i]] = i;
        }
    }
    return child;
}

void GeneticAlgorithm::mutateSwap(std::vector<int>& path) {
    int n = path.size();
    std::uniform_int_distribution<> dis(1, n - 1);
    int i = dis(gen);
    int j = dis(gen);
    std::swap(path[i], path[j]);
}

void GeneticAlgorithm::mutateInversion(std::vector<int>& path) {
    int n = path.size();
    std::uniform_int_distribution<> dis(1, n - 1);
    int a = dis(gen);
    int b = dis(gen);
    if (a > b) std::swap(a, b);
    std::reverse(path.begin() + a, path.begin() + b + 1);
}

GeneticResult GeneticAlgorithm::solve() {
    std::vector<Individual> population = generateInitialPopulation();

    Individual globalBest = population[0];
    for(const auto& ind : population) {
        if(ind.cost < globalBest.cost) globalBest = ind;
    }

    Timer timer;
    std::uniform_real_distribution<> probDist(0.0, 1.0);

    double lastPopAvg = 0.0;
    double bestTime = 0.0;

    while (timer.getElapsedSeconds() < timeLimitSeconds) {
        std::vector<Individual> nextGeneration;
        nextGeneration.reserve(popSize);

        Individual currentBest = population[0];
        for(const auto& ind : population) {
            if(ind.cost < currentBest.cost) currentBest = ind;
        }
        nextGeneration.push_back(currentBest);

        while (nextGeneration.size() < (size_t)popSize) {
            Individual parent1 = (selectionMethod == 1) ? tournamentSelection(population) : rouletteSelection(population);
            Individual parent2 = (selectionMethod == 1) ? tournamentSelection(population) : rouletteSelection(population);

            Individual child;

            if (probDist(gen) < crossoverRate) {
                child.path = (crossoverMethod == 1) ? crossoverOX(parent1.path, parent2.path) : crossoverPMX(parent1.path, parent2.path);
            } else {
                child.path = parent1.path;
            }

            if (probDist(gen) < mutationRate) {
                if (mutationMethod == 1) mutateSwap(child.path);
                else mutateInversion(child.path);
            }

            child.cost = calculateCost(child.path);

            if (child.cost < globalBest.cost) {
                globalBest = child;
                bestTime = timer.getElapsedSeconds();
            }

            nextGeneration.push_back(child);
        }
        population = std::move(nextGeneration);
    }

    long long sumCost = 0;
    for (const auto& ind : population) {
        sumCost += ind.cost;
    }
    lastPopAvg = (double)sumCost / popSize;

    return {globalBest.cost, lastPopAvg, bestTime};
}