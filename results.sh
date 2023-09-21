#!/bin/bash
cd $(dirname "$BASH_SOURCE[0]")


export CPLEX_HOME="${CPLEX_HOME:-/opt/ibm/ILOG/CPLEX_Studio2211}"
export CPLEX_MACHINE="${CPLEX_MACHINE:-x86-64_linux/static_pic}"

mkdir -p bin
mkdir -p results

run_testbed(){

  g++ -o3 -D$2 -DNODEBUG -o bin/$1 src/runs.cpp "-I$CPLEX_HOME/cplex/include/"  "-I$CPLEX_HOME/concert/include"  \
      "-L$CPLEX_HOME/concert/lib/$CPLEX_MACHINE" "-L$CPLEX_HOME/cplex/lib/$CPLEX_MACHINE" \
      "-L./concorde" \
      -fopenmp -lconcert -lilocplex -lcplex -lpthread -ldl -l:concorde.a

  echo "running $1..."
  ./bin/$1  "./data/a280.tsp" >    "results/$1-1.txt"
  ./bin/$1  "./data/att48.tsp" >   "results/$1-2.txt"
  ./bin/$1  "./data/att532.tsp" >  "results/$1-3.txt"
  ./bin/$1  "./data/bier127.tsp" > "results/$1-4.txt"
  ./bin/$1  "./data/eil101.tsp" >  "results/$1-5.txt"
  ./bin/$1  "./data/kroA150.tsp" > "results/$1-6.txt"
  ./bin/$1  "./data/kroA200.tsp" > "results/$1-7.txt"
  ./bin/$1  "./data/pr299.tsp" >   "results/$1-8.txt"
  ./bin/$1  "./data/pr439.tsp" >   "results/$1-9.txt"
  ./bin/$1  "./data/pr1002.tsp" >  "results/$1-0.txt"

}

run_testbed annealing RUN_ANNEALING


#g++ -o3 -DRUN_GENETIC=true -DNODEBUG -o bin/genetic src/metaheuristic.cpp "-I$CPLEX_HOME/cplex/include/"  "-I$CPLEX_HOME/concert/include"  \
#    "-L$CPLEX_HOME/concert/lib/$CPLEX_MACHINE" "-L$CPLEX_HOME/cplex/lib/$CPLEX_MACHINE" \
#    "-L./concorde" \
#    -fopenmp -lconcert -lilocplex -lcplex -lpthread -ldl -l:concorde.a
#
#echo "running genetic..."
#./bin/genetic  "./data/a280.tsp" >    "results/genetic1.txt"
#./bin/genetic  "./data/att48.tsp" >   "results/genetic2.txt"
#./bin/genetic  "./data/att532.tsp" >  "results/genetic3.txt"
#./bin/genetic  "./data/bier127.tsp" > "results/genetic4.txt"
#./bin/genetic  "./data/eil101.tsp" >  "results/genetic5.txt"
#./bin/genetic  "./data/kroA150.tsp" > "results/genetic6.txt"
#./bin/genetic  "./data/kroA200.tsp" > "results/genetic7.txt"
#./bin/genetic  "./data/pr299.tsp" >   "results/genetic8.txt"
#./bin/genetic  "./data/pr439.tsp" >   "results/genetic9.txt"
#./bin/genetic  "./data/pr1002.tsp" >  "results/genetic0.txt"


#g++ -o3 -DRUN_VNS=true -DNODEBUG -o bin/vns src/metaheuristic.cpp "-I$CPLEX_HOME/cplex/include/"  "-I$CPLEX_HOME/concert/include"  \
#    "-L$CPLEX_HOME/concert/lib/$CPLEX_MACHINE" "-L$CPLEX_HOME/cplex/lib/$CPLEX_MACHINE" \
#    "-L./concorde" \
#    -fopenmp -lconcert -lilocplex -lcplex -lpthread -ldl -l:concorde.a
#
#echo "running vns..."
#./bin/vns  "./data/a280.tsp" >    "results/vns1.txt"
#./bin/vns  "./data/att48.tsp" >   "results/vns2.txt"
#./bin/vns  "./data/att532.tsp" >  "results/vns3.txt"
#./bin/vns  "./data/bier127.tsp" > "results/vns4.txt"
#./bin/vns  "./data/eil101.tsp" >  "results/vns5.txt"
#./bin/vns  "./data/kroA150.tsp" > "results/vns6.txt"
#./bin/vns  "./data/kroA200.tsp" > "results/vns7.txt"
#./bin/vns  "./data/pr299.tsp" >   "results/vns8.txt"
#./bin/vns  "./data/pr439.tsp" >   "results/vns9.txt"
#./bin/vns  "./data/pr1002.tsp" >  "results/vns0.txt"





#g++ -o3 -DRUN_LOCALBRANCH=true -DNODEBUG -o bin/localbranch src/cplex/localbranch.cpp "-I$CPLEX_HOME/cplex/include/"  "-I$CPLEX_HOME/concert/include"  \
#    "-L$CPLEX_HOME/concert/lib/$CPLEX_MACHINE" "-L$CPLEX_HOME/cplex/lib/$CPLEX_MACHINE" \
#    "-L./concorde" \
#    -fopenmp -lconcert -lilocplex -lcplex -lpthread -ldl -l:concorde.a
#
## echo "running hardfixing..."
#./bin/localbranch  "./data/a280.tsp" >    "results/localbranch1.txt"
#./bin/localbranch  "./data/att48.tsp" >   "results/localbranch2.txt"
#./bin/localbranch  "./data/att532.tsp" >  "results/localbranch3.txt"
#./bin/localbranch  "./data/bier127.tsp" > "results/localbranch4.txt"
#./bin/localbranch  "./data/eil101.tsp" >  "results/localbranch5.txt"
#./bin/localbranch  "./data/kroA150.tsp" > "results/localbranch6.txt"
#./bin/localbranch  "./data/kroA200.tsp" > "results/localbranch7.txt"
#./bin/localbranch  "./data/pr299.tsp" >   "results/localbranch8.txt"
#./bin/localbranch  "./data/pr439.tsp" >   "results/localbranch9.txt"
#./bin/localbranch  "./data/pr1002.tsp" >  "results/localbranch0.txt"

# g++ -o3 -DRUN_HARDFIXING=true -DNODEBUG -o bin/hardfixing src/cplex/hardfixing.cpp "-I$CPLEX_HOME/cplex/include/"  "-I$CPLEX_HOME/concert/include"  \
#     "-L$CPLEX_HOME/concert/lib/$CPLEX_MACHINE" "-L$CPLEX_HOME/cplex/lib/$CPLEX_MACHINE" \
#     "-L./concorde" \
#     -fopenmp -lconcert -lilocplex -lcplex -lpthread -ldl -l:concorde.a
#
# echo "running hardfixing..."
# ./bin/hardfixing  "./data/a280.tsp" >    "results/hardfixing1.txt"
# ./bin/hardfixing  "./data/att48.tsp" >   "results/hardfixing2.txt"
# ./bin/hardfixing  "./data/att532.tsp" >  "results/hardfixing3.txt"
# ./bin/hardfixing  "./data/bier127.tsp" > "results/hardfixing4.txt"
# ./bin/hardfixing  "./data/eil101.tsp" >  "results/hardfixing5.txt"
# ./bin/hardfixing  "./data/kroA150.tsp" > "results/hardfixing6.txt"
# ./bin/hardfixing  "./data/kroA200.tsp" > "results/hardfixing7.txt"
# ./bin/hardfixing  "./data/pr299.tsp" >   "results/hardfixing8.txt"
# ./bin/hardfixing  "./data/pr439.tsp" >   "results/hardfixing9.txt"
# ./bin/hardfixing  "./data/pr1002.tsp" >  "results/hardfixing0.txt"