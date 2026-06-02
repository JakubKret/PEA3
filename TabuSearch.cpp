#include "TabuSearch.h"
#include "Timer.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <random>
#include <queue>

TabuSearch::TabuSearch(const AtspGraph& g)
    : graph(g), timeLimitSeconds(10.0), tabuTenure(15), maxTabuListSize(100), useAspiration(true) {
    neighborsToGenerate = g.getDimension() * 10;
}

void TabuSearch::setTimeLimit(double seconds) { timeLimitSeconds = seconds; }
void TabuSearch::setTabuTenure(int tenure) { tabuTenure = tenure; }
void TabuSearch::setMaxTabuListSize(int size) { maxTabuListSize = size; }
void TabuSearch::setNeighborsToGenerate(int neighbors) { neighborsToGenerate = neighbors; }
void TabuSearch::setAspiration(bool enable) { useAspiration = enable; }

long long TabuSearch::calculateCost(const std::vector<int>& path) const {
    long long cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cost += graph.getCost(path[i], path[i+1]);
    }
    cost += graph.getCost(path.back(), path[0]);
    return cost;
}

Solution TabuSearch::solveNNFromNode(int startNode) const {
    int n = graph.getDimension();
    std::vector<bool> visited(n, false);
    Solution sol;
    sol.totalCost = 0;

    int current = startNode;
    sol.path.push_back(current);
    visited[current] = true;

    for (int step = 1; step < n; ++step) {
        int next_node = -1;
        int min_cost = std::numeric_limits<int>::max();

        for (int j = 0; j < n; ++j) {
            if (!visited[j] && graph.getCost(current, j) < min_cost) {
                min_cost = graph.getCost(current, j);
                next_node = j;
            }
        }
        visited[next_node] = true;
        sol.path.push_back(next_node);
        current = next_node;
    }
    sol.totalCost = calculateCost(sol.path);
    return sol;
}

Solution TabuSearch::generateInitialSolutionRNN() const {
    int n = graph.getDimension();
    Solution bestSolution;
    bestSolution.totalCost = std::numeric_limits<long long>::max();

    for (int i = 0; i < n; ++i) {
        Solution candidate = solveNNFromNode(i);
        if (candidate.totalCost < bestSolution.totalCost) {
            bestSolution = candidate;
        }
    }
    return bestSolution;
}

Solution TabuSearch::generateRandomSolution() const {
    int n = graph.getDimension();
    Solution sol;
    for (int i = 0; i < n; ++i) sol.path.push_back(i);

    std::random_device rd;
    std::mt19937 g(rd());
    // Mieszamy wszystko oprocz wierzcholka startowego (0)
    std::shuffle(sol.path.begin() + 1, sol.path.end(), g);

    sol.totalCost = calculateCost(sol.path);
    return sol;
}

Solution TabuSearch::solve(const Solution& initialSolution) {
    int n = graph.getDimension();
    Solution currentSolution = initialSolution;
    Solution bestSolution = currentSolution;

    std::vector<std::vector<int>> tabuList(n, std::vector<int>(n, 0));
    std::queue<std::pair<int, int>> tabuHistory;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);

    Timer timer;
    timer.start();

    int iter = 0;
    while (timer.getElapsedSeconds() < timeLimitSeconds) {
        int bestSwapA = -1, bestSwapB = -1;
        long long bestNeighborhoodCost = std::numeric_limits<long long>::max();
        std::vector<int> bestNeighborhoodPath;

        for (int k = 0; k < neighborsToGenerate; ++k) {
            int i = dis(gen);
            int j = dis(gen);
            while (i == j) { j = dis(gen); }
            if (i > j) std::swap(i, j);

            std::vector<int> candidatePath = currentSolution.path;
            std::swap(candidatePath[i], candidatePath[j]);
            long long candidateCost = calculateCost(candidatePath);

            int cityA = currentSolution.path[i];
            int cityB = currentSolution.path[j];

            bool isTabu = tabuList[cityA][cityB] > iter;
            bool aspiration = useAspiration ? (candidateCost < bestSolution.totalCost) : false;

            if (!isTabu || aspiration) {
                if (candidateCost < bestNeighborhoodCost) {
                    bestNeighborhoodCost = candidateCost;
                    bestNeighborhoodPath = candidatePath;
                    bestSwapA = cityA;
                    bestSwapB = cityB;
                }
            }
        }

        if (bestSwapA != -1 && bestSwapB != -1) {
            currentSolution.path = bestNeighborhoodPath;
            currentSolution.totalCost = bestNeighborhoodCost;

            if (currentSolution.totalCost < bestSolution.totalCost) {
                bestSolution = currentSolution;
            }

            tabuList[bestSwapA][bestSwapB] = iter + tabuTenure;
            tabuList[bestSwapB][bestSwapA] = iter + tabuTenure;
            tabuHistory.push({bestSwapA, bestSwapB});

            if (tabuHistory.size() > (size_t)maxTabuListSize) {
                auto oldest = tabuHistory.front();
                tabuHistory.pop();
                tabuList[oldest.first][oldest.second] = 0;
                tabuList[oldest.second][oldest.first] = 0;
            }
        }
        iter++;
    }
    return bestSolution;
}