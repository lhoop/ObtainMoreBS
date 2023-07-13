## Symbols
Please note that we use k0 to k80 represent new variables in our code, all recovered superpolys are for new variables. 

## 0 preparation
install Gurobi   (we used Gurobi 9.5.2)
install SageMath (we used SageMath Version 9.0)
change parameters in line 21 of "exec.sh" based on your Gurobi version 

## 1 Structure of the reference codes
start.py: a starting code used to call other codes, which also contains the process to extract the quadratic or balanced superpolys and the and obtain addtional polynomials by combining quadratic superpolys.

mothercube.cpp : the main function to evaluate the superpoly, you can change the number of rounds or the mothercube in it.

trivium.cpp, deg.cpp, computeANF.cpp,evaluateResTrivium.cpp,key-recovery.cpp,newtrivium.cpp,utrivium.cpp,triviumtrack.cpp,triviumframework.cpp : codes for recoverying superpolys.

result: the folder is for information about superpolys. 'balance:22' represents that k22 is a balanced variable in the superpoly. '20 21 22 64'  represents that this superpoly involves k20,k21,k22,k64. 'allnum:4' represents that this superpoly involves 4 secret variables. 

subcubes.txt: it contains the subcubes to be searched,  '-1 -1 19 11'  represents a subcube with indices I\{19,11}, I is the mother cube indices.


##2 Usage of the codes 

1. Select the number of rounds and a mother cube in mothercube.cpp (line 472 and line 440), put the subcubes you want to search in subcubes.txt. (The default parameter is for 825 rounds).  

2. 'python start.py' or 'nohup python start.py &'

3. The ouput will be displayed in the folder named "result". Some data generated in the process will be displayed in the folder named "tmp".


##TIP 

Please note that our code set a time limit and a limit of the number of solutions for Gurobi, polynomials beyond the limit are considered useless. Therefore, different PC performance may make a difference. Make sure your processor has enough threads or you can relax the time limit. Our search is conducted by Gurobi Solver (version 9.5.2) on a workstation with 2×AMD EPYC 7302 16-core (32 siblings) Processor 3.3 GHz, (total 64 threads), 256G RAM, and Ubuntu 20.10. 
