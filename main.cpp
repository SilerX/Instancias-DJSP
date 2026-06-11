#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <climits>
#include "instance.h"
#include "solution.h"
#include "decode.h"
#include "results.h"
using namespace std;

int main() {
    mt19937 rng(777);
    int PS = 100;

    vector<string> instanceNames;
    for (int i = 1; i <= 80; i++) {
        string num = (i < 10) ? "0" + to_string(i) : to_string(i);
        instanceNames.push_back("ta" + num);
    }

    vector<int> factories = {2, 3, 4};

    for (const string& name : instanceNames) {
        for (int f : factories) {

            string filename = "Instancias-DJSP-main/Instancias-DJSP-main/djsp_instances/"
                              + name + "_f" + to_string(f) + ".txt";

            Instance inst = readInstance(filename);

            vector<Solution> population = initPopulation(inst, PS, rng);
            Solution global_best;
            global_best.makespan = INT_MAX;

            // 1. Evaluación Inicial y Set de P_best / G_best
            for (Solution& sol : population) {
                sol.makespan = decode(sol, inst);
                sol.best_FA = sol.FA;
                sol.best_OS = sol.OS;
                sol.best_makespan = sol.makespan;
                
                if (sol.makespan < global_best.makespan) {
                    global_best = sol;
                }
            }

            // 2. Parámetros del DPSO
            int maxIter = 500; 
            int estancamiento = 0;
            const int LIMITE_ESTANCAMIENTO = 15;

            // 3. Ciclo Principal del Enjambre
            for (int iter = 0; iter < maxIter; iter++) {
                double inercia = 0.9 - (0.8 * iter / maxIter);
                bool mejora_en_iteracion = false;

                // Movimiento de partículas
                for (Solution& p : population) {
                    moverParticulaDPSO(p, global_best, inercia, rng);
                    p.makespan = decode(p, inst);

                    // Actualizar memoria individual (P_best)
                    if (p.makespan < p.best_makespan) {
                        p.best_makespan = p.makespan;
                        p.best_FA = p.FA;
                        p.best_OS = p.OS;
                    }

                    // Actualizar líder (G_best)
                    if (p.makespan < global_best.makespan) {
                        global_best = p;
                        mejora_en_iteracion = true;
                    }
                }

                // 4. Control de Estancamiento y Tabu Search
                if (mejora_en_iteracion) {
                    estancamiento = 0;
                } else {
                    estancamiento++;
                }

                if (estancamiento >= LIMITE_ESTANCAMIENTO) {
                    cout << " [Activando TS en iter " << iter << "] "; // Descomenta para debuggear
                    aplicarTabuSearch(global_best, inst, rng);
                    global_best.makespan = decode(global_best, inst);
                    estancamiento = 0;
                }
            }

            // 5. Guardar el mejor resultado final
            saveResult(name, f, global_best.makespan);
            cout << "Instancia: " << name << "_f" << f << " | Makespan: " << global_best.makespan << "\n";
        }
    }
   
    cout << "Finalizando ejecucion y guardando resultados..." << endl;
    return 0;
}