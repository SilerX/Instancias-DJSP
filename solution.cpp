#include "solution.h"
#include "decode.h"
#include <algorithm>

using namespace std;

vector<Solution> initPopulation(const Instance& inst, int PS, mt19937& rng) {
    vector<Solution> pop(PS);
    uniform_int_distribution<int> factDist(0, inst.f - 1);

    for (Solution& sol : pop) {
        sol.FA.resize(inst.n);
        for (int j = 0; j < inst.n; j++) {
            sol.FA[j] = factDist(rng);
        }

        sol.OS.resize(inst.n * inst.m);
        int k = 0;
        for (int j = 0; j < inst.n; j++) {
            for (int op = 0; op < inst.m; op++) {
                sol.OS[k++] = j;
            }
        }
        shuffle(sol.OS.begin(), sol.OS.end(), rng);

        sol.best_FA = sol.FA;
        sol.best_OS = sol.OS;
        sol.best_makespan = 9999999;
    }
    return pop;
}

void moverParticulaDPSO(Solution& p, const Solution& g_best, double inercia, mt19937& rng) {
    uniform_real_distribution<double> dist(0.0, 1.0);
    double c1 = 0.3; 
    double c2 = 0.5; 

    for (size_t i = 0; i < p.FA.size(); i++) {
        double r = dist(rng);
        if (r > inercia) {
            if (r < inercia + c1) {
                p.FA[i] = p.best_FA[i];
            } else if (r < inercia + c1 + c2) {
                p.FA[i] = g_best.FA[i];
            }
        }
    }

    double r_os = dist(rng);
    if (r_os > inercia) {
        if (dist(rng) < 0.5) {
            p.OS = p.best_OS;
        } else {
            p.OS = g_best.OS;
        }
    } else {
        uniform_int_distribution<int> posDist(0, p.OS.size() - 1);
        int pos1 = posDist(rng);
        int pos2 = posDist(rng);
        swap(p.OS[pos1], p.OS[pos2]);
    }
}

void aplicarTabuSearch(Solution& g_best, const Instance& inst, mt19937& rng) {
    vector<int> cargaFabricas(inst.f, 0);
    for (int j = 0; j < inst.n; j++) {
        int fabrica = g_best.FA[j];
        for (int op = 0; op < inst.m; op++) {
            cargaFabricas[fabrica] += inst.procTime[j][op];
        }
    }

    int fabricaCritica = 0;
    int maxCarga = 0;
    for (int f = 0; f < inst.f; f++) {
        if (cargaFabricas[f] > maxCarga) {
            maxCarga = cargaFabricas[f];
            fabricaCritica = f;
        }
    }

    vector<int> indicesCriticos;
    for (size_t k = 0; k < g_best.OS.size(); k++) {
        int job = g_best.OS[k];
        if (g_best.FA[job] == fabricaCritica) {
            indicesCriticos.push_back(k);
        }
    }
            if (indicesCriticos.size() < 2) return;

    int MAX_TABU_ITER = 50; 
    Solution best_neighbor = g_best;
    uniform_int_distribution<int> idxDist(0, indicesCriticos.size() - 1);

    for (int t = 0; t < MAX_TABU_ITER; t++) {
        Solution neighbor = g_best;
        
        int pos1 = indicesCriticos[idxDist(rng)];
        int pos2 = indicesCriticos[idxDist(rng)];
        
        swap(neighbor.OS[pos1], neighbor.OS[pos2]);
        neighbor.makespan = decode(neighbor, inst);

        if (neighbor.makespan < best_neighbor.makespan) {
            best_neighbor = neighbor;
            break; 
        }
    }

    if (best_neighbor.makespan < g_best.makespan) {
        g_best = best_neighbor;
    }
}