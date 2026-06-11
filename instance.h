#pragma once
#include <vector>
#include <string>
using namespace std;
struct Instance {
    int n;  // jobs
    int m;  // machines
    int f;  // factories
    vector<std::vector<int>> procTime;      // procTime[j][op]     = tiempo de procesamiento
    vector<std::vector<int>> machineOrder;  // machineOrder[j][op] = maquina (0-indexed)
};

Instance readInstance(const std::string& filename);
