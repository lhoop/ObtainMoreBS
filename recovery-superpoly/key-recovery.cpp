#pragma once
#include<map>
#include<cmath>
#include<vector>
#include<bitset>
#include<string>
#include<array>
#include<set>
#include<unordered_map>
#include"gurobi_c++.h" 
#include"node.h"
#include"log.h"
#include"triviumtrack.h"
#include"newtrivium.h"
#include"trivium.h"
#include"computeANF.h"

const int THREAD = 64;
const int MAX = 500000; 

using namespace std;

array<set<bitset<288>,CMPS<288>>, 288> r24ANFs;

void preComputeANF(const bitset<80>& cube, int rounds)
{
	for (int i = 0; i < 288; i++)
	{
		bitset<288> last;
		last[i] = 1;
		map<bitset<288>, int, CMPS<288>> counterMap;
		auto status = computeANF(rounds, cube, last, counterMap, 120, 2);
		if (status == EXPAND)
		{
			logger(__func__ + string(" : compute ANFs failed."));
			exit(-1);
		}
		readTerms(counterMap, r24ANFs[i]);
		//cout << resolveTerms(r24ANFs[i]) << endl;
	}
}

void variableSub()
{
	for (int j = 93; j < 105; j++)
	{

		bitset<288> ki;
		ki[69 + j - 93] = 1;

		bitset<288> ki1ki2;
		ki1ki2[68 + j - 93] = 1; ki1ki2[67 + j - 93] = 1;

		bitset<288> kic;
		kic[42 + j - 93] = 1;

		if ((j == 104 || r24ANFs[j].find(ki) != r24ANFs[j].end()) &&
			r24ANFs[j].find(ki1ki2) != r24ANFs[j].end() &&
			r24ANFs[j].find(kic) != r24ANFs[j].end())
		{
			if(j!=104)
				r24ANFs[j].erase(ki);
			r24ANFs[j].erase(ki1ki2);
			r24ANFs[j].erase(kic);

			bitset<288> Xi;
			Xi[69 + j - 93] = 1;
			r24ANFs[j].emplace(Xi);
		}
		else
		{
			logger(__func__ + string(" : variables substitution failed."));
			exit(-1);
		}

	}
}

STATUS kXRecModel(int rounds, const bitset<80>& cube, const bitset<288>& last, map<bitset<81>, int, CMPS<81>>& counterMap,
	double time, int threads)
{

	GRBEnv env = GRBEnv();
	env.set(GRB_IntParam_LogToConsole, 0);
	env.set(GRB_IntParam_Threads, threads);
	env.set(GRB_IntParam_PoolSearchMode, 2);//focus on finding n best solutions
	env.set(GRB_IntParam_PoolSolutions, MAX); // try to find 2000000
	GRBModel model = GRBModel(env);

	// first describe the propagation from k,X bits to the state of round 24
	map<bitset<288>, vector<GRBVar>,CMPS<288>> midcopyList;
	array<vector<GRBVar>, 288> xorList;
	array<vector<GRBVar>, 81> kXcopyList;
	array<vector<GRBVar>, 80>  vcopyList;
	bitset<288> xorFlags;

	for (int i = 0; i < 288; i++)
	{

		for (auto & x : r24ANFs[i])
		{
			if (x.count() > 0)
			{
				GRBVar var = model.addVar(0, 1, 0, GRB_BINARY);
				midcopyList[x].emplace_back(var);
				xorList[i].emplace_back(var);
			}
			else
				xorFlags[i] = 1;
		}
	}

		// generate constraints for xor
	vector<GRBVar> s(288);
	for (int i = 0; i < 288; i++)
	{
		//cout << resolveTerms(r24ANFs[i]) << endl;
		s[i] = model.addVar(0, 1, 0, GRB_BINARY);
		GRBLinExpr exp = 0;
		for (auto& x : xorList[i])
			exp += x;

		if (xorFlags[i])
			model.addConstr(s[i] >= exp);
		else
			model.addConstr(s[i] == exp);

	}

		// generate constraints for midcopy
	for (auto& x : midcopyList)
	{
		GRBVar var = model.addVar(0, 1, 0, GRB_BINARY);
		model.addGenConstrOr(var, &(x.second[0]), x.second.size());

			// kX
		for (int i = 0; i < 81; i++)
			if (x.first[i])
				kXcopyList[i].emplace_back(var);

			// v
		for (int i = 0; i < 80; i++)
			if (x.first[i + 93])
				vcopyList[i].emplace_back(var);
	}

		// generate constraints for copy of kX bits and v bits
	vector<GRBVar> kXv(81 + 80);
	for (int i = 0; i < 81; i++)
	{
		kXv[i] = model.addVar(0, 1, 0, GRB_BINARY);
		model.addGenConstrOr(kXv[i], &(kXcopyList[i][0]), kXcopyList[i].size());
	}

	for (int i = 0; i < 80; i++)
	{
		kXv[i+81] = model.addVar(0, 1, 0, GRB_BINARY);
		if (vcopyList[i].size() == 0 && cube[i] == 0)
			model.addConstr(kXv[i + 81] == 0);
		else if (vcopyList[i].size() > 0 && cube[i] == 1)
		{
			model.addConstr(kXv[i + 81] == 1);
			model.addGenConstrOr(kXv[i + 81], &(vcopyList[i][0]), vcopyList[i].size());
		}
		else
		{
			logger(__func__ + string(" : mismatch detected. "));
			exit(-1);
		}
	}

		// describe the propagation from round 24 to the target round
	vector<GRBVar> works = s;
	for (int r = 24; r < rounds; r++)
	{
		triviumCore(model, works, 65, 170, 90, 91, 92);
		triviumCore(model, works, 161, 263, 174, 175, 176);
		triviumCore(model, works, 242, 68, 285, 286, 287);

		vector<GRBVar> temp = works;
		for (int i = 0; i < 288; i++)
			works[(i + 1) % 288] = temp[i];
	}

	for (int i = 0; i < 288; i++)
		if (last[i] == 1)
			model.addConstr(works[i] == 1);
		else
			model.addConstr(works[i] == 0);

	GRBLinExpr nkX = 0;
	for (int i = 0; i < 81; i++)
		nkX += kXv[i];
	model.setObjective(nkX, GRB_MAXIMIZE);

	if (time > 0)
		model.set(GRB_DoubleParam_TimeLimit, time);

	model.optimize();

	if (model.get(GRB_IntAttr_Status) == GRB_TIME_LIMIT)
	{
		logger(__func__ + string(" : ") + to_string(rounds) +
			string(" | EXPAND "));
		return EXPAND;
	}
	else
	{
		int solCount = model.get(GRB_IntAttr_SolCount);
		if (solCount >= MAX)
		{
			cerr << "solCount value  is too big !" << endl;
			exit(-1);
		}


		if (solCount > 0)
		{
			bitset<81> start;
			for (int i = 0; i < solCount; i++)
			{
				model.set(GRB_IntParam_SolutionNumber, i);
				for (int j = 0; j < 81; j++)
					if (round(kXv[j].get(GRB_DoubleAttr_Xn)) == 1)
						start[j] = 1;
					else
						start[j] = 0;
				counterMap[start]++;
			}
			return SOLUTION;
		}
		else
			return NOSOLUTION;
	}
}

map<int, vector<bitset<80>>> rSubMap;

void preComputeSubMap()
{
	for (int i = 69; i < 81; i++)
	{
		if (i != 80)
		{
			bitset<80> ki;
			ki[i] = 1;
			rSubMap[i].emplace_back(ki);
		}

		bitset<80> ki1ki2;
		ki1ki2[i - 1] = 1; ki1ki2[i - 2] = 1;
		rSubMap[i].emplace_back(ki1ki2);

		bitset<80> kic;
		kic[42 + i - 69] = 1;
		rSubMap[i].emplace_back(kic);
	}
}

void rvariableSub(const bitset<81>& _In, map<bitset<80>, int, CMPS<80>> &_OutMap)
{
	//logger(resolveTerm(_In));
	bitset<81> rMask;
	for (int i = 69; i < 81; i++)
		rMask[i] = 1;

	bitset<81> _sPart = _In & rMask;

	bitset<80> _nsPart;
	for (int i = 0; i < 69; i++)
		_nsPart[i] = _In[i];

	vector<bitset<80>> _sTerms = { _nsPart };
	for (int i = 69; i < 81; i++)
	{
		if (_sPart[i])
			_sTerms = vecMul<80>(_sTerms, rSubMap[i]);
	}

	//logger(resolveTerms(_sTerms));

	for (auto& x : _sTerms)
		_OutMap[x]++;
}

void preComputeForSub(const bitset<80>& cube)
{
	preComputeANF(cube, 24);
	variableSub();
	preComputeSubMap();
}

STATUS kRecModel(int rounds, const bitset<80>& cube, const
	bitset<288>& last, map<bitset<80>, int, CMPS<80>>& counterMap, float
	time, int thread)
{
	map<bitset<81>, int, CMPS<81>> kXcounterMap;
	auto status = kXRecModel(rounds, cube, last, kXcounterMap, time, thread);
	if (status == SOLUTION)
	{
		for (auto& x : kXcounterMap)
		{
			if (x.second % 2)
				rvariableSub(x.first, counterMap);
		}
	}
	return status;
}




