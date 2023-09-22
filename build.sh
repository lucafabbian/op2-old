#!/bin/bash
cd $(dirname "$BASH_SOURCE[0]")


export CPLEX_HOME="${CPLEX_HOME:-/opt/ibm/ILOG/CPLEX_Studio2211}"
export CPLEX_MACHINE="${CPLEX_MACHINE:-x86-64_linux/static_pic}"

mkdir -p bin
g++ -o3 -o bin/main src/main.cpp "-I$CPLEX_HOME/cplex/include/"  "-I$CPLEX_HOME/concert/include"  \
    "-L$CPLEX_HOME/concert/lib/$CPLEX_MACHINE" "-L$CPLEX_HOME/cplex/lib/$CPLEX_MACHINE" \
    -fopenmp -lconcert -lilocplex -lcplex -lpthread -ldl
