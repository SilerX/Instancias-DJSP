#include "instance.h"
#include <fstream>
#include <iostream>
using namespace std;

Instance readInstance(const string& filename) {
    Instance inst{};
    ifstream file(filename);
    if (!file) {
        cerr << "Error: no se pudo abrir " << filename << "\n";
        return inst;
    }

    file >> inst.n >> inst.m >> inst.f;

    inst.procTime.assign(inst.n, vector<int>(inst.m));
    inst.machineOrder.assign(inst.n, vector<int>(inst.m));


    //Hermosa lectura, salto a la siguiente palabra todo el rato nomas care palo. Con limites claros en cuanto dura/SALTOS una linea segun "m"
    for (int j = 0; j < inst.n; j++) {
        for (int i = 0; i < inst.m; i++) {
            file >> inst.procTime[j][i];
        }
    }

    for (int j = 0; j < inst.n; j++) {
        for (int i = 0; i < inst.m; i++) {
            file >> inst.machineOrder[j][i];
        }
    }

    return inst;
}
