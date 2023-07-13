#pragma once
#ifndef __COMPUTEANF_H__
#define __COMPUTEANF_H__

#include<vector>
#include<bitset>
#include<map>
#include"node.h"
#include"gurobi_c++.h"

STATUS computeANF(int rounds, const bitset<80>& cube, const
    bitset<288>& last, map<bitset<288>, int, CMPS<288>>& counterMap, float
    time, int thread);
void readTerms(map<bitset<288>, int, CMPS<288>>& counterMap,set<bitset<288>,CMPS<288>>& terms);
string resolveTerm(const bitset<81>& term);
string resolveTerms(set<bitset<288>, CMPS<288>>& terms);
string resolveTerms(vector<bitset<80>>& terms);

#endif