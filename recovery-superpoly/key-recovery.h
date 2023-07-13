#pragma once
#ifndef __KEYREC_H__
#define __KEYREC_H__

#include<vector>
#include<bitset>
#include<map>
#include"node.h"
#include"gurobi_c++.h"

STATUS kXRecModel(int rounds, const bitset<80>& cube, const bitset<288>& last, map<bitset<81>, int, CMPS<81>>& counterMap,
	double time, int threads);
void preComputeForSub(const bitset<80>& cube);
STATUS kRecModel(int rounds, const bitset<80>& cube, const
	bitset<288>& last, map<bitset<80>, int, CMPS<80>>& counterMap, float
	time, int thread);

#endif