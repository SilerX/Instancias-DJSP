#pragma once
#include <vector>
#include <random>
#include "instance.h"

struct Solution {
    std::vector<int> FA;
    std::vector<int> OS;
    int makespan = 0;

    // Memoria histórica (P_best)
    std::vector<int> best_FA;
    std::vector<int> best_OS;
    int best_makespan = 9999999;
};

std::vector<Solution> initPopulation(const Instance& inst, int PS, std::mt19937& rng);
void moverParticulaDPSO(Solution& p, const Solution& g_best, double inercia, std::mt19937& rng);
void aplicarTabuSearch(Solution& g_best, const Instance& inst, std::mt19937& rng);