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

const int THREAD = 64;
const int MAX = 500000; 

using namespace std;



STATUS computeANF(int rounds,  const bitset<80> & cube,const
    bitset<288>& last, map<bitset<288>, int, CMPS<288>>& counterMap, float
    time, int thread)
{
    GRBEnv env = GRBEnv();
    env.set(GRB_IntParam_LogToConsole, 0);
    env.set(GRB_IntParam_Threads, thread);
    env.set(GRB_IntParam_PoolSearchMode, 2);//focus on finding n best solutions
    env.set(GRB_IntParam_MIPFocus, 3);
    env.set(GRB_IntParam_PoolSolutions, MAX); // try to find 2000000
    GRBModel model = GRBModel(env);

    vector<GRBVar> s(288);
    for (int i = 0; i < 288; i++)
        s[i] = model.addVar(0, 1, 0, GRB_BINARY);

    for (int i = 0; i < 80; i++)
        if (cube[i] == 0)
            model.addConstr(s[i + 93] == 0);

    // key, last three bits
    for (int i = 80; i < 93; i++)
        model.addConstr(s[i] == 0);
    for (int i = 93 + 80; i < 285; i++)
        model.addConstr(s[i] == 0);

    vector<GRBVar> works = s;
    for (int r = 0; r < rounds; r++)
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

    GRBLinExpr nk = 0;
    for (int i = 0; i < 80; i++)
        nk += s[i];
    model.setObjective(nk, GRB_MAXIMIZE);

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
            bitset<288> start;
            for (int i = 0; i < solCount; i++)
            {
                model.set(GRB_IntParam_SolutionNumber, i);
                for (int j = 0; j < 285; j++)
                    if (round(s[j].get(GRB_DoubleAttr_Xn)) == 1)
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


void readTerms(map<bitset<288>, int, CMPS<288>>& counterMap, set<bitset<288>,CMPS<288>>& terms)
{
    terms.clear();

    map<bitset<288>, int, CMPS<288>> termMap;
    for (auto& x : counterMap)
    {
        bitset<288> term;
        for (int i = 0; i < 80; i++)
        {
            term[i] = x.first[i];
            term[93 + i] = x.first[i + 93];
        }
        termMap[term] += x.second;
    }

    for (auto& x : termMap)
        if (x.second % 2)
            terms.emplace(x.first);
}

string resolveTerm(const bitset<288>& term)
{
    if (term.count() == 0)
        return "1";

    string termStr;
    for (int i = 0; i < 81; i++)
        if (term[i])
            termStr += "k" + to_string(i);

    for (int i = 0; i < 80; i++)
        if (term[i + 93])
            termStr += "v" + to_string(i);

    return termStr;
}

string resolveTerm(const bitset<81>& term)
{
    if (term.count() == 0)
        return "1";

    string termStr;
    for (int i = 0; i < 81; i++)
        if (term[i])
            termStr += (i >= 69 ? 'X' : 'k') + to_string(i);

    return termStr;
}

string resolveTerm(const bitset<80>& term)
{
    if (term.count() == 0)
        return "1";

    string termStr;
    for (int i = 0; i < 81; i++)
        if (term[i])
            termStr += 'k'+ to_string(i);

    return termStr;
}

string resolveTerms(set<bitset<288>,CMPS<288>>& terms)
{
    if (terms.size() == 0)
        return "0";

    string termsStr;
    auto sep = "";
    for (auto& x : terms)
    {
        termsStr += sep + resolveTerm(x);
        sep = "+";
    }

    return termsStr;
}

string resolveTerms(vector<bitset<80>>& terms)
{
    if (terms.size() == 0)
        return "0";

    string termsStr;
    auto sep = "";
    for (auto& x : terms)
    {
        termsStr += sep + resolveTerm(x);
        sep = "+";
    }

    return termsStr;
}
