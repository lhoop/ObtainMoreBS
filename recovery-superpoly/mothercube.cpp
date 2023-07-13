#include<iostream>
#include<fstream>
#include<vector>
#include<bitset>
#include<algorithm>
#include<thread>
#include<mutex>
#include<future>
#include<sstream>
#include<malloc.h>
#include<set>
#include<array>
#include"log.h"
#include"deg.h"
#include"thread_pool.h"
#include"trivium.h"
#include"triviumtrack.h"
#include"newtrivium.h"
#include"triviumcallback.h"
#include"triviumframework.h"
#include"key-recovery.h"

using namespace std;
using namespace thread_pool;
mutex stream_mutex;
mutex map_mutex;
mutex term_mutex;
int THREAD_NUM = 64;

array<vector<bitset<80>>, 288> conskCofs;
array<vector<bitset<81>>, 288> conskXCofs;
void variableSub(vector<bitset<80>>& _In, vector<bitset<81>>& _Out)
{
	set<bitset<80>,CMPS<80>> _InSet(_In.begin(), _In.end());
	for (int i = 69; i < 81; i++)
	{
		bitset<80> ki;
		ki[i] = 1;

		bitset<80> ki1ki2;
		ki1ki2[i - 1] = 1;
		ki1ki2[i - 2] = 1;

		bitset<80> kic;
		kic[42 + i - 69] = 1;

		if ((i == 80 || _InSet.find(ki) != _InSet.end()) &&
			_InSet.find(ki1ki2) != _InSet.end() &&
			_InSet.find(kic) != _InSet.end())
		{
			if (i != 80)
				_InSet.erase(ki);
			_InSet.erase(ki1ki2);
			_InSet.erase(kic);

			bitset<81> Xi;
			Xi[i] = 1;
			_Out.emplace_back(Xi);
		}

	}

	for (auto& x : _InSet)
	{
		for(int i = 69;i < 80;i++)
			if (x[i])
			{
				logger(__func__ + string(" : variables substitution error."));
				exit(-1);
			}

		bitset<81> ki;
		for (int i = 0; i < 69; i++)
			ki[i] = x[i];
		_Out.emplace_back(ki);
	}
}

void resolveTerms(vector<bitset<80>>& kcofs, stringstream& ss);
void resolveTerms(vector<bitset<81>>& kXcofs, stringstream& ss);

void preCompute(int rounds, bitset<80>& cube)
{
	for (int i = 0; i < 288; i++)
		conskCofs[i].clear();
	for(int i =0;i<288;i++)
		conskXCofs[i].clear();

	vector<bitset<288>> nonZeroTrackReps;
	triviumTrack(cube, nonZeroTrackReps);


	vector<bitset<288>> varsTrackReps;
	triviumVarsTrack(cube, varsTrackReps, nonZeroTrackReps);

	bitset<288> nonZeroFlag, varsFlag;
	setRoundFlags(rounds, cube, nonZeroFlag, varsFlag, nonZeroTrackReps, varsTrackReps);

	for (int i = 0; i < 288; i++)
		if (nonZeroFlag[i] == 1 && varsFlag[i] == 0)
		{
			map<bitset<80>, int, CMPS<80>> kcofMap;
			vector<bitset<80>> kcofs;
			bitset<288> lastState;
			lastState[i] = 1;
			bitset<80> cube0;
			MidSolutionCounter(rounds, cube0, lastState, kcofMap, 120, 2);
			for (auto& x : kcofMap)
				if (x.second % 2)
					kcofs.emplace_back(x.first);
			conskCofs[i] = kcofs;

			variableSub(kcofs,conskXCofs[i]);
		}

	logger("Precomputation for round " + to_string(rounds) + " Completed.");
}



void printSol(int rounds, const bitset<288>& vector, const map<bitset<80>,
	int, CMPS<80>> &solutions)
{
	logger(__func__ + string(": ") + to_string(rounds) + string("\t") +
		vector.to_string());

	string path = string("tmp/TERM/") + to_string(rounds) + string(".txt");
	ofstream os;
	os.open(path, ios::out | ios::app);
	os << rounds << "\t" << vector << endl;
	for (auto & it : solutions)
		os << it.first << "\t" << it.second << "\n";
	os << endl;
	os.close();
}

void resolveTerms(vector<bitset<80>>& kcofs, stringstream& ss)
{
	auto sep = "";
	ss << "(";
	for (auto& x : kcofs)
	{
		ss << sep;
		if (x.count() == 0)
			ss << "1";
		else
			for (int i = 0; i < 80; i++)
				if (x[i])
					ss << "k" << i;

		sep = "+";
	}
	ss << ")";
}

void resolveTerms(vector<bitset<81>>& kXcofs, stringstream& ss)
{
	auto sep = "";
	ss << "(";
	for (auto& x : kXcofs)
	{
		ss << sep;
		if (x.count() == 0)
			ss << "1";
		else
			for (int i = 0; i < 81; i++)
				if (x[i])
					ss << (i >= 69 ? 'X' : 'k') << i;

		sep = "+";
	}
	ss << ")";
}


void filterTerms(const bitset<80>& cube, int rounds, vector<bitset<288>>& terms)
{
	int size0 = terms.size();
	vector<bitset<288>> tmp(terms.begin(), terms.end());
	terms.clear();
	for (auto& it : tmp)
	{
		auto d = computeDegree(cube, rounds, it);
		if (d >= cube.count())
			terms.push_back(it);
	}
	int size1 = terms.size();

	logger(__func__ + string(": ") + to_string(size0) + string("\t") +
		to_string(size1));
}

// this function is only called from SolutionFunc to get solutions from counterMap
void SecondSolutionFunc(int rounds,int midround, bitset<80>& cube, map<bitset<288>, int, CMPS<288> >& counterMap, map<bitset<80>, int, CMPS<80> >& solutions, vector<bitset<288>>& expandTerms, float time, int singlethread)
{
	solutions.clear();

	// first sort out midsolutions
	vector<bitset<288> > midsolutions;
	for (auto& x : counterMap)
	{
		if (x.second % 2 == 1)
			midsolutions.emplace_back(x.first);
		else
		{
			int midsols = 0;
			auto status = newMidSolutionCounter(midround, cube, x.first, midsols, time, singlethread);
			if (status == EXPAND || status == NOSOLUTION)
			{
				logger(__func__ + string(" : midsolution count error."));
				exit(-1);
			}

			int sols = x.second;
			if ((sols / midsols) % 2 == 1)
				midsolutions.emplace_back(x.first);
		}
	}

	// get final solutions from midsolutions
	// nonzero track
	vector<bitset<288>> nonZeroTrackReps;
	triviumTrack(cube, nonZeroTrackReps);

	// variables track
	vector<bitset<288>> varsTrackReps;
	triviumVarsTrack(cube, varsTrackReps, nonZeroTrackReps);

	bitset<288> nonZeroFlag;
	bitset<288> varsFlag;
	setRoundFlags(midround, cube, nonZeroFlag, varsFlag, nonZeroTrackReps, varsTrackReps);

	stringstream ss;

	for (auto& x : midsolutions)
	{

		
	
		bitset<288> midcube;
		for (int i = 0; i < 288; i++)
			if (varsFlag[i])
				midcube[i] = x[i];
		map<bitset<81>, int, CMPS<81>> midcubesols;
		auto status = kXRecModel(midround, cube, midcube, midcubesols, 120, 2);
		if (status == NOSOLUTION)
		{
			logger(__func__ + string(" : get final solutions error."));
			exit(-1);
		}
		else if (status == EXPAND)
		{
			logger(__func__ + string(" expandTerm added : ") + x.to_string());

			lock_guard<mutex> guard(term_mutex);
			expandTerms.emplace_back(x);
			continue;
		}

		vector<bitset<81>> midcubekcofs;
		for (auto& y : midcubesols)
			if (y.second % 2)
				midcubekcofs.emplace_back(y.first);

		if (midcubekcofs.size() > 0)
		{
			resolveTerms(midcubekcofs, ss);

			for (int i = 0; i < 288; i++)
				if (nonZeroFlag[i] && varsFlag[i] == 0 && x[i])
				{
					ss << "*";
					resolveTerms(conskXCofs[i], ss);
				}

			ss << endl;
		}
				
	}

	loggerStream(ss, rounds);

}


STATUS SolutionFunc(int rounds, int midround, bitset<80>& cube, bitset<288>& last, map<bitset<80>, int, CMPS<80>>& solutions, vector<bitset<288>>& expandTerms, float time, int singlethread)
{
	if (midround > rounds || rounds < 0 || midround < 0)
	{
		logger(__func__ + string(" : parameter error."));
		exit(-1);
	}

	map<bitset<288>, int, CMPS<288> > counterMap;
	auto status = newMidSolutionCounter(rounds, cube, last, counterMap, time, singlethread, midround);

	if (status == SOLUTION)
		SecondSolutionFunc(rounds,midround, cube, counterMap, solutions, expandTerms, time, singlethread);

	return status;
}

// below is threadworker
void SolutionSearcherWorker(bitset<288>& vec, int ROUND, bitset<80>& cube,
	vector<bitset<288>>& layerTerms, float time, int singlethread, int midround, map<bitset<80>, int, CMPS<80>>& totalSolutions, vector<bitset<288>>& expandTerms)
{
	map<bitset<80>, int, CMPS<80>> solutions;

	auto status = SolutionFunc(ROUND, midround, cube, vec, solutions, expandTerms, time, singlethread);


	if (status == EXPAND)
	{
		lock_guard<mutex> guard(map_mutex);
		layerTerms.push_back(vec);
	}
	else if (status == SOLUTION)
	{
		/*lock_guard<mutex> guard(stream_mutex);
		for (auto& x : solutions)
			totalSolutions[x.first] = (totalSolutions[x.first] + x.second) % 2;*/
		//loggerStream(ss, ROUND);
	}
}



void testSearchWorkerFunc(int cnt, int rounds, bitset<80>& cube, bitset<288>  last, vector<bitset<80> > res, vector<bitset<288> >& expandTerms, double time, int singlethread, int midround)
{
	map<bitset<80>, int, CMPS<80>> solutions;
	auto status = SolutionFunc(rounds, midround, cube, last, solutions, expandTerms, time, singlethread);
	

	if (status == EXPAND || status == NOSOLUTION)
	{
		logger(__func__ + string(" : SolutionFunc status error."));
		logger("error num : " + to_string(cnt));
		exit(-1);
	}


	vector<bitset<80>> res2;

	for (auto& x : solutions)
		if (x.second % 2 == 1)
			res2.emplace_back(x.first);

	// compare
	sort(res.begin(), res.end(), CMP<80>);
	sort(res2.begin(), res2.end(), CMP<80>);

	if (!vecCmp<80>(res, res2))
	{
		logger("term" + to_string(cnt) + " test failed.");
		logger("failed rounds:" + to_string(rounds));
		logger("failed laststate: " + last.to_string());
		exit(-1);

		
	}
	else
	{
		logger("term" + to_string(cnt) + " test succeed.");
	}
}

void ReadResAndCount(fstream& fs, vector<bitset<80> >& cofs)
{
	string oneline;
	bitset<80> cof;
	int cnt;
	while (getline(fs, oneline))
	{
		if (!oneline.empty())
		{
			stringstream ss;
			ss << oneline;
			ss >> cof;
			ss >> cnt;
			if (cnt % 2)
				cofs.push_back(cof);
		}
		else
			break;
	}
}

void testRes(bitset<80>& testCube, int testmidround, string& filepath)
{
	fstream fs;
	fs.open(filepath);

	string oneline;
	int rounds;
	int cnt = 0;
	ThreadPool testthread_pool;
	vector<std::future<void>> testfutures;
	vector<bitset<288> > expandTerms;
	while (getline(fs, oneline))
	{

		string lastStateStr;
		stringstream ss;
		ss << oneline;
		ss >> rounds;
		ss >> lastStateStr;


		vector<bitset<80>> cofs;
		ReadResAndCount(fs, cofs);



		bitset<288> lastState(lastStateStr);
		testfutures.emplace_back(testthread_pool.Submit(testSearchWorkerFunc, cnt, rounds, ref(testCube), lastState, cofs, ref(expandTerms), 720, 2, testmidround));
		cnt++;
	}

	for (auto& it : testfutures)
		it.get();

	if (expandTerms.size() > 0)
		cout << "expandTerms detected." << endl;

}


int main(int argc, const char * argv[])
{



	int decube0 =  atoi(argv[1]); 
	int decube1 =  atoi(argv[2]);
	int decube2 =  atoi(argv[3]);
	int decube3 =  atoi(argv[4]);

	
	//825 I2 size 53 
	vector<int> cubeIndex = {2,5,8,10,12,15,17,19,23,29,31,41,44,46,51,55,63,66,72,78,3,0,69,6,26,7,50,68,25,48,33,4,21,76,36,16,14,37,38,39,59,61,18,53,34,74,40,1,57,9,13,22,35};
	
	
	
	//810 I1 size 44
	//vector<int> cubeIndex = {0,2,4,6,8,10,11,12,14,15,17,19,20,21,22,23,25,27,29,30,32,34,36,37,39,41,43,45,47,50,52,54,55,60,62,64,70,72,75,79,67,57,53,48};




	bitset<80> cube{ 0 };
	for (auto& it : cubeIndex)
		cube[it] = 1;

	if (decube0 >= 0)
		cube[decube0] = 0;
	if (decube1 >= 0)
		cube[decube1] = 0;
	if (decube2 >= 0)
		cube[decube2] = 0;
	if (decube3 >= 0)
		cube[decube3] = 0;


	cout << cube << endl;

	
	preComputeForSub(cube);
	int midround = 90;
	ThreadPool thread_pool{};


	int ROUND = 825;
	//int ROUND = 810;
	int step = 300;
	int expandStep = 120;
	float expandTime = 60;

	float time = 0;

	vector<bitset<288>> terms;
	vector<bitset<288>> layerTerms;

	BackExpandPolynomial(step, terms);

	ROUND = ROUND - step;
	filterTerms(cube, ROUND, terms);

	logger(__func__ + string("First expand"));

	int singlethread = 2;


	map<bitset<80>, int, CMPS<80>> totalSolutions;
	vector<bitset<288> > expandTerms;

	if (ROUND < CALLBACKBOUND)
		newExpandFunc(cube, terms, ROUND, expandStep, thread_pool, expandTime, singlethread);
	else
		expandFunc(cube, terms, ROUND, expandTime, singlethread);


	while (true)
	{

		if (ROUND > 600)
			time = 20;
		else if (ROUND > 500)
			time = 20;
		else if (ROUND > 400)
			time = 10;
		else if (ROUND > 300)
			time = 10;
		else if (ROUND > 250)
			time = 10;
		else if (ROUND > 100)
			time = 1;
		else if (ROUND > 20)
			time = 1;
		else
			time = 1;


		layerTerms.clear();
		preCompute(midround, cube);
		vector<std::future<void>> futures;
		for (auto& it : terms)
			futures.emplace_back(thread_pool.Submit(SolutionSearcherWorker, it,
				ROUND, ref(cube), ref(layerTerms), time, singlethread, midround, ref(totalSolutions), ref(expandTerms)));
		for (auto& it : futures)
			it.get();

		filterMap<80>(totalSolutions);
		filterExpandTerms<288>(expandTerms);

#ifdef _WIN32
#else
		showProcessMemUsage();
		malloc_trim(0);
		showProcessMemUsage();
#endif


		logger(string("layerTermsSize ") + to_string(layerTerms.size()));

		logger(string("expandTermsSize ") + to_string(expandTerms.size()));
		if (layerTerms.size() == 0 && expandTerms.size() == 0)
			break;
		else if (layerTerms.size() == 0)
		{
			logger("Start to handle expandTerms.");
			ROUND = midround;
			midround = ROUND / 2;
			layerTerms = expandTerms;
			filterTerms(cube, ROUND, layerTerms);
			expandTerms.clear();
		}

		if (ROUND < 10)
		{
			logger("ROUND is to small.");
			exit(-1);
		}

		terms = layerTerms;


		if (ROUND < FASTBOUND)
			expandStep = 20;

		if (ROUND - expandStep < midround)
			expandStep = ROUND - midround;


		if (ROUND < CALLBACKBOUND)
			newExpandFunc(cube, terms, ROUND, expandStep, thread_pool, expandTime, singlethread);
		else
			expandFunc(cube, terms, ROUND, expandTime, singlethread);


		if (ROUND == midround)
		{
			logger("current ROUND : " + to_string(ROUND));
			logger("start combine expandTerms with terms.");
			map<bitset<288>, int, CMPS<288>> termCounter;
			for (auto& x : terms)
				termCounter[x] ++;
			for (auto& x : expandTerms)
				termCounter[x] ++;

			expandTerms.clear();
			terms.clear();
			for (auto& x : termCounter)
				if (x.second % 2 == 1)
					terms.emplace_back(x.first);

			midround = ROUND / 2;
		}


	}
	cout << "Success!" << endl;
	
    char tmpdirname[255];
	fstream fs2;
	sprintf(tmpdirname, "result/superpoly/search_log.txt");
	fs2.open(tmpdirname, ios::app);
	fs2<< "Success!"<< endl;
	fs2.close();

}

