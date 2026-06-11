#include "results.h"
#include <fstream>
#include <iostream>
#include <set>
#include <string>
using namespace std;

void saveResult(const string& instName, int f, int makespan) {
    static set<int> initialized;

    bool first = (initialized.find(f) == initialized.end());
    auto mode  = first ? (ios::out | ios::trunc) : ios::app;

    string filename = "results_f" + to_string(f) + ".csv";
    ofstream file(filename, mode);

    if (!file) {
        cerr << "Error: no se pudo abrir " << filename << "\n";
        return;
    }

    if (first) {
        file << "instance,makespan\n";
        initialized.insert(f);
    }

    file << instName << "," << makespan << "\n";
}
