export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"
rm nohup.out
rm -rf tmp
mkdir result
mkdir result/superpoly
mkdir result/superpoly/useless_superpolys
mkdir result/superpoly/may_valuable_superpolys
mkdir tmp
mkdir tmp/TERM
mkdir tmp/STATE
mkdir tmp/LOG
mkdir tmp/sage
mkdir tmp/sage/after_add
mkdir tmp/sage/after_reduce_first
mkdir tmp/sage/after_reduce_second
mkdir tmp/sage/after_reduce_third
mkdir tmp/sage/after_split
mkdir tmp/quadratic
mkdir tmp/may_valuable_superpolys

g++  mothercube.cpp computeANF.cpp key-recovery.cpp deg.cpp trivium.cpp newtrivium.cpp triviumcallback.cpp triviumtrack.cpp triviumframework.cpp utrivium.cpp -o trivium -std=c++2a -O3 -lm -lpthread -I/$GUROBI_HOME/include/ -L/$GUROBI_HOME/lib -lgurobi_c++ -lgurobi95 -lm

g++ evaluateResTrivium.cpp -std=c++2a -O3 -lpthread -o evaluateRes
