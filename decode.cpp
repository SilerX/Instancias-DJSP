#include "decode.h"
#include "solution.h"
#include <algorithm>
#include <vector>
#include <set>
using namespace std;

static int findGap(const vector<pair<int,int>>& intervals, int earliest, int dur) {
    if (intervals.empty()) {
        return earliest;
    }
    if (intervals[0].first - earliest >= dur) {
        return earliest;
    }
    for (int i = 0; i + 1 < (int)intervals.size(); i++) {
        int gapStart = max(intervals[i].second, earliest);
        int gapEnd   = intervals[i + 1].first;
        if (gapEnd - gapStart >= dur) {
            return gapStart;
        }
    }
    return max(intervals.back().second, earliest);
}

int decode(const Solution& sol, const Instance& inst) {
    int globalMakespan = 0;

    for (int fk = 0; fk < inst.f; fk++) {
        set<int> factorySet;
        for (int j = 0; j < inst.n; j++) {
            if (sol.FA[j] == fk) {
                factorySet.insert(j);
            }
        }

        if (factorySet.empty()) { continue; }

        vector<int> factOS;
        factOS.reserve(factorySet.size() * inst.m);
        for (int job : sol.OS) {
            if (factorySet.count(job)) {
                factOS.push_back(job);
            }
        }

        vector<vector<pair<int,int>>> machIntervals(inst.m);
        vector<int> jobEnd(inst.n, 0);
        vector<int> opIdx(inst.n, 0);

        for (int job : factOS) {
            int op      = opIdx[job];
            int machine = inst.machineOrder[job][op];
            int dur     = inst.procTime[job][op];

            int start = findGap(machIntervals[machine], jobEnd[job], dur);

            auto it = lower_bound(machIntervals[machine].begin(),
                                  machIntervals[machine].end(),
                                  make_pair(start, start + dur));
            machIntervals[machine].insert(it, {start, start + dur});

            jobEnd[job] = start + dur;
            opIdx[job]++;
        }

        int factMakespan = 0;
        for (const auto& mach : machIntervals) {
            if (!mach.empty()) {
                factMakespan = max(factMakespan, mach.back().second);
            }
        }

        globalMakespan = max(globalMakespan, factMakespan);
    }

    return globalMakespan;
}
