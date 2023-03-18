#!/bin/bash
cd $(dirname "$BASH_SOURCE[0]")

mkdir -p bin && g++ -o3 -o bin/main src/main.cpp -fopenmp && TSP_FILE="./data/test.tsp" bin/main